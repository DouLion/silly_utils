
import re
import sys

# Proto 基础类型到 C++ 类型和 Protozero 方法的映射
PROTO_TO_CPP = {
    'double': 'double', 'float': 'float',
    'int32': 'int32_t', 'int64': 'int64_t',
    'uint32': 'uint32_t', 'uint64': 'uint64_t',
    'sint32': 'int32_t', 'sint64': 'int64_t',
    'bool': 'bool', 'string': 'std::string', 'bytes': 'std::string'
}

PBF_MAP = {
    'double': 'double', 'float': 'float',
    'int32': 'int32', 'int64': 'int64',
    'uint32': 'uint32', 'uint64': 'uint64',
    'bool': 'bool', 'string': 'string', 'bytes': 'string'
}

def get_cpp_default(cpp_type):
    if cpp_type in ['float', 'double']: return '0.0f'
    if cpp_type in ['int32_t', 'uint32_t', 'int64_t', 'uint64_t']: return '0'
    if cpp_type == 'bool': return 'false'
    return ''

def parse_proto(content):
    # 清理注释和格式化符号以便分词
    content = re.sub(r'//.*', '', content)
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    content = content.replace('{', ' { ').replace('}', ' } ').replace(';', ' ; ')
    content = content.replace('<', ' < ').replace('>', ' > ').replace('=', ' = ').replace(',', ' , ')
    tokens = content.split()

    messages = []
    scope_stack = []

    i = 0
    while i < len(tokens):
        t = tokens[i]
        if t in ['syntax', 'package']:
            while tokens[i] != ';': i += 1
            i += 1
        elif t == 'message':
            msg_name = tokens[i+1]
            scope_stack.append(msg_name)
            full_name = "_".join(scope_stack)
            messages.append({
                'full_name': full_name,
                'scope': list(scope_stack),
                'fields': []
            })
            i += 2
            if tokens[i] == '{': i += 1
        elif t == '}':
            if scope_stack: scope_stack.pop()
            i += 1
        elif t == 'repeated':
            f_type = tokens[i+1]
            f_name = tokens[i+2]
            f_tag = tokens[i+4]
            messages[-1]['fields'].append({'rule': 'repeated', 'type': f_type, 'name': f_name, 'tag': f_tag})
            i += 6
        elif t == 'map':
            # map < k , v > name = tag ;
            k_type = tokens[i+2]
            v_type = tokens[i+4]
            f_name = tokens[i+6]
            f_tag = tokens[i+8]
            messages[-1]['fields'].append({'rule': 'map', 'type': 'map', 'name': f_name, 'tag': f_tag, 'k_type': k_type, 'v_type': v_type})
            i += 10
        else:
            # scalar or nested message: type name = tag ;
            if i+2 < len(tokens) and tokens[i+2] == '=':
                f_type = tokens[i]
                f_name = tokens[i+1]
                f_tag = tokens[i+3]
                messages[-1]['fields'].append({'rule': '', 'type': f_type, 'name': f_name, 'tag': f_tag})
                i += 5
            else:
                i += 1

    return messages

def resolve_type(type_name, scope, msg_names):
    if type_name in PROTO_TO_CPP: return PROTO_TO_CPP[type_name], False
    for i in range(len(scope), -1, -1):
        prefix = "_".join(scope[:i])
        candidate = f"{prefix}_{type_name}" if prefix else type_name
        if candidate in msg_names: return candidate, True
    raise Exception(f"Type {type_name} not found!")

def process_messages(messages):
    msg_names = {m['full_name'] for m in messages}

    # 1. Resolve types
    for m in messages:
        for f in m['fields']:
            if f['rule'] == 'map':
                f['k_cpp'], _ = resolve_type(f['k_type'], m['scope'], msg_names)
                f['v_cpp'], f['v_is_msg'] = resolve_type(f['v_type'], m['scope'], msg_names)
            else:
                f['cpp_type'], f['is_msg'] = resolve_type(f['type'], m['scope'], msg_names)

    # 2. Topological Sort (Ensure nested structs are declared before parents)
    deps = {m['full_name']: set() for m in messages}
    for m in messages:
        for f in m['fields']:
            if f['rule'] == 'map' and f['v_is_msg']: deps[m['full_name']].add(f['v_cpp'])
            elif f['rule'] != 'map' and f['is_msg']: deps[m['full_name']].add(f['cpp_type'])

    visited, sorted_msgs = set(), []
    def visit(name):
        if name in visited: return
        visited.add(name)
        for dep in deps[name]: visit(dep)
        sorted_msgs.append(name)

    for m in messages: visit(m['full_name'])

    msg_dict = {m['full_name']: m for m in messages}
    return [msg_dict[name] for name in sorted_msgs]

