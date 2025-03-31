//
// Created by dly on 2023/8/3.
//

#include "silly_file.h"
#include <fstream>
#include <regex>
#include "encode/silly_encode.h"

namespace sfs = std::filesystem;

// TODO: 需要做些参照解决编码问题 https://blog.csdn.net/qq_36437446/article/details/105279221
// https://blog.csdn.net/hybluck/article/details/112256543
// 为什么用notepad++ 打开时 有些是中文编码(GBK23..)有些是ANSI
//

static std::string _wildcard2regex(const std::string& pattern) {
    std::string regexPattern;
    for (char c : pattern) {
        switch (c) {
            case '*': regexPattern += ".*"; break;  // * 匹配任意数量的字符
            case '?': regexPattern += "."; break;   // ? 匹配单个字符
            case '.': regexPattern += "\\."; break; // . 需要转义
            case '\\': regexPattern += "\\\\"; break; // \ 需要转义
            default: regexPattern += c; break;
        }
    }
    return "^" + regexPattern + "$";  // 确保整个字符串匹配
}

static bool validate_utf8(const std::string& str) {
    int n;
    for (size_t i = 0; i < str.length();) {
        if ((str[i] & 0x80) == 0x00) { // 单字节
            n = 1;
        } else if ((str[i] & 0xE0) == 0xC0) { // 两字节
            n = 2;
        } else if ((str[i] & 0xF0) == 0xE0) { // 三字节
            n = 3;
        } else if ((str[i] & 0xF8) == 0xF0) { // 四字节
            n = 4;
        } else {
            return false;
        }
        for (int j = 1; j < n; ++j)
            if ((str[i + j] & 0xC0) != 0x80)
                return false;
        i += n;
    }
    return true;
}


