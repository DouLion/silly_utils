//
// Created by dly on 2023/8/3.
//
#include "silly_file.h"


using namespace silly::file;

// TODO: 需要做些参照解决编码问题 https://blog.csdn.net/qq_36437446/article/details/105279221
// https://blog.csdn.net/hybluck/article/details/112256543
// 为什么用notepad++ 打开时 有些是中文编码(GBK23..)有些是ANSI
//

static std::wstring _cxx11_string_wstring(const std::string &str)
{
    // 需要c++11 或者c++17
    std::wstring ws_result;
    try
    {
        using convert_typeX = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_typeX, wchar_t> converterX;

        ws_result = converterX.from_bytes(str);
    }
    catch (std::exception &e)
    {
        SU_ERROR_PRINT("%s", e.what());
        ws_result.clear();
    }
    return ws_result;
}
static std::string _wildcard2regex(const std::string &pattern)
{
    std::string regexPattern;
    for (char c : pattern)
    {
        switch (c)
        {
            case '*':
                regexPattern += ".*";
                break;  // * 匹配任意数量的字符
            case '?':
                regexPattern += ".";
                break;  // ? 匹配单个字符
            case '.':
                regexPattern += "\\.";
                break;  // . 需要转义
            case '\\':
                regexPattern += "\\\\";
                break;  // \ 需要转义
            default:
                regexPattern += c;
                break;
        }
    }
    return "^" + regexPattern + "$";  // 确保整个字符串匹配
}

static bool _is_utf8(const std::string &str)
{
    int n;
    for (size_t i = 0; i < str.length();)
    {
        if ((str[i] & 0x80) == 0x00)
        {  // 单字节
            n = 1;
        }
        else if ((str[i] & 0xE0) == 0xC0)
        {  // 两字节
            n = 2;
        }
        else if ((str[i] & 0xF0) == 0xE0)
        {  // 三字节
            n = 3;
        }
        else if ((str[i] & 0xF8) == 0xF0)
        {  // 四字节
            n = 4;
        }
        else
        {
            return false;
        }
        for (int j = 1; j < n; ++j)
            if ((str[i + j] & 0xC0) != 0x80)
                return false;
        i += n;
    }
    return true;
}
std::filesystem::path utils::realpath(const std::string &path)
{
    std::filesystem::path realpath(path);
#if IS_WIN32
    if (_is_utf8(path))
    {
        realpath = _cxx11_string_wstring(path);
    }
#endif
    return realpath;
}
size_t utils::read(const std::string &path, std::string &content, const size_t &offset, const size_t &len)
{
    size_t ret_read_size = 0;
    content.clear();

    std::fstream input(realpath(path), std::ios::binary | std::ios::in);

    if (!input.is_open())
    {
        return ret_read_size;
    }
    input.ignore(std::numeric_limits<std::streamsize>::max());
    size_t file_size = input.gcount();
    input.clear();  //  ignore函数会移动到文件尾部, 需要移动回文件头
    input.seekg(0, std::ios_base::beg);
    if (offset >= file_size)  // 保证读值不为空
    {
        input.close();
        return ret_read_size;
    }
    input.seekg(0, std::ios::beg);
    ret_read_size = SU_MIN(len, file_size - offset);
    content.resize(ret_read_size);

    input.seekg(offset, std::ios::beg);
    input.read(&content[0], ret_read_size);
    input.close();
    return ret_read_size;
}

size_t utils::read(const std::string &path, unsigned char **content, const size_t &offset, const size_t &len)
{
    size_t read_size = 0;
    if ((*content))  // content 不能有内容
    {
        return read_size;
    }
    std::string s_cont;
    read_size = utils::read(path, s_cont, offset, len);
    if (read_size)
    {
        *content = (unsigned char *)malloc(read_size);
        if ((*content))
        {
            memcpy((*content), &s_cont[0], read_size);
        }
        else
        {
            read_size = 0;
        }
    }
    return read_size;
}

bool utils::read(const std::string &path, std::vector<std::string> &lines)
{
    std::fstream input(realpath(path), std::ios::binary | std::ios::in);
    if (input.is_open())
    {
        std::string line;
        while (std::getline(input, line))
        {
            lines.push_back(line);
        }
    }
    else
    {
        return false;
    }
    input.close();
    return true;
}

