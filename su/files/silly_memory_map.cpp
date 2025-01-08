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

inline std::wstring s_2_ws(const std::string& s)
{
    std::wstring ret;
    if (!s.empty())
    {
        ret.resize(s.size());
        int wide_char_count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), static_cast<int>(s.size()), &ret[0], static_cast<int>(s.size()));
        ret.resize(wide_char_count);
    }
    return ret;
}
}  // namespace win
#endif

memory_map::memory_map(void)
{
}
memory_map::~memory_map(void)
{
}

memory_map::cur* memory_map::at(const size_t& offset)
{
    if (m_mmap && m_param.offset < m_map_len)
    {
        return m_mmap + m_param.offset;
    }
    return nullptr;
}
bool memory_map::read(memory_map::cur* dst, const size_t& size, const size_t& offset)
{
    if (m_mmap && dst && size + m_param.offset < m_map_len)
    {
        memcpy(dst, m_mmap + m_param.offset, size);
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
        // if (!strcpy_s(m_mmap + m_param.offset, size, src))
        if (memcpy(m_mmap + m_param.offset, src, size))
        {
            m_map_len += size;
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
        if (::msync(get_mapping_start(), m_map_len, MS_SYNC) != 0)
#endif
        {
            return false;
        }
    }
#ifdef _WIN32
    if (::FlushFileBuffers(m_hdl_file) == 0)
    {
        return false;
    }
#endif
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
    if (m_hdl_file != 0)
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
    const int64_t length = 0;  // TODO 这个是假的
    const int64_t aligned_offset = make_offset_page_aligned(m_param.offset);
    const int64_t length_to_map = m_param.offset - aligned_offset + length;
#ifdef _WIN32
    const int64_t max_file_size = m_param.offset + length;
    m_hdl_map = ::CreateFileMapping(m_hdl_file, 0, m_param.flag == param::eAccess::ReadOnly ? PAGE_READONLY : PAGE_READWRITE, win::int64_high(max_file_size), win::int64_low(max_file_size), 0);
    if (m_hdl_map == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to create file mapping");
    }
    void* mapping_start = ::MapViewOfFile(m_hdl_map, m_param.flag == param::eAccess::ReadOnly ? FILE_MAP_READ : FILE_MAP_WRITE, win::int64_high(aligned_offset), win::int64_low(aligned_offset), length_to_map);
    if (mapping_start == nullptr)
    {
        // Close file handle if mapping it failed.
        ::CloseHandle(m_hdl_map);
        throw std::runtime_error("Failed to create file mapping");
    }
#else  // POSIX
    char* mapping_start = static_cast<char*>(::memory_map(0,  // Don't give hint as to where to map.
                                                    length_to_map,
                                                    m_param.flag == param::eAccess::read ? PROT_READ : PROT_WRITE,
                                                    MAP_SHARED,
                                                    m_hdl_file,
                                                    aligned_offset));
    if (mapping_start == MAP_FAILED)
    {
        error = detail::last_error();
        return {};
    }
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

bool memory_map::open_file()
{
#if WIN32
    DWORD access = m_param.flag == param::eAccess::ReadOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;

    m_hdl_file = ::CreateFileW(m_param.path.wstring().c_str(), access, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_hdl_file == INVALID_HANDLE_VALUE)
    {
        return false;
    }
#else
    // Open file
    int flags = (readonly ? O_RDONLY : O_RDWR);
    if (m_param.new_file_size != 0 && !readonly)
        flags |= (O_CREAT | O_TRUNC);
#ifdef _LARGEFILE64_SOURCE
    flags |= O_LARGEFILE;
#endif
    errno = 0;
    if (m_param.path.is_wide())
    {
        errno = EINVAL;
        cleanup_and_throw("wide path not supported here");
    }  // happens on CYGWIN
    m_hdl_file = ::open(m_param.path.c_str(), flags, S_IRWXU);
    if (errno != 0)
        cleanup_and_throw("failed opening file");

    //--------------Set file size---------------------------------------------//

    if (m_param.new_file_size != 0 && !readonly)
        if (BOOST_IOSTREAMS_FD_TRUNCATE(m_hdl_file, m_param.new_file_size) == -1)
            cleanup_and_throw("failed setting file size");

    //--------------Determine file size---------------------------------------//

    bool success = true;
    if (m_param.length != SIZE_MAX)
    {
        m_size = m_param.length;
    }
    else
    {
        struct BOOST_IOSTREAMS_FD_STAT info;
        success = ::BOOST_IOSTREAMS_FD_FSTAT(m_hdl_file, &info) != -1;
        m_size = info.st_size;
    }
    if (!success)
        cleanup_and_throw("failed querying file size");
#endif
}

bool memory_map::unmap()
{
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
    if (data_)
    {
        ::munmap(const_cast<pointer>(get_mapping_start()), m_map_len);
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
        return false;
    }
    if (mode != param::ReadOnly)
    {
        std::cerr << "目前仅完整支持读取功能" << std::endl;
        return false;
    }
    m_param.path = std::filesystem::path(file);
    m_param.flag = static_cast<memory_map::param::eAccess>(m_param.flag);
    m_param.offset = off;
    m_is_wide = m_param.path.string() != m_param.path.u8string();
    m_map_len = file::tools::size(m_param.path.string());
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
