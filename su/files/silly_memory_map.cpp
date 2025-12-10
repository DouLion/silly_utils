//
// Created by dell on 2023/8/10.
//

#include "silly_memory_map.h"
#include <files/silly_file.h>
#include <log/silly_log.h>

inline size_t page_size()
{
    static const size_t ret = [] {
#ifdef _WIN32
        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);
        return SystemInfo.dwAllocationGranularity;
#else
        return sysconf(_SC_PAGE_SIZE);
#endif
    }();
    return ret;
}
size_t inline make_offset_page_aligned(size_t offset) noexcept
{
    const size_t page_size_ = page_size();
    // Use integer division to round down to the nearest page alignment.
    return offset / page_size_ * page_size_;
}
#ifdef IS_WIN32
namespace win
{
/** Returns the 4 upper bytes of an 8-byte integer. */
inline DWORD int64_high(int64_t n) noexcept
{
    return n >> 32;
}

/** Returns the 4 lower bytes of an 8-byte integer. */
inline DWORD int64_low(int64_t n) noexcept
{
    return n & 0xffffffff;
}

}  // namespace win
#endif

#ifdef _WIN32
// Windows 定义
#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5
#else
// Linux/Unix 兼容定义
#define CREATE_NEW (O_CREAT | O_EXCL | O_RDWR)
#define CREATE_ALWAYS (O_CREAT | O_TRUNC | O_RDWR)
#define OPEN_EXISTING O_RDWR
#define OPEN_ALWAYS (O_CREAT | O_RDWR)
#define TRUNCATE_EXISTING (O_TRUNC | O_RDWR)
#endif


suMemMapFile::Ptr suMemMapFile::ptr(const size_t& offset) const
{
    if (m_mmap && offset < m_map_len)
    {
        return m_mmap + offset;
    }
    return nullptr;
}
bool suMemMapFile::read(suMemMapFile::Ptr dst, const size_t& size, const size_t& offset) const
{
    if (m_mmap && dst && size + offset < m_map_len)
    {
        memcpy(dst, m_mmap + offset, size);
        return true;
    }
    return false;
}
bool suMemMapFile::write(suMemMapFile::Ptr src, const size_t& size, const size_t& offset)
{
    bool status = false;
    if (m_mmap)
    {
        std::unique_lock lock(m_w_mutex);
        // if (!strcpy_s(m_mmap + offset, size, src))
        if (memcpy(m_mmap + offset, src, size))
        {
#ifdef _WIN32
            if (::FlushViewOfFile(m_mmap + offset, size))
#else  // POSIX
            if (::msync(m_mmap + offset, size, MS_SYNC) != 0)
#endif
            {
                status = true;
            }
        }
    }

    return status;
}

// bool suMemMapFile::sync()
//{
//     if (!is_open())
//     {
//         return false;
//     }
//
//     if (m_mmap)
//     {
// #ifdef _WIN32
//         if (::FlushViewOfFile(m_mmap, m_map_len) == 0 || ::FlushFileBuffers(m_file_hdl) == 0)
// #else  // POSIX
//         if (::msync(m_mmap, m_map_len, MS_SYNC) != 0)
// #endif
//         {
//             return false;
//         }
//     }
//     return true;
// }
void suMemMapFile::close(const bool& del)
{
    if (unmap())
    {
        clear();
    }
    if (del)
    {
        if (m_file.exists())
        {
            supath::rmfile(m_file);
        }
    }
}

void suMemMapFile::cleanup_and_throw(const char* msg)
{
#ifdef WIN32
    DWORD error = GetLastError();
    if (m_map_hdl != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_map_hdl);
    if (m_file_hdl != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_file_hdl);
    SetLastError(error);
#else
    int error = errno;
    if (m_file_hdl != INVALID_HANDLE_VALUE)
        ::close(m_file_hdl);
    errno = error;
#endif
    throw std::runtime_error(msg);
}
bool suMemMapFile::map_file()
{
    try
    {
        try_map_file();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}
void suMemMapFile::try_map_file()
{
#ifdef _WIN32
#ifndef NDEBUG
    std::stringstream ss;
    ss << "TryMapFile, 线程: " << std::this_thread::get_id();
    SLOG_DEBUG(ss.str())
#endif
    DWORD access = PAGE_READONLY;
    if (m_mode == Write)
    {
        access = PAGE_READWRITE;
    }
    m_map_hdl = ::CreateFileMapping(m_file_hdl, 0, access, 0, 0, 0);
    if (m_map_hdl == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to create file mapping");
    }
    access = access == PAGE_READONLY ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS;

    // 总是映射完整文件, Windows 的 MapViewOfFile要求：
    // 映射偏移量（dwFileOffsetHigh/Low）必须是内存页大小的整数倍（通常是 4KB = 4096 字节）。
    // 所以不能直接从 offset = 10开始映射，必须从最近的页边界开始。
    void* mapping_start = ::MapViewOfFile(m_map_hdl, access,  0, 0, m_file_len);
    if (mapping_start == nullptr)
    {
        // Close file handle if mapping it failed.
        ::CloseHandle(m_map_hdl);
        throw std::runtime_error("Failed to create file mapping");
    }
#else  // POSIX
    void* mapping_start = ::mmap(0, m_map_len, m_mode == eMMFMode::Read ? PROT_READ : PROT_WRITE, MAP_SHARED, m_file_hdl, 0);
#endif
    m_mmap = static_cast<Ptr>(mapping_start);
    m_mmap += m_map_offset;
}
bool suMemMapFile::resize(size_t size)
{
    return false;
}

void suMemMapFile::clear()
{
    m_mmap = 0;
    m_map_len = 0;
#ifdef WIN32
    m_file_hdl = INVALID_HANDLE_VALUE;
    m_map_hdl = INVALID_HANDLE_VALUE;
#endif
}
std::uintmax_t suMemMapFile::filesize()
{
    if (is_open())
    {
    }
#ifdef WIN32
    // 获取文件大小
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(m_file_hdl, &fileSize))
    {
        return fileSize.QuadPart;
    }
#else
    // 获取文件大小
    struct stat sb;
    if (fstat(m_file_hdl, &sb) == -1)
    {
        return sb.st_size;
    }
#endif
    return 0;
}