def generate_cpp(messages):
    out = [
        "#pragma once",
        "#include <string>",
        "#include <vector>",
        "#include <map>",
        "#include <protozero/pbf_writer.hpp>",
        "#include <protozero/pbf_reader.hpp>\n",
        "namespace lightproto {\n"
    ]

    # --- 1. 生成 C++ 结构体 ---
    for m in messages:
        out.append(f"struct {m['full_name']} {{")
        for f in m['fields']:
            if f['rule'] == 'repeated':
                out.append(f"    std::vector<{f['cpp_type']}> {f['name']};")
            elif f['rule'] == 'map':
                out.append(f"    std::map<{f['k_cpp']}, {f['v_cpp']}> {f['name']};")
            else:
                default_val = get_cpp_default(f['cpp_type'])
                init_str = f" = {default_val};" if default_val else ";"
                out.append(f"    {f['cpp_type']} {f['name']}{init_str}")

        # 生成 is_empty 函数 (用于零值剔除)
        out.append("\n    bool is_empty() const {")
        conds = []
        for f in m['fields']:
            if f['rule'] in ['repeated', 'map'] or f.get('cpp_type') == 'std::string': conds.append(f"{f['name']}.empty()")
            elif f.get('is_msg'): conds.append(f"{f['name']}.is_empty()")
            else: conds.append(f"{f['name']} == 0")
        if not conds: out.append("        return true;")
        else: out.append("        return " + " && ".join(conds) + ";")
        out.append("    }\n};")
        out.append("")

    # 声明所有 WriteMessage 和 ReadMessage 以避免互相依赖找不到
    for m in messages:
        out.append(f"inline void WriteMessage(const {m['full_name']}& src, protozero::pbf_writer& pbf);")
        out.append(f"inline void ReadMessage({m['full_name']}& dst, protozero::pbf_reader& pbf);")
    out.append("")

    # --- 2. 生成序列化逻辑 (Write) ---
    for m in messages:
        out.append(f"inline void WriteMessage(const {m['full_name']}& src, protozero::pbf_writer& pbf) {{")
        for f in m['fields']:
            if f['rule'] == 'repeated':
                if f['type'] in PBF_MAP and f['type'] != 'string':
                    out.append(f"    if (!src.{f['name']}.empty()) {{")
                    out.append(f"        pbf.add_packed_{PBF_MAP[f['type']]}({f['tag']}, std::begin(src.{f['name']}), std::end(src.{f['name']}));")
                    out.append(f"    }}")
                elif f['type'] == 'string':
                    out.append(f"    for (const auto& item : src.{f['name']}) pbf.add_string({f['tag']}, item);")
                else:
                    out.append(f"    for (const auto& item : src.{f['name']}) {{")
                    out.append(f"        protozero::pbf_writer pbf_sub(pbf, {f['tag']});")
                    out.append(f"        WriteMessage(item, pbf_sub);")
                    out.append(f"    }}")
            elif f['rule'] == 'map':
                out.append(f"    for (const auto& kv : src.{f['name']}) {{")
                out.append(f"        protozero::pbf_writer pbf_map(pbf, {f['tag']});")
                out.append(f"        pbf_map.add_{PBF_MAP[f['k_type']]}(1, kv.first);")
                if f['v_is_msg']:
                    out.append(f"        protozero::pbf_writer pbf_val(pbf_map, 2);")
                    out.append(f"        WriteMessage(kv.second, pbf_val);")
                else:
                    out.append(f"        pbf_map.add_{PBF_MAP[f['v_type']]}(2, kv.second);")
                out.append(f"    }}")
            elif f['is_msg']:
                out.append(f"    if (!src.{f['name']}.is_empty()) {{")
                out.append(f"        protozero::pbf_writer pbf_sub(pbf, {f['tag']});")
                out.append(f"        WriteMessage(src.{f['name']}, pbf_sub);")
                out.append(f"    }}")
            else:
                cond = f"!src.{f['name']}.empty()" if f['type'] == 'string' else f"src.{f['name']} != 0"
                out.append(f"    if ({cond}) pbf.add_{PBF_MAP[f['type']]}({f['tag']}, src.{f['name']});")
        out.append(f"}}\n")

    # --- 3. 生成反序列化逻辑 (Read) ---
    for m in messages:
        out.append(f"inline void ReadMessage({m['full_name']}& dst, protozero::pbf_reader& pbf) {{")
        out.append(f"    while (pbf.next()) {{")
        out.append(f"        switch (pbf.tag()) {{")
        for f in m['fields']:
            out.append(f"            case {f['tag']}: {{")
            if f['rule'] == 'repeated':
                if f['type'] in PBF_MAP and f['type'] != 'string':
                    out.append(f"                auto it = pbf.get_packed_{PBF_MAP[f['type']]}();")
                    out.append(f"                dst.{f['name']}.insert(dst.{f['name']}.end(), it.begin(), it.end());")
                elif f['type'] == 'string':
                    out.append(f"                dst.{f['name']}.push_back(pbf.get_string());")
                else:
                    out.append(f"                {f['cpp_type']} item;")
                    out.append(f"                protozero::pbf_reader pbf_item = pbf.get_message();")
                    out.append(f"                ReadMessage(item, pbf_item);")
                    out.append(f"                dst.{f['name']}.push_back(item);")
            elif f['rule'] == 'map':
                out.append(f"                protozero::pbf_reader pbf_map = pbf.get_message();")
                out.append(f"                {f['k_cpp']} key{ ' = 0' if f['k_cpp'] != 'std::string' else '' };")
                out.append(f"                {f['v_cpp']} val;")
                out.append(f"                while (pbf_map.next()) {{")
                out.append(f"                    if (pbf_map.tag() == 1) key = pbf_map.get_{PBF_MAP[f['k_type']]}();")
                if f['v_is_msg']:
                    out.append(f"                    else if (pbf_map.tag() == 2) {{")
                    out.append(f"                        protozero::pbf_reader pbf_val = pbf_map.get_message();")
                    out.append(f"                        ReadMessage(val, pbf_val);")
                    out.append(f"                    }}")
                else:
                    out.append(f"                    else if (pbf_map.tag() == 2) val = pbf_map.get_{PBF_MAP[f['v_type']]}();")
                out.append(f"                    else pbf_map.skip();")
                out.append(f"                }}")
                out.append(f"                dst.{f['name']}[key] = val;")
            elif f['is_msg']:
                out.append(f"                protozero::pbf_reader pbf_sub = pbf.get_message();")
                out.append(f"                ReadMessage(dst.{f['name']}, pbf_sub);")
            else:
                out.append(f"                dst.{f['name']} = pbf.get_{PBF_MAP[f['type']]}();")
            out.append(f"                break;")
            out.append(f"            }}")
        out.append(f"            default: pbf.skip(); break;")
        out.append(f"        }}")
        out.append(f"    }}")
        out.append(f"}}\n")

    # --- 4. 顶层快捷调用函数 ---
    out.append("    // --- 便捷 API ---")
    out.append("    template <typename T>")
    out.append("    inline std::string Serialize(const T& src) {")
    out.append("        std::string buffer;")
    out.append("        buffer.reserve(4096);")
    out.append("        protozero::pbf_writer pbf(buffer);")
    out.append("        WriteMessage(src, pbf);")
    out.append("        return buffer;")
    out.append("    }")
    out.append("")
    out.append("    template <typename T>")
    out.append("    inline T Deserialize(const std::string& data) {")
    out.append("        T dst;")
    out.append("        protozero::pbf_reader pbf(data);")
    out.append("        ReadMessage(dst, pbf);")
    out.append("        return dst;")
    out.append("    }")

    out.append("\n} // namespace lightproto")
    return "\n".join(out)

if __name__ == "__main__":
    file_name = sys.argv[1] if len(sys.argv) > 1 else 'hydromodel.txt'
    with open(file_name, 'r', encoding='utf-8') as f:
        content = f.read()

    msgs = process_messages(parse_proto(content))
    print(generate_cpp(msgs))