size_t silly::file::utils::read(const std::string &u8path, std::string &content, const size_t &offset, const size_t &len)
{
    size_t ret_read_size = 0;
    content.clear();
    std::fstream input;
#if IS_WIN32
    if (silly_encode::is_utf8(u8path))
    {
        input.open(silly_encode::cxx11_string_wstring(u8path), std::ios::binary | std::ios::in);
    }
    else
    {
        input.open(u8path, std::ios::binary | std::ios::in);
    }
#else
    input.open(u8path, std::ios::binary | std::ios::in);
#endif

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

size_t silly::file::utils::read(const std::string &u8path, unsigned char **content, const size_t &offset, const size_t &len)
{
    size_t read_size = 0;
    if ((*content))  // content 不能有内容
    {
        return read_size;
    }
    std::string s_cont;
    read_size = silly::file::utils::read(u8path, s_cont, offset, len);
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

bool silly::file::utils::read(const std::string &u8path, std::vector<std::string> &lines)
{
    std::fstream input;
#if IS_WIN32
    input.open(silly_encode::cxx11_string_wstring(u8path), std::ios::binary | std::ios::in);
#else
    input.open(u8path, std::ios::binary | std::ios::in);
#endif
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

size_t silly::file::utils::write(const std::string &u8path, const std::string &content)
{
    size_t write_len = 0;
    std::fstream ofs_w;

#if IS_WIN32
    if (silly_encode::is_utf8(u8path))
    {
        ofs_w.open(silly_encode::cxx11_string_wstring(u8path), std::ios::binary | std::ios::out);
    }
    else
    {
        ofs_w.open(u8path, std::ios::binary | std::ios::out);
    }
#else
    ofs_w.open(u8path, std::ios::binary | std::ios::in);
#endif

    if (!ofs_w.is_open())
    {
        return write_len;
    }
    ofs_w.write(content.c_str(), content.size());
    return content.size();
}

size_t silly::file::utils::write(const std::string &u8path, const std::vector<std::string> &lines)
{
    size_t write_len = 0;
#if IS_WIN32
    std::ofstream ofs_w(silly_encode::cxx11_string_wstring(u8path));
#else
    std::ofstream ofs_w(u8path);
#endif
    if (!ofs_w.is_open())
    {
        return write_len;
    }
    for (auto l : lines)
    {
        ofs_w.write(l.c_str(), l.size());
        write_len += l.size();
    }
    return write_len;
}

std::vector<std::string> silly::file::utils::list(const std::string &u8path, const std::string &filter)
{
    std::vector<std::string> vs_ret_list;
    sfs::path sfp_root(silly_encode::cxx11_string_wstring(u8path));
    if (!sfs::exists(sfp_root))
    {
        return vs_ret_list;
    }

    std::regex reg_filter(file_filter_regex(filter));
    bool b_match_all = (filter == SILLY_FILE_MATCH_ALL_WILDCHAR);

    if (sfs::is_directory(sfp_root))
    {
        try
        {
            for (auto iter_f : sfs::directory_iterator(sfp_root, sfs::directory_options::skip_permission_denied))
            {
                if (b_match_all || std::regex_match(iter_f.path().filename().u8string(), reg_filter))
                {
                    vs_ret_list.push_back(iter_f.path().u8string());
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
        if (b_match_all || std::regex_match(sfp_root.filename().u8string(), reg_filter))
        {
            vs_ret_list.push_back(sfp_root.u8string());
        }
    }
    return vs_ret_list;
}

std::vector<std::string> silly::file::utils::relist(const std::string &u8path, const std::string &filter)
{
    std::vector<std::string> vs_ret_list;
    sfs::path sfp_root(u8path);
    if (!sfs::exists(sfp_root))
    {
        return vs_ret_list;
    }

    std::regex reg_filter(file_filter_regex(filter));
    bool b_match_all = (filter == SILLY_FILE_MATCH_ALL_WILDCHAR);
    if (sfs::is_directory(sfp_root))
    {
        try
        {
            for (auto iter_f : sfs::recursive_directory_iterator(sfp_root, sfs::directory_options::skip_permission_denied))
            {
                if (b_match_all || std::regex_match(iter_f.path().filename().u8string(), reg_filter))
                {
                    vs_ret_list.push_back(iter_f.path().u8string());
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
        if (b_match_all || std::regex_match(sfp_root.filename().u8string(), reg_filter))
        {
            vs_ret_list.push_back(sfp_root.u8string());
        }
    }
    return vs_ret_list;
}

std::string silly::file::utils::file_filter_regex(const std::string &filter)
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
size_t silly::file::utils::last_modify_sec(const std::string &u8path)
{
    size_t stamp = 0;
    try
    {
        // 检查文件是否存在
        if (std::filesystem::exists(u8path))
        {
            // 获取文件的最后修改时间
            auto ftime = std::filesystem::last_write_time(u8path);

            // 转换为系统时间点
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

            /* // 转换为time_t类型
             std::time_t cftime = ;*/
            stamp = static_cast<size_t>(std::chrono::system_clock::to_time_t(sctp));
        }
        else
        {
            std::cerr << "文件 " << u8path << " 不存在。" << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
    }

    return stamp;
}
size_t silly::file::utils::last_modify_ms(const std::string &u8path)
{
    size_t stamp = 0;
    try
    {
        // 检查文件是否存在
        if (std::filesystem::exists(u8path))
        {
            // 获取文件的最后修改时间
            auto ftime = std::filesystem::last_write_time(u8path);

            // 转换为系统时间点
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

            // 获取自纪元开始的时间点
            auto duration = sctp.time_since_epoch();

            // 转换为毫秒
            stamp = static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
        }
        else
        {
            std::cerr << "文件 " << u8path << " 不存在。" << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
    }

    return stamp;
}
size_t silly::file::utils::size(const std::string &u8path)
{
    size_t file_size = 0;
    std::fstream input;
#if IS_WIN32
    input.open(silly_encode::cxx11_string_wstring(u8path), std::ios::binary | std::ios::in);
#else
    input.open(u8path, std::ios::binary | std::ios::in);
#endif
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
bool silly::file::utils::exist(const std::string &u8path)
{
    return exist(std::filesystem::path(u8path));
}
bool silly::file::utils::exist(const std::filesystem::path &u8path)
{
    try
    {
        return std::filesystem::exists(u8path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}
bool silly::file::utils::mkdir(const std::string &u8path)
{
    return mkdir(std::filesystem::path(u8path));
}
bool silly::file::utils::mkdir(const std::filesystem::path &u8path)
{
    try
    {
        return std::filesystem::create_directories(u8path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}
void silly::file::utils::rm(const std::string &u8path)
{
    rm(std::filesystem::path(u8path));
}
void silly::file::utils::rm(const std::filesystem::path &u8path)
{
    try
    {
        std::filesystem::remove(u8path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
void silly::file::utils::rrm(const std::string &u8path)
{
    rrm(std::filesystem::path(u8path));
}
void silly::file::utils::rrm(const std::filesystem::path &u8path)
{
    try
    {
        std::filesystem::remove_all(u8path);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
silly::file::node::node(std::string path)
{
    this->path = path;
    this->is_dir = std::filesystem::is_directory(path);
}
silly::file::node::node(std::filesystem::path path)
{
    this->path = path.u8string();
    this->is_dir = std::filesystem::is_directory(path);
}
silly::file::node::node(const silly::file::node &other)
{
    this->path = other.path;
    this->is_dir = other.is_dir;
}
silly::file::node &silly::file::node::operator=(const silly::file::node &other)
{
    this->path = other.path;
    this->is_dir = other.is_dir;
    return *this;
}
std::string silly::file::node::name() const
{
    return std::filesystem::path(this->path).filename().u8string();
}
std::string silly::file::node::stem() const
{
    return std::filesystem::path(this->path).stem().u8string();
}
std::string silly::file::node::ext() const
{
    return std::filesystem::path(this->path).extension().u8string();
}

void silly::file::node::trace()
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