size_t utils::write(const std::string &path, const std::string &content)
{
    size_t write_len = 0;
    std::fstream output(realpath(path), std::ios::binary | std::ios::out);

    if (!output.is_open())
    {
        return write_len;
    }
    output.write(content.c_str(), content.size());
    return content.size();
}

size_t utils::write(const std::string &path, const std::vector<std::string> &lines)
{
    size_t write_len = 0;
    std::fstream output(realpath(path), std::ios::binary | std::ios::out);

    if (!output.is_open())
    {
        return write_len;
    }
    for (auto l : lines)
    {
        output.write(l.c_str(), l.size());
        write_len += l.size();
    }
    return write_len;
}

std::vector<std::string> utils::list(const std::string &path, const std::string &filter)
{
    std::vector<std::string> ret;
    std::filesystem::path _root(_cxx11_string_wstring(path));
    if (!std::filesystem::exists(_root))
    {
        return ret;
    }

    std::regex reg_filter(file_filter_regex(filter));
    bool match_all = (filter == SILLY_FILE_MATCH_ALL_WILDCHAR);

    if (std::filesystem::is_directory(_root))
    {
        try
        {
            for (auto itp : std::filesystem::directory_iterator(_root, std::filesystem::directory_options::skip_permission_denied))
            {
                if (match_all || std::regex_match(itp.path().filename().u8string(), reg_filter))
                {
                    ret.push_back(itp.path().u8string());
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    else
    {
        if (match_all || std::regex_match(_root.filename().u8string(), reg_filter))
        {
            ret.push_back(_root.u8string());
        }
    }
    return ret;
}

std::vector<std::string> utils::relist(const std::string &path, const std::string &filter)
{
    std::vector<std::string> ret;
    std::filesystem::path _root = realpath(path);
    if (!std::filesystem::exists(_root))
    {
        return ret;
    }

    std::regex reg_filter(file_filter_regex(filter));
    bool match_all = (filter == SILLY_FILE_MATCH_ALL_WILDCHAR);
    if (std::filesystem::is_directory(_root))
    {
        try
        {
            for (const auto& itp : std::filesystem::recursive_directory_iterator(_root, std::filesystem::directory_options::skip_permission_denied))
            {
                if (match_all || std::regex_match(itp.path().filename().u8string(), reg_filter))
                {
                    ret.push_back(itp.path().u8string());
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    else
    {
        if (match_all || std::regex_match(_root.filename().u8string(), reg_filter))
        {
            ret.push_back(_root.u8string());
        }
    }
    return ret;
}

std::string utils::file_filter_regex(const std::string &filter)
{
    std::string s_result;
    for (const auto c : filter)
    {
        if (c == '.')
        {
            s_result.append("\\.");
        }
        else if (c == '*')
        {
            s_result.append(".*");
        }
        else
        {
            s_result.push_back(c);
        }
    }
    return s_result;
}
size_t utils::last_modify_sec(const std::string &path)
{
    size_t stamp = 0;
    try
    {
        // 检查文件是否存在
        std::filesystem::path rp = realpath(path);
        if (std::filesystem::exists(rp))
        {
            // 获取文件的最后修改时间
            auto ftime = std::filesystem::last_write_time(rp);

            // 转换为系统时间点
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

            /* // 转换为time_t类型
             std::time_t cftime = ;*/
            stamp = static_cast<size_t>(std::chrono::system_clock::to_time_t(sctp));
        }
        else
        {
            std::cerr << "文件 " << path << " 不存在。" << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
    }

    return stamp;
}
size_t utils::last_modify_ms(const std::string &path)
{
    size_t stamp = 0;
    try
    {
        // 检查文件是否存在
        std::filesystem::path rp = realpath(path);
        if (std::filesystem::exists(rp))
        {
            // 获取文件的最后修改时间
            auto ftime = std::filesystem::last_write_time(rp);

            // 转换为系统时间点
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

            // 获取自纪元开始的时间点
            auto duration = sctp.time_since_epoch();

            // 转换为毫秒
            stamp = static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
        }
        else
        {
            std::cerr << "文件 " << path << " 不存在。" << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
    }

    return stamp;
}
size_t utils::size(const std::string &path)
{
    size_t file_size = 0;
    std::fstream input(realpath(path), std::ios::binary | std::ios::in);

    if (!input.is_open())
    {
        return file_size;
    }
    // 移动到文件末尾
    input.seekg(0, std::ios::end);

    // 获取文件大小
    file_size = input.tellg();
    return file_size;
}
bool utils::exist(const std::string &path)
{
    return exist(std::filesystem::path(path));
}
bool utils::exist(const std::filesystem::path &path)
{
    try
    {
        return std::filesystem::exists(path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}
bool utils::mkdir(const std::string &path)
{
    return mkdir(std::filesystem::path(path));
}
bool utils::mkdir(const std::filesystem::path &path)
{
    try
    {
        return std::filesystem::create_directories(path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}
void utils::rm(const std::string &path)
{
    rm(std::filesystem::path(path));
}
void utils::rm(const std::filesystem::path &path)
{
    try
    {
        std::filesystem::remove(path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
void utils::rrm(const std::string &path)
{
    rrm(std::filesystem::path(path));
}
void utils::rrm(const std::filesystem::path &path)
{
    try
    {
        std::filesystem::remove_all(path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
size_t utils::read(const std::filesystem::path &path, std::string &content, const size_t &offset, const size_t &len)
{
    return read(path.string(), content, offset, len);
}
size_t utils::read(const std::filesystem::path &path, unsigned char **content, const size_t &offset, const size_t &len)
{
    return read(path.string(), content, offset, len)    ;
}
bool utils::read(const std::filesystem::path &path, std::vector<std::string> &lines)
{
    return read(path.string(), lines);
}
size_t utils::size(const std::filesystem::path &path)
{
    return size(path.string());
}
size_t utils::write(const std::filesystem::path &path, const std::string &content)
{
    return write(path.string(), content);
}
size_t utils::write(const std::filesystem::path &path, const std::vector<std::string> &lines)
{
    return write(path.string(), lines);
}
std::vector<std::string> utils::list(const std::filesystem::path &path, const std::string &filter)
{
    return list(path.string(), filter);
}
std::vector<std::string> utils::relist(const std::filesystem::path &path, const std::string &filter)
{
    return relist(path.string(), filter);
}
size_t utils::last_modify_sec(const std::filesystem::path &path)
{
    return last_modify_sec(path.string());
}
size_t utils::last_modify_ms(const std::filesystem::path &path)
{
    return last_modify_ms(path.string());
}
void utils::copy(const std::string &src, const std::string &dst)
{
    copy(std::filesystem::path(src), std::filesystem::path(dst));
}

}
void utils::copy(const std::filesystem::path &src, const std::filesystem::path &dst)
{
    try{
        std::filesystem::copy(src, dst);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
void utils::rcopy(const std::string &src, const std::string &dst)
{
    rcopy(std::filesystem::path(src), std::filesystem::path(dst));
}
void utils::rcopy(const std::filesystem::path &src, const std::filesystem::path &dst)
{
    try
    {
        std::filesystem::copy(src, dst, std::filesystem::copy_options::recursive);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
node::node(std::string path)
{
    this->path = path;
    this->is_dir = std::filesystem::is_directory(path);
}
node::node(std::filesystem::path path)
{
    this->path = path.u8string();
    this->is_dir = std::filesystem::is_directory(path);
}
node::node(const node &other)
{
    this->path = other.path;
    this->is_dir = other.is_dir;
}
node &node::operator=(const node &other)
{
    this->path = other.path;
    this->is_dir = other.is_dir;
    return *this;
}
std::string node::name() const
{
    return std::filesystem::path(this->path).filename().u8string();
}
std::string node::stem() const
{
    return std::filesystem::path(this->path).stem().u8string();
}
std::string node::ext() const
{
    return std::filesystem::path(this->path).extension().u8string();
}

void node::trace()
{
    is_dir = std::filesystem::is_directory(this->path);
    if (is_dir)
    {
        for (auto &p : std::filesystem::directory_iterator(this->path))
        {
            children.emplace_back(std::make_unique<node>(p.path()));
            children.back()->trace();  // 递归构建子节点
        }
    }
}