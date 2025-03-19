//
// Created by dell on 2023/8/10.
//

#include "silly_memory_map.h"
#include <files/silly_file.h>
using namespace silly::file;
#if WIN32
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

memory_map::memory_map(void)
{
}
memory_map::~memory_map(void)
{
}

memory_map::cur* memory_map::ptr(const size_t& offset)
{
    if (m_mmap && offset < m_map_len)
    {
        return m_mmap + offset;
    }
    return nullptr;
}
bool memory_map::read(memory_map::cur* dst, const size_t& size, const size_t& offset)
{
    if (m_mmap && dst && size + offset < m_map_len)
    {
        memcpy(dst, m_mmap + offset, size);
        return true;
    }
    return false;
}
bool memory_map::write(memory_map::cur* src, const size_t& size, const size_t& offset)
{
    bool status = false;
    if (m_mmap)
    {
        std::unique_lock lock(m_w_mutex);
        // if (!strcpy_s(m_mmap + offset, size, src))
        if (memcpy(m_mmap + offset, src, size) && sync())
        {
            status = true;
        }
    }

    return status;
}

bool memory_map::sync()
{
    if (!is_open())
    {
        return false;
    }

    if (m_mmap)
    {
#ifdef _WIN32
        if (::FlushViewOfFile(m_mmap, m_map_len) == 0 || ::FlushFileBuffers(m_hdl_file) == 0)
#else  // POSIX
        if (::msync(m_mmap, m_map_len, MS_SYNC) != 0)
#endif
        {
            return false;
        }
    }
    return true;
}
void memory_map::close()
{
    if (unmap())
    {
        clear();
    }
}

void memory_map::cleanup_and_throw(const char* msg)
{
#ifdef WIN32
    DWORD error = GetLastError();
    if (m_hdl_map != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_hdl_map);
    if (m_hdl_file != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_hdl_file);
    SetLastError(error);
#else
    int error = errno;
    if (m_hdl_file != INVALID_HANDLE_VALUE)
        ::close(m_hdl_file);
    errno = error;
#endif
    throw std::runtime_error(msg);
}
bool memory_map::map_file()
{
    try
    {
        try_map_file();
    }
    catch (const std::exception& e)
    {
        /*if (m_param.hint)
        {
            m_param.hint = 0;
            try_map_file();
        }
        else
        {
            throw;
        }*/
        return false;
    }
    return true;
}
void memory_map::try_map_file()
{
    int64_t length = filesize();  // TODO 这个是假的

#ifdef _WIN32
#ifndef NDEBUG
    std::stringstream ss;
    ss << "TryMapFile, 线程: " << std::this_thread::get_id();
    SLOG_DEBUG(ss.str())
#endif
    const int64_t aligned_offset = make_offset_page_aligned(m_param.offset);
    const int64_t length_to_map = m_param.offset - aligned_offset + length;
    const int64_t max_file_size = m_param.offset + length;
    DWORD access = m_param.flag == access_mode::Read ? PAGE_READONLY : PAGE_READWRITE;
    m_hdl_map = ::CreateFileMapping(m_hdl_file, 0, access, win::int64_high(max_file_size), win::int64_low(max_file_size), 0);
    if (m_hdl_map == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to create file mapping");
    }
    // void* mapping_start = ::MapViewOfFile(m_hdl_map, m_param.flag == access_mode::Read ? FILE_MAP_READ : FILE_MAP_WRITE, win::int64_high(aligned_offset), win::int64_low(aligned_offset), length_to_map);
    void* mapping_start = ::MapViewOfFile(m_hdl_map, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (mapping_start == nullptr)
    {
        // Close file handle if mapping it failed.
        ::CloseHandle(m_hdl_map);
        throw std::runtime_error("Failed to create file mapping");
    }
#else  // POSIX
    void* mapping_start = ::mmap(0, m_map_len, m_param.flag == access_mode::Read ? PROT_READ : PROT_WRITE, MAP_SHARED, m_hdl_file, m_map_offset);
#endif
    m_mmap = static_cast<char*>(mapping_start);
}
bool memory_map::resize(size_t size)
{
    return false;
}

void memory_map::clear()
{
    m_mmap = 0;
    m_map_len = 0;
#ifdef WIN32
    m_hdl_file = INVALID_HANDLE_VALUE;
    m_hdl_map = INVALID_HANDLE_VALUE;
#endif
}
std::uintmax_t memory_map::filesize()
{
    if (is_open())
    {
    }
#ifdef WIN32
    // 获取文件大小
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(m_hdl_file, &fileSize))
    {
        return fileSize.QuadPart;
    }
#else
    // 获取文件大小
    struct stat sb;
    if (fstat(m_hdl_file, &sb) == -1)
    {
        return sb.st_size;
    }
#endif
    return 0;
}

bool memory_map::open_file()
{
#if WIN32
#ifndef NDEBUG
    std::stringstream ss;
    ss << "OpenFile, 线程: " << std::this_thread::get_id();
    SLOG_DEBUG(ss.str())
#endif
    DWORD access = GENERIC_READ | GENERIC_WRITE;
    DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD disposition = OPEN_EXISTING;
    DWORD attr = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN;
    m_hdl_file = ::CreateFileW(m_param.path.wstring().c_str(), access, share, 0, disposition, attr, 0);
    if (m_hdl_file == INVALID_HANDLE_VALUE)
    {
        return false;
    }
#else
    // Open file
    int mode = (m_param.flag == access_mode::Read ? O_RDONLY : O_RDWR);
    m_hdl_file = ::open(m_param.path.c_str(), mode, (mode_t)0600);
    if (m_hdl_file == INVALID_HANDLE_VALUE)
    {
        return false;
    }
#endif
    return true;
}

bool memory_map::unmap()
{
#ifndef NDEBUG
    std::stringstream ss;
    ss << "Umap, 线程: " << std::this_thread::get_id();
    SLOG_DEBUG(ss.str())
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
        ::CloseHandle(m_hdl_map);
    }
#else  // POSIX
    if (is_mapped())
    {
        ::munmap(m_mmap, m_map_len);
    }
#endif

    // If `m_hdl_file` was obtained by our opening it (when map is called with
    // a path, rather than an existing file handle), we need to close it,
    // otherwise it must not be closed as it may still be used outside this
    // instance.
#ifdef _WIN32
    ::CloseHandle(m_hdl_file);
#else  // POSIX
    ::close(m_hdl_file);
#endif

    clear();
    return true;
}

bool memory_map::is_open()
{
    return m_hdl_file != INVALID_HANDLE_VALUE;
}
bool memory_map::is_mapped()
{
#ifdef _WIN32
    return m_hdl_map != INVALID_HANDLE_VALUE;
#else  // POSIX
    return is_open();
#endif
}

bool memory_map::open(const std::string& file, const int& mode, const int64_t& off)
{
    if (!std::filesystem::exists(file))
    {
        std::cerr << "文件不存在" << std::endl;
        return false;
    }
   /* if (mode != access_mode::Read)
    {
        std::cerr << "目前仅完整支持读取功能" << std::endl;
        return false;
    }*/
    m_param.path = std::filesystem::path(file);
    m_param.flag = static_cast<memory_map::access_mode>(m_param.flag);
    m_param.offset = off;
    m_map_len = file::utils::size(m_param.path.string());
    return open();
}
bool memory_map::open(const memory_map::param& p)
{
    m_param = p;
    return open();
}

bool memory_map::open()
{
    open_file();
    return map_file();  // May modify p.hint
}
