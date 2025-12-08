/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-04
 * @file: silly_path.c
 * @description: silly_path实现
 * @version: v1.0.1 2025-12-04 dou li yang
 */
#include "silly_path.h"
#include <system/silly_system.h>
suPath::suPath(const std::string& path)
{
#if _WIN32
    if (HAS_UTF8(path))
    {
        m_path = std::filesystem::path(UTF8ToGBK(path));
    }
    else
#endif
    {
        m_path = std::filesystem::path(path);
    }
}
suPath::suPath(const char* path)
{
    const std::string s(path, strlen(path));
#if _WIN32
    if (HAS_UTF8(s))
    {
        m_path = std::filesystem::path(UTF8ToGBK(s));
    }
    else
#endif
    {
        m_path = std::filesystem::path(s);
    }
}
suPath::suPath(const suPath& path)
{
    m_path = path.m_path;
}
suPath::suPath(const std::filesystem::path& path)
{
    m_path = path;
}
#if _WIN32
suPath::suPath(const std::wstring& path)
{
    m_path = std::filesystem::path(path);
}
suPath::suPath(const wchar_t* path)
{
    m_path = std::filesystem::path(path);
}
suPath& suPath::append(const std::wstring& node)
{
    m_path.append(node);
    return *this;
}
std::wstring suPath::name_ws() const
{
    return m_path.filename().wstring();
}
std::wstring suPath::stem_ws() const
{
    return m_path.stem().wstring();
}
std::wstring suPath::extension_ws() const
{
    return m_path.extension().wstring();
}
std::wstring suPath::wstring() const
{
    return m_path.wstring();
}
#endif
suPath suPath::absolute() const
{
    return std::filesystem::absolute(m_path);
}
suPath suPath::absolute(const suPath& path)
{
    return std::filesystem::absolute(path.m_path);
}
suPath suPath::relative(const suPath& root) const
{
    return std::filesystem::relative(m_path, root.m_path);
}
suPath suPath::relative(const suPath& src, const suPath& root)
{
    return {std::filesystem::relative(src.path(), root.m_path)};
}
suPath suPath::parent() const
{
    return {m_path.parent_path()};
}
suPath suPath::parent(const suPath& path)
{
    return path.parent();
}
suPath suPath::root() const
{
    return {m_path.root_path()};
}
suPath suPath::root(const suPath& path)
{
    return path.root();
}
bool suPath::is_absolute() const
{
    return m_path.is_absolute();
}
bool suPath::is_absolute(const suPath& path)
{
    return path.is_absolute();
}
bool suPath::is_relative() const
{
    return !m_path.is_relative();
}
bool suPath::is_relative(const suPath& path)
{
    return !path.is_relative();
}
bool suPath::exists() const
{
    return std::filesystem::exists(m_path);
}
bool suPath::is_dir(const suPath& path)
{
    return std::filesystem::is_directory(path.m_path);
}
bool suPath::is_file(const suPath& path)
{
    return std::filesystem::is_regular_file(path.m_path);
}
bool suPath::is_link(const suPath& path)
{
    return std::filesystem::is_symlink(path.m_path);
}
bool suPath::is_dir() const
{
    if (exists())
    {
        return std::filesystem::is_directory(m_path);
    }
    return false;
}
bool suPath::is_file() const
{
    if (exists())
    {
        return std::filesystem::is_regular_file(m_path);
    }
    return false;
}
bool suPath::is_link() const
{
    if (exists())
    {
        return std::filesystem::is_symlink(m_path);
    }
    return false;
}
suPath suPath::cwd()
{
    suPath ret;
    ret.m_path = std::filesystem::current_path();
    return ret;
}
bool suPath::chdir(const suPath& path)
{
    try
    {
        std::filesystem::current_path(path.m_path);
    }
    catch (std::exception& e)
    {
        path.m_err = e.what();
        return false;
    }
    return true;
}
bool suPath::mkdir() const
{
    if (is_file())
    {
        return mkdir(parent());
    }
    else if (is_dir())
    {
        return mkdir(*this);
    }
    return false;
}
bool suPath::mkdir(const suPath& path)
{
    if (!path.exists())
    {
        std::filesystem::create_directories(path.m_path);
    }
    return path.is_dir();
}
bool suPath::rmfile(const suPath& fp)
{
    if (fp.is_file())
    {
        std::filesystem::remove(fp.m_path);
    }
    else
    {
        fp.m_err = fp.u8string() + " 不是普通文件";
        return false;
    }
    return true;
}
bool suPath::rmdir(const suPath& fp, const bool& r)
{
    if (fp.is_dir())
    {
        if (r)
        {
            std::filesystem::remove_all(fp.m_path);
        }
        else
        {
            std::filesystem::remove(fp.m_path);
        }
    }
    else
    {
        fp.m_err = fp.u8string() + " 不是普通目录";
        return false;
    }
    return true;
}
bool suPath::mklnk(const suPath& lnk) const
{
    try
    {
        if (lnk.exists())
        {
            m_err = lnk.u8string() + " 链接已存在";
            return false;
        }
        if (is_dir())
        {
            mkdir(lnk.parent());
            std::filesystem::create_directory_symlink(m_path, absolute(lnk.m_path).path());
        }
        else if (is_file())
        {
            std::filesystem::create_symlink(m_path, absolute(lnk.m_path).path());
        }
    }
    catch (std::exception& e)
    {
        m_err = e.what();
        return false;
    }
    
    return lnk.exists();
}

