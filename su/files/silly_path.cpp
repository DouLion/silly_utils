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
    if (!exists())
    {
        std::filesystem::create_directories(m_path);
    }
    return is_dir();

}
bool suPath::mkdir(const suPath& path)
{
    return path.mkdir();
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
size_t suPath::file_size() const
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
size_t suPath::file_size(const suPath& fp)
{
    return fp.file_size();
}
#if _WIN32
bool extend_and_preallocate_file(const wchar_t* path, uint64_t target_size) {
    // 打开或创建文件，不覆盖已有内容
    HANDLE hFile = CreateFileW(
        path,
        GENERIC_READ | GENERIC_WRITE,  // 需要读权限以获取大小，写权限以扩展
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,                   // ← 关键：不覆盖已有文件
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFile failed: " << GetLastError() << "\n";
        return false;
    }

    // 获取当前文件大小
    LARGE_INTEGER current_size_li;
    if (!GetFileSizeEx(hFile, &current_size_li)) {
        std::cerr << "GetFileSizeEx failed\n";
        CloseHandle(hFile);
        return false;
    }
    uint64_t current_size = static_cast<uint64_t>(current_size_li.QuadPart);

    if (target_size <= current_size) {
        // 已经足够大，无需操作
        CloseHandle(hFile);
        return true;
    }

    // 定位到目标大小 - 1 的位置
    LARGE_INTEGER li;
    li.QuadPart = static_cast<LONGLONG>(target_size - 1);
    if (!SetFilePointerEx(hFile, li, nullptr, FILE_BEGIN)) {
        std::cerr << "SetFilePointerEx failed\n";
        CloseHandle(hFile);
        return false;
    }

    // 写入一个字节，强制 NTFS 分配从 current_size 到 target_size 的所有簇
    char dummy = 0;
    DWORD bytesWritten;
    if (!WriteFile(hFile, &dummy, 1, &bytesWritten, nullptr)) {
        std::cerr << "WriteFile failed: " << GetLastError() << "\n";
        CloseHandle(hFile);
        return false;
    }

    // 可选：刷新元数据（确保大小和分配持久化）
    FlushFileBuffers(hFile);

    CloseHandle(hFile);
    return true;
}
#else
bool extend_and_preallocate_file(const char* path, size_t target_size) {
    // 打开文件：O_RDWR | O_CREAT（不存在则创建），权限 0644
    int fd = open(path, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        throw std::system_error(errno, std::generic_category(), "open failed");
    }

    // 获取当前文件大小
    struct stat st;
    if (fstat(fd, &st) != 0) {
        close(fd);
        throw std::system_error(errno, std::generic_category(), "fstat failed");
    }
    off_t current_size = st.st_size;

    if (static_cast<size_t>(current_size) >= target_size) {
        // 已经足够大，无需操作
        close(fd);
        return true;
    }

    // 关键：预分配从 0 到 target_size 的整个范围
    // posix_fallocate 会：
    //   - 如果文件小于 target_size → 扩展它；
    //   - 保证 [0, target_size) 的磁盘空间被真正分配（非稀疏）；
    //   - 保留原有数据不变；
    int ret = posix_fallocate(fd, 0, static_cast<off_t>(target_size));
    if (ret != 0) {
        close(fd);
        throw std::system_error(ret, std::generic_category(), "posix_fallocate failed");
    }

    close(fd);
    return true;
}
#endif
bool suPath::resize_file(const size_t& len) const
{
    if (!exists())
    {
        auto pp = parent();
        if (!pp.mkdir())
        {
            m_err = pp.m_err;
            return false;
        }
        // 创建一个空文件
        std::ofstream f(m_path, std::ios::binary);
        if (!f) {
            m_err = "Failed to create file: " + m_path.string();
            return false;
        }
#ifndef NDEBUG
        std::cout << "创建新文件" << std::endl;
#endif
    }
    size_t size = file_size();
#ifndef NDEBUG
     std::cout << "现有大小:" << size << std::endl;
#endif
    if (len > size)
    {
        try
        {
#ifndef NDEBUG
            std::cout << "文件resize:" << u8string() << " "<< size <<  "=> " << len  << std::endl;
#endif
            #if _WIN32
            extend_and_preallocate_file(m_path.wstring().c_str(), len);
#else
            extend_and_preallocate_file(m_path.string().c_str(), len);
#endif
        }
        catch (std::exception& e)
        {
            m_err = e.what();
            return false;
        }
        return true;
    }
    return true;
}
bool suPath::resize_file(const suPath& fp, const size_t& len)
{
    return fp.resize_file(len);
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