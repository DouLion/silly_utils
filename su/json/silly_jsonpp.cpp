//
// Created by dell on 2023/12/26.
//

#include <json/silly_jsonpp.h>
#include <files/silly_file.h>
#include <log/silly_log.h>
using namespace silly;

Json::Value jsonpp::loadf(const std::string& file)
{
    std::fstream input;
    input.open(file, std::ios::binary | std::ios::in);
    if (!input.is_open())
    {
        return Json::nullValue;
    }
    Json::Reader reader;
    Json::Value root;

    if (reader.parse(input, root))
    {
        return root;
    }
    else
    {
        root = Json::nullValue;
    }
    input.close();
    return root;
}

Json::Value jsonpp::loads(const std::string& content)
{
    Json::Reader reader;
    Json::Value root;

    if (reader.parse(content, root))
    {
        return root;
    }
    return Json::nullValue;
}

std::string jsonpp::to_string(const Json::Value root, const jsonpp::opt& _opt)
{
    Json::StreamWriterBuilder stream_builder;
    if (_opt.utf8)
    {
        stream_builder["emitUTF8"] = true;
    }

    if (_opt.precision)
    {
        stream_builder["precision"] = _opt.precision;
        // decimal 仅小小数位保留    123456.789 保留 1位 是123456.7
        // significant 整个数字保留  123456.789 保留 7位 是123456.7
        stream_builder["precisionType"] = "decimal";  //
    }

    std::ostringstream stream;

    std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
    writer->write(root, &stream);
    return stream.str();
}

std::string jsonpp::stringify(const Json::Value root, const jsonpp::opt& _opt)
{
    return jsonpp::to_string(root, _opt);
}

std::string jsonpp::dumps(const Json::Value& root, const jsonpp::opt& _opt)
{
    return jsonpp::to_string(root, _opt);
}

bool jsonpp::dumpf(const std::string& file, const Json::Value& root, const jsonpp::opt& _opt)
{
    return silly_file::write(file, jsonpp::to_string(root, _opt)) > 0;
}

bool jsonpp::savef(const std::string& file, const Json::Value& root, const jsonpp::opt& _opt)
{
    return dumpf(file, jsonpp::to_string(root, _opt));
}

void find_object(const Json::Value jv_obj, const std::string& key, const std::string& filter, std::vector<std::string>& arr);
void find_array(const Json::Value jv_arr, const std::string& key, const std::string& filter, std::vector<std::string>& arr);

void find_array(const Json::Value jv_arr, const std::string& key, const std::string& filter, std::vector<std::string>& arr)
{
    for (auto jv_tmp : jv_arr)
    {
        if (jv_tmp.isArray())
        {
            find_array(jv_tmp, key, filter, arr);
        }
        else if (jv_tmp.isObject())
        {
            find_object(jv_tmp, key, filter, arr);
        }
    }
}

void find_object(const Json::Value jv_obj, const std::string& key, const std::string& filter, std::vector<std::string>& arr)
{
    for (auto name : jv_obj.getMemberNames())
    {
        if (name == key)
        {
            if (jv_obj[name].isString())
            {
                std::string content = jv_obj[name].asString();
                if (filter.empty())
                {
                    arr.push_back(content);
                }
                else
                {
                    if (content.find(filter) < content.size())
                    {
                        arr.push_back(content);
                    }
                }
            }
        }
        if (jv_obj[name].isArray())
        {
            find_array(jv_obj[name], key, filter, arr);
        }
        else if (jv_obj[name].isObject())
        {
            find_object(jv_obj[name], key, filter, arr);
        }
    }
}

void jsonpp::find_by_key(const std::string& json, const std::string& key, const std::string& filter, std::vector<std::string>& arr)
{
    Json::Value root = loads(json);
    find_by_key(root, key, filter, arr);
}

void jsonpp::find_by_key(const Json::Value& root, const std::string& key, const std::string& filter, std::vector<std::string>& arr)
{
    if (root.isNull())
    {
        return;
    }
    if (root.isArray())
    {
        find_array(root, key, filter, arr);
    }
    else if (root.isObject())
    {
        find_object(root, key, filter, arr);
    }
}