bool suPath::mklnk(const suPath& src, const suPath& lnk)
{
    return src.mklnk(lnk);
}
bool suPath::rmlnk(const suPath& fp)
{
    try
    {
        if (fp.is_link())
        {
            std::filesystem::remove(fp.m_path);
        }
        else
        {
            fp.m_err = "链接不存在";
            return false;
        }
    }
    catch (std::exception& e)
    {
        fp.m_err = e.what();
        return false;
    }
    return true;
    
}
bool suPath::copyfile(const suPath& dst, const bool& cover) const
{
    try
    {
        if (is_file())
        {
            if (dst.exists())
            {
                if (cover)
                {
                    std::filesystem::copy_file(m_path, dst.m_path, std::filesystem::copy_options::overwrite_existing);
                }
            }
            else
            {
                mkdir(dst.parent());
                std::filesystem::copy_file(m_path, dst.m_path, std::filesystem::copy_options::skip_existing);
            }
        }
        else
        {
            m_err = "复制源是文件夹, 使用copydir函数";
            return false;
        }
    }
    catch (std::exception& e)
    {
        m_err = e.what();
        return false;
    }
    return true;
}
bool suPath::copyfile(const suPath& src, const suPath& dst, const bool& cover)
{
    return src.copyfile(dst, cover);
    
}