bool suMemMapFile::open_file()
{
#ifdef IS_WIN32
#ifndef NDEBUG
    std::stringstream ss;
    ss << "OpenFile, 线程: " << std::this_thread::get_id();
    SLOG_DEBUG(ss.str())
#endif
    DWORD access = GENERIC_READ | GENERIC_WRITE;
    DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
    // DWORD disposition = CREATE_ALWAYS;
    DWORD attr = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN;
    m_file_hdl = ::CreateFileW(m_file.wstring().c_str(), access, share, 0, m_disposition, attr, 0);
    if (m_file_hdl == INVALID_HANDLE_VALUE)
    {
        return false;
    }
#else
    // Open file
    int mode = (m_mode == eMMFMode::Read ? O_RDONLY : O_RDWR);
    m_file_hdl = ::open(m_file.string().c_str(), mode, (mode_t)0600);
    if (m_file_hdl == INVALID_HANDLE_VALUE)
    {
        return false;
    }
#endif
    return true;
}

bool suMemMapFile::unmap()
{
#ifndef NDEBUG
    std::stringstream ss;
#endif
    if (!is_open())
    {
        return false;
    }
    // TODO do we care about errors here?
#ifdef _WIN32
    if (is_mapped())
    {
        ::UnmapViewOfFile(m_mmap);
        ::CloseHandle(m_map_hdl);
    }
#else  // POSIX
    if (is_mapped())
    {
        ::munmap(m_mmap, m_map_len);
    }
#endif

    // If `m_file_hdl` was obtained by our opening it (when map is called with
    // a path, rather than an existing file handle), we need to close it,
    // otherwise it must not be closed as it may still be used outside this
    // instance.
#ifdef _WIN32
    ::CloseHandle(m_file_hdl);
#else  // POSIX
    ::close(m_file_hdl);
#endif

    clear();
    return true;
}

bool suMemMapFile::is_open() const
{
    return m_file_hdl != INVALID_HANDLE_VALUE;
}
bool suMemMapFile::is_mapped() const
{
#ifdef _WIN32
    return m_map_hdl != INVALID_HANDLE_VALUE;
#else  // POSIX
    return is_open();
#endif
}

bool suMemMapFile::open(const supath& file, const eMMFMode& mode, const int64_t& offset)
{
    Param p;
    p.path = file;
    p.mode = mode;
    p.offset = offset;
    if (mode == eMMFMode::Write)
    {
        if (file.exists())
        {
            std::cerr << "使用open(const Param& p) 函数 明确写模式" << std::endl;
            return false;
        }
        //p.disposition = CreateAlways;
    }
    else
    {
        p.disposition = OpenExisting;
    }
    return open(p);
}
bool suMemMapFile::open(const suMemMapFile::Param& p)
{
    std::scoped_lock<std::mutex> lock(m_oc);
    // 打开文件未完成时不允许写
    std::scoped_lock<std::mutex> lock2(m_w_mutex);
    m_file = p.path;
    m_mode = p.mode;
    m_file_len = m_file.file_size();
    //if (p.mode == Read)
    {
        if (m_file_len <= 0)
        {
            SLOG_ERROR("文件不存在或大小为0,无法读打开")
            return false;
        }
        m_disposition = p.disposition;
        m_map_offset = p.offset;
        if (m_map_offset >= m_file_len)
        {
            SLOG_ERROR("偏移不应超过文件大小")
            return false;
        }
        if (p.map_size == 0)
        {
            m_map_len = m_file_len - m_map_offset;
        }
        else
        {
            m_map_len = p.map_size;
        }

    }
    /*else if (p.mode == Write)
    {
        
        return false;
    }
    else
    {
        return false;
    }*/

    return open();
}

bool suMemMapFile::open()
{
    open_file();
    return map_file();  // May modify p.hint
}
