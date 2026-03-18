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
#include <winioctl.h>
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
        } {
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
/**
     * @brief Resize file to target_size. (Windows 平台)
     */
bool resize_file_append_zeros(const wchar_t* path, uint64_t target_size)
{
    HANDLE hFile = CreateFileW(
        path,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        std::cerr << "CreateFile failed: " << GetLastError() << "\n";
        return false;
    }

    LARGE_INTEGER current_size_li;
    if (!GetFileSizeEx(hFile, &current_size_li))
    {
        CloseHandle(hFile);
        return false;
    }
    uint64_t current_size = static_cast<uint64_t>(current_size_li.QuadPart);

    if (current_size >= target_size)
    {
        CloseHandle(hFile);
        return true;
    }

    // 扩展文件大小
    LARGE_INTEGER li;
    li.QuadPart = static_cast<LONGLONG>(target_size);
    SetFilePointerEx(hFile, li, nullptr, FILE_BEGIN);
    SetEndOfFile(hFile);

    // 优先尝试 FSCTL_SET_ZERO_DATA (高效，但不支持 FAT32/exFAT)
    FILE_ZERO_DATA_INFORMATION zeroInfo;
    zeroInfo.FileOffset.QuadPart = static_cast<LONGLONG>(current_size);
    zeroInfo.BeyondFinalZero.QuadPart = static_cast<LONGLONG>(target_size);

    DWORD bytesReturned;
    if (!DeviceIoControl(hFile,
                         FSCTL_SET_ZERO_DATA,
                         &zeroInfo,
                         sizeof(zeroInfo),
                         nullptr,
                         0,
                         &bytesReturned,
                         nullptr))
    {
        // 【降级逻辑】：如果是不支持的文件系统，手动写入 0
        SetFilePointerEx(hFile, current_size_li, nullptr, FILE_BEGIN);
        const DWORD buffer_size = 1024 * 1024;
        std::vector<char> buffer(buffer_size, 0);
        uint64_t remaining = target_size - current_size;

        while (remaining > 0)
        {
            DWORD to_write = (remaining < buffer_size) ? static_cast<DWORD>(remaining) : buffer_size;
            DWORD written = 0;
            if (!WriteFile(hFile, buffer.data(), to_write, &written, nullptr) || written == 0)
            {
                std::cerr << "WriteFile fallback failed: " << GetLastError() << "\n";
                CloseHandle(hFile);
                return false;
            }
            remaining -= written;
        }
    }

    FlushFileBuffers(hFile);
    CloseHandle(hFile);
    return true;
}

#else
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>

/**
 * @brief Resize file to target_size. (Linux/POSIX 平台)
 */
bool resize_file_append_zeros(const char* path, size_t target_size)
{
    int fd = open(path, O_RDWR | O_CREAT, 0644);
    if (fd == -1)
    {
        std::cerr << "open failed: " << strerror(errno) << "\n";
        return false;
    }

    struct stat st;
    if (fstat(fd, &st) != 0)
    {
        close(fd);
        return false;
    }
    off_t current_size = st.st_size;

    if (static_cast<size_t>(current_size) >= target_size)
    {
        close(fd);
        return true;
    }

    off_t size_to_add = static_cast<off_t>(target_size) - current_size;

    // 【性能优化】优先使用 posix_fallocate (内核级物理空间分配+置零)
    // 注意：posix_fallocate 直接返回错误码，不设置 errno
    int fallocate_ret = posix_fallocate(fd, current_size, size_to_add);

    if (fallocate_ret != 0)
    {
        // 【降级逻辑】如果文件系统不支持 posix_fallocate，降级为你的 pwrite 循环
        // 此时不用显式调用 ftruncate，一直 pwrite 到 target_size 自然会扩展文件
        const size_t buffer_size = 1024 * 1024;
        std::vector<char> buffer(buffer_size, 0);

        off_t offset = current_size;
        off_t remaining = size_to_add;

        while (remaining > 0)
        {
            size_t to_write = (remaining < static_cast<off_t>(buffer_size))
                                  ? static_cast<size_t>(remaining)
                                  : buffer_size;

            ssize_t written = pwrite(fd, buffer.data(), to_write, offset);
            if (written <= 0)
            {
                // EINTR 表示被信号打断，可以重试；这里为了简洁直接当做失败
                std::cerr << "pwrite fallback failed: " << strerror(errno) << "\n";
                close(fd);
                return false;
            }
            offset += written;
            remaining -= written;
        }
    }

    // 确保数据落盘 (posix_fallocate 不保证落盘，也需要 fsync)
    fsync(fd);
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
        if (!f)
        {
            m_err = "Failed to create file: " + m_path.string();
            return false;
        }
    }
    if (const size_t size = file_size(); len > size)
    {
        try
        {
#if _WIN32
            resize_file_append_zeros(m_path.wstring().c_str(), len);
#else
            resize_file_append_zeros(m_path.string().c_str(), len);
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