bool jsonpp::check_str(const Json::Value& root, const std::string& key, std::string& val)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }

    if (root.isMember(key))
    {
        if (root[key].isString())
        {
            val = root[key].asString();
            return true;
        }
        else
        {
            SLOG_DEBUG("字段 {} 不是string类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }
    return false;
}

bool jsonpp::check_double(const Json::Value& root, const std::string& key, double& val)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }

    if (root.isMember(key))
    {
        if (root[key].isDouble())
        {
            val = root[key].asDouble();
            return true;
        }
        else
        {
            SLOG_DEBUG("字段 {} 不是double类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }
    return false;
}
bool jsonpp::check_bool(const Json::Value& root, const std::string& key, bool& val)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }

    if (root.isMember(key))
    {
        if (root[key].isBool())
        {
            val = root[key].asBool();
            return true;
        }
        {
            SLOG_DEBUG("字段 {} 不是bool类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }
    return false;
}
bool jsonpp::check_arr(const Json::Value& root, const std::string& key, Json::Value& jv_arr)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }

    if (root.isMember(key))
    {
        if (root[key].isArray())
        {
            jv_arr = root[key];
            return true;
        }
        else
        {
            SLOG_DEBUG("字段 {} 不是数组类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }
    return false;
}
bool jsonpp::check_obj(const Json::Value& root, const std::string& key, Json::Value& jv_obj)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }

    if (root.isMember(key))
    {
        if (root[key].isObject())
        {
            jv_obj = root[key];
            return true;
        }
        else
        {
            SLOG_DEBUG("字段 {} 不是数组类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }
    return false;
}

bool jsonpp::check_int32(const Json::Value& root, const std::string& key, int32_t& val)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }
    if (root.isMember(key))
    {
        if (root[key].isInt())
        {
            val = root[key].asInt();
            return true;
        }
        else
        {
            SLOG_DEBUG("字段 {} 不是int类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }
    return false;
}
bool jsonpp::check_uint32(const Json::Value& root, const std::string& key, uint32_t& val)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }
    if (root.isMember(key))
    {
        if (root[key].isUInt())
        {
            val = root[key].asUInt();
            return true;
        }
        else
        {
            SLOG_DEBUG("字段 {} 不是uint类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }
    return false;
}
bool jsonpp::check_int64(const Json::Value& root, const std::string& key, int64_t& val)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }
    if (root.isMember(key))
    {
        if (root[key].isInt64())
        {
            val = root[key].asInt64();
            return true;
        }
        else
        {
            SLOG_DEBUG("字段 {} 不是int64类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }
    return false;
}
bool jsonpp::check_uint64(const Json::Value& root, const std::string& key, uint64_t& val)
{
    if (root.isNull())
    {
        SLOG_DEBUG("json 为空")
        return false;
    }
    if (root.isMember(key))
    {
        if (root[key].isUInt64())
        {
            val = root[key].asUInt64();
            return true;
        }
        else
        {
            SLOG_DEBUG("字段 {} 不是uint64类型", key)
        }
    }
    else
    {
        SLOG_DEBUG("不存在字段 {}", key)
    }

    return false;
}

void jsonpp::try_str(const Json::Value& root, const std::string& key, std::string& val)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
        ;
    }

    if (root.isMember(key))
    {
        if (root[key].isString())
        {
            val = root[key].asString();
        }
        else
        {
            throw std::runtime_error("字段 " + key + " 不是string类型");
            ;
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}

void jsonpp::try_double(const Json::Value& root, const std::string& key, double& val)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
    }

    if (root.isMember(key))
    {
        if (root[key].isDouble())
        {
            val = root[key].asDouble();
        }
        else
        {
            throw std::runtime_error("字段 " + key + " 不是double类型");
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}
void jsonpp::try_bool(const Json::Value& root, const std::string& key, void& val)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
    }

    if (root.isMember(key))
    {
        if (root[key].isBool())
        {
            val = root[key].asBool();
        }
        {
            throw std::runtime_error("字段 " + key + " 不是bool类型");
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}
void jsonpp::try_arr(const Json::Value& root, const std::string& key, Json::Value& jv_arr)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
    }

    if (root.isMember(key))
    {
        if (root[key].isArray())
        {
            jv_arr = root[key];
        }
        else
        {
            throw std::runtime_error("字段 " + key + " 不是数组类型");
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}
void jsonpp::try_obj(const Json::Value& root, const std::string& key, Json::Value& jv_obj)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
    }

    if (root.isMember(key))
    {
        if (root[key].isObject())
        {
            jv_obj = root[key];
        }
        else
        {
            throw std::runtime_error("字段 " + key + " 不是数组类型");
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}

void jsonpp::try_int32(const Json::Value& root, const std::string& key, int32_t& val)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
    }
    if (root.isMember(key))
    {
        if (root[key].isInt())
        {
            val = root[key].asInt();
        }
        else
        {
            throw std::runtime_error("字段 " + key + " 不是int类型");
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}
void jsonpp::try_uint32(const Json::Value& root, const std::string& key, uint32_t& val)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
    }
    if (root.isMember(key))
    {
        if (root[key].isUInt())
        {
            val = root[key].asUInt();
        }
        else
        {
            throw std::runtime_error("字段 " + key + " 不是uint类型");
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}
void jsonpp::try_int64(const Json::Value& root, const std::string& key, int64_t& val)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
    }
    if (root.isMember(key))
    {
        if (root[key].isInt64())
        {
            val = root[key].asInt64();
        }
        else
        {
            throw std::runtime_error("字段 " + key + " 不是int64类型");
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}
void jsonpp::try_uint64(const Json::Value& root, const std::string& key, uint64_t& val)
{
    if (root.isNull())
    {
        throw std::runtime_error("json 为空");
    }
    if (root.isMember(key))
    {
        if (root[key].isUInt64())
        {
            val = root[key].asUInt64();
        }
        else
        {
            throw std::runtime_error("字段 " + key + " 不是uint64类型");
        }
    }
    else
    {
        throw std::runtime_error("不存在字段 " + key);
    }
}