bool suPath::copydir(const suPath& dst, const bool& cover) const
{
    try
    {
        if (is_dir())
        {
            if (dst.exists())
            {
                if (cover)
                {
                    std::filesystem::copy(m_path, dst.m_path, std::filesystem::copy_options::overwrite_existing);
                }
            }
            else
            {
                mkdir(dst.parent());
                std::filesystem::copy(m_path, dst.m_path, std::filesystem::copy_options::skip_existing);
            }
        }
        {
            m_err = "复制源是普通文件, 使用copyfile函数";
            return false;
        }
    }
    catch (std::exception& e)
    {
        m_err = e.what();
        return false;
    }
    return true;
}
bool suPath::copydir(const suPath& src, const suPath& dst, const bool& cover)
{
    return src.copydir(dst, cover);
}
std::vector<suPath> suPath::list(const std::string& u8filter) const
{
    std::vector<suPath> ret;
    if (!exists())
    {
        return ret;
    }

    if (is_dir())
    {
        std::regex reg_filter(WILDCARD2REGEX(u8filter));
        bool match_all = (u8filter == SU_MATCH_ALL_WILD_CHAR);
        try
        {
            for (const auto& itp : std::filesystem::directory_iterator(m_path, std::filesystem::directory_options::skip_permission_denied))
            {
                if (!match_all)
                {
                    if (!std::regex_match(itp.path().filename().u8string(), reg_filter))
                    {
                        continue;
                    }
                }
                ret.push_back(itp.path());
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    return ret;
}
std::vector<suPath> suPath::relist(const std::string& u8filter) const
{
    std::vector<suPath> ret;
    if (!exists())
    {
        return ret;
    }

    if (is_dir())
    {
        std::regex reg_filter(WILDCARD2REGEX(u8filter));
        bool match_all = (u8filter == SU_MATCH_ALL_WILD_CHAR);
        try
        {
            for (const auto& itp : std::filesystem::recursive_directory_iterator(m_path, std::filesystem::directory_options::skip_permission_denied))
            {
                if (!match_all)
                {
                    if (!std::regex_match(itp.path().filename().u8string(), reg_filter))
                    {
                        continue;
                    }
                }
                ret.push_back(itp.path());
            }
        }
        catch (std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    return ret;
}
std::vector<suPath> suPath::list(const suPath& fp, const std::string& u8filter)
{
    return fp.list(u8filter);
}
std::vector<suPath> suPath::relist(const suPath& fp, const std::string& u8filter)
{
    return fp.relist(u8filter);
}

bool suPath::exists(const suPath& path)
{
    return std::filesystem::exists(path.m_path);
}

suPath& suPath::append(const std::string& node)
{
#if _WIN32
    if (HAS_UTF8(node))
    {
        m_path.append(UTF8ToGBK(node));
    }
    else
#endif
    {
        m_path.append(node);
    }
    return *this;
}

std::string suPath::name() const
{
    return m_path.filename().string();
}
std::string suPath::name_utf8() const
{
    return m_path.filename().u8string();
}
std::string suPath::stem() const
{
    return m_path.stem().string();
}
std::string suPath::stem_utf8() const
{
    return m_path.stem().u8string();
}
std::string suPath::extension() const
{
    return m_path.extension().string();
}
std::string suPath::extension_utf8() const
{
    return m_path.extension().u8string();
}
size_t suPath::size() const
{
    try
    {
        if (is_file())
        {
            return std::filesystem::file_size(m_path);
        }
    }
    catch (std::exception& e)
    {
        m_err = e.what();
    }
    
    return 0;
}
size_t suPath::size(const suPath& fp)
{
    return fp.size();
}
std::time_t suPath::mstamp() const
{
    try
    {
        if (exists())
        {
            // 获取文件的最后修改时间
            auto ftime = std::filesystem::last_write_time(m_path);
            // 转换为系统时间点
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
            // 转换为time_t类型
            return std::chrono::system_clock::to_time_t(sctp);
        }
    }
    catch (std::exception& e)
    {
        m_err = e.what();
    }
    

    return 0;
}
std::time_t suPath::mstamp(const suPath& fp)
{
    return fp.mstamp();
}
std::time_t suPath::crstamp() const
{
    if (!exists())
    {
        return 0;
    }
    throw std::runtime_error("Not implemented");
    if (exists())
    {
        auto statu = std::filesystem::status(m_path);
    }
    return 0;
}
std::time_t suPath::crstamp(const suPath& fp)
{
    return fp.crstamp();
}
std::time_t suPath::astamp() const
{
    if (!exists())
    {
        return 0;
    }
    throw std::runtime_error("Not implemented");
}
std::time_t suPath::astamp(const suPath& fp)
{
    return fp.astamp();
}
std::string suPath::string() const
{
    return m_path.string();
}
std::string suPath::u8string() const
{
    return m_path.u8string();
}
std::filesystem::path suPath::path() const
{
    return m_path;
}