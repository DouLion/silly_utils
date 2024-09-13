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

size_t silly_file::read(const std::string &path, std::string &content, const size_t &offset, const size_t &len)
{
    size_t ret_read_size = 0;
    content.clear();
    std::fstream input;
#if IS_WIN32
    input.open(silly_encode::cxx11_string_wstring(path), std::ios::binary | std::ios::in);
#else
    input.open(path, std::ios::binary | std::ios::in);
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
    ret_read_size = SU_MIN(len, file_size) - offset;
    content.resize(ret_read_size);

    input.seekg(offset, std::ios::beg);
    input.read(&content[0], ret_read_size);
    input.close();
    return ret_read_size;
}

size_t silly_file::read(const std::string &path, unsigned char **content, const size_t &offset, const size_t &len)
{
    size_t read_size = 0;
    if ((*content))  // content 不能有内容
    {
        return read_size;
    }
    std::string s_cont;
    read_size = silly_file::read(path, s_cont, offset, len);
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

bool silly_file::read(const std::string &path, std::vector<std::string> &lines)
{
    std::fstream input;
#if IS_WIN32
    input.open(silly_encode::cxx11_string_wstring(path), std::ios::binary | std::ios::in);
#else
    input.open(path, std::ios::binary | std::ios::in);
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

size_t silly_file::write(const std::string &path, const std::string &content)
{
    size_t write_len = 0;
#if IS_WIN32
    std::ofstream ofs_w(silly_encode::cxx11_string_wstring(path), std::ios::out | std::ios::binary);
#else
    std::ofstream ofs_w(path);
#endif

    if (!ofs_w.is_open())
    {
        return write_len;
    }
    ofs_w.write(content.c_str(), content.size());
    return content.size();
}

size_t silly_file::write(const std::string &path, const std::vector<std::string> &lines)
{
    size_t write_len = 0;
#if IS_WIN32
    std::ofstream ofs_w(silly_encode::cxx11_string_wstring(path));
#else
    std::ofstream ofs_w(path);
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

std::vector<std::string> silly_file::list_all(const std::string &path, const std::string &filter)
{
    std::vector<std::string> vs_ret_list;
    sfs::path sfp_root(silly_encode::cxx11_string_wstring(path));
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

std::vector<std::string> silly_file::list_all_recurse(const std::string &path, const std::string &filter)
{
    std::vector<std::string> vs_ret_list;
    sfs::path sfp_root(path);
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

std::string silly_file::file_filter_regex(const std::string &filter)
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
size_t silly_file::last_modify_stamp_sec(const std::string &path)
{
    size_t stamp = 0;
    try
    {
        // 检查文件是否存在
        if (std::filesystem::exists(path))
        {
            // 获取文件的最后修改时间
            auto ftime = std::filesystem::last_write_time(path);

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
size_t silly_file::last_modify_stamp_ms(const std::string &path)
{
    size_t stamp = 0;
    try
    {
        // 检查文件是否存在
        if (std::filesystem::exists(path))
        {
            // 获取文件的最后修改时间
            auto ftime = std::filesystem::last_write_time(path);

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
