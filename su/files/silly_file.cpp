//
// Created by dly on 2023/8/3.
//
#include "silly_file.h"

using namespace silly::file;

// TODO: 需要做些参照解决编码问题 https://blog.csdn.net/qq_36437446/article/details/105279221
// https://blog.csdn.net/hybluck/article/details/112256543
// 为什么用notepad++ 打开时 有些是中文编码(GBK23..)有些是ANSI
//



std::filesystem::path utils::realpath(const std::filesystem::path &fp)
{
#ifdef IS_WIN32
    // 如果路径已经是宽字符串形式，直接返回
    if (!fp.native().empty() && std::is_same_v<std::filesystem::path::value_type, wchar_t>)
    {
        return fp.lexically_normal();
    }

    // 处理字符串形式的路径
    std::string fullname = fp.string();
    if (!IS_GBK(fullname))
    {
        try
        {
            std::wstring widePath = UTF8_S2WS(fullname);
            return std::filesystem::path(widePath).lexically_normal();
        }
        catch (...)
        {
            // 转换失败，回退到原始路径
            return fp.lexically_normal();
        }
    }
#endif
    return fp.lexically_normal();
}
std::filesystem::path utils::realpath(const std::string &ftpstr)
{
#ifdef IS_WIN32
   
    if (!IS_GBK(ftpstr))
    {
        try
        {
            std::wstring widePath = UTF8_S2WS(ftpstr);
            return std::filesystem::path(widePath).lexically_normal();
        }
        catch (...)
        {
        }
    }
#endif
    return std::filesystem::path(ftpstr);
}

size_t utils::read(const std::filesystem::path &fp, std::string &content, const size_t &offset, const size_t &len)
{
    size_t ret_read_size = 0;
    content.clear();

    std::fstream input(realpath(fp), std::ios::binary | std::ios::in);

    if (!input.is_open())
    {
        return ret_read_size;
    }
    size_t file_size = utils::size(fp);
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

std::string utils::read(const std::filesystem::path &fp, const size_t &offset, const size_t &len)
{
    std::string ret;
    sufile::read(fp, ret, offset, len);
    return ret;
}

bool utils::readlines(const std::filesystem::path &fp, std::vector<std::string> &lines)
{
    std::fstream input(realpath(fp), std::ios::binary | std::ios::in);
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

std::vector<std::string> utils::readlines(const std::filesystem::path &fp)
{
    std::vector<std::string> ret;
    readlines(fp, ret);
    return ret;
}

size_t utils::write(const std::filesystem::path &fp, const std::string &content)
{
    size_t write_len = 0;
    std::fstream output(realpath(fp), std::ios::binary | std::ios::out);

    if (!output.is_open())
    {
        return write_len;
    }
    output.write(content.c_str(), content.size());
    return content.size();
}

size_t utils::write(const std::filesystem::path &fp, const std::vector<std::string> &lines)
{
    size_t write_len = 0;
    std::fstream output(realpath(fp), std::ios::binary | std::ios::out);

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

std::vector<std::filesystem::path> utils::list(const std::filesystem::path &fp, const std::string &u8filter)
{
    std::vector<std::filesystem::path> ret;
    std::filesystem::path _root = utils::realpath(fp);
    if (!std::filesystem::exists(_root))
    {
        return ret;
    }

    std::regex reg_filter(file_filter_regex(u8filter));
    bool match_all = (u8filter == SILLY_FILE_MATCH_ALL_WILDCHAR);

    if (std::filesystem::is_directory(_root))
    {
        try
        {
            for (const auto &itp : std::filesystem::directory_iterator(_root, std::filesystem::directory_options::skip_permission_denied))
            {
                if (match_all || std::regex_match(itp.path().filename().u8string(), reg_filter))
                {
                    ret.push_back(itp.path());
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

std::vector<std::filesystem::path> utils::relist(const std::filesystem::path &fp, const std::string &u8filter)
{
    std::vector<std::filesystem::path> ret;
    std::filesystem::path _root = realpath(fp);
    if (!std::filesystem::exists(_root))
    {
        return ret;
    }

    std::regex reg_filter(file_filter_regex(u8filter));
    bool match_all = (u8filter == SILLY_FILE_MATCH_ALL_WILDCHAR);
    if (std::filesystem::is_directory(_root))
    {
        try
        {
            for (const auto &itp : std::filesystem::recursive_directory_iterator(_root, std::filesystem::directory_options::skip_permission_denied))
            {
                if (match_all || std::regex_match(itp.path().filename().u8string(), reg_filter))
                {
                    ret.push_back(itp.path());
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

std::time_t utils::last_modify_sec(const std::filesystem::path &fp)
{
    std::time_t stamp = 0;
    try
    {
        // 检查文件是否存在
        std::filesystem::path rp = realpath(fp);
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
            std::cerr << "文件 " << fp.string() << " 不存在。" << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
    }

    return stamp;
}

std::time_t utils::last_modify_ms(const std::filesystem::path &fp)
{
    std::time_t stamp = 0;
    try
    {
        // 检查文件是否存在
        std::filesystem::path rp = realpath(fp);
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
            std::cerr << "文件 " << fp << " 不存在。" << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "文件系统错误: " << e.what() << std::endl;
    }

    return stamp;
}

bool utils::exist(const std::filesystem::path &path)
{
    try
    {
        return std::filesystem::exists(realpath(path));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return false;
}

bool utils::mkdir(const std::filesystem::path &path)
{
    try
    {
        return std::filesystem::create_directories(realpath(path));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}

void utils::rmfile(const std::filesystem::path &path)
{
    try
    {
        std::filesystem::remove(realpath(path));
    }
    catch (const std::exception &e)
    {
    }
}
void utils::rmdir(const std::filesystem::path &path)
{
    try
    {
        std::filesystem::remove_all(realpath(path));
    }
    catch (const std::exception &e)
    {
    }
}

size_t utils::size(const std::filesystem::path &path)
{
    size_t ret = 0;
    try
    {
        ret = std::filesystem::file_size(realpath(path));
    }
    catch (...)
    {
    }
    return ret;
}

void utils::copyfile(const std::filesystem::path &src, const std::filesystem::path &dst)
{
    try
    {
        std::filesystem::copy(realpath(src), realpath(dst));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void utils::copydir(const std::filesystem::path &src, const std::filesystem::path &dst)
{
    try
    {
        std::filesystem::copy(realpath(src), realpath(dst), std::filesystem::copy_options::recursive);
    }
    catch (const std::exception &e)
    {
    }
}

node::node(std::string path)
{
    this->path = path;
    this->is_dir = std::filesystem::is_directory(sufile::realpath(path));
}

node::node(std::filesystem::path path)
{
    this->path = path.u8string();
    this->is_dir = std::filesystem::is_directory(sufile::realpath(path));
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
    return std::filesystem::path(sufile::realpath(path)).filename().u8string();
}
std::string node::stem() const
{
    return std::filesystem::path(sufile::realpath(path)).stem().u8string();
}
std::string node::ext() const
{
    return std::filesystem::path(sufile::realpath(path)).extension().u8string();
}

void node::trace()
{
    is_dir = std::filesystem::is_directory(sufile::realpath(this->path));
    if (is_dir)
    {
        for (auto &p : std::filesystem::directory_iterator(sufile::realpath(this->path)))
        {
            children.emplace_back(std::make_unique<node>(p.path()));
            children.back()->trace();  // 递归构建子节点
        }
    }
}