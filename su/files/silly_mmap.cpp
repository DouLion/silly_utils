//
// Created by dell on 2023/8/10.
//

#include "silly_mmap.h"
#include <files/silly_file.h>
using namespace silly;
#if WIN32
#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4

#define MAP_FILE 0
#define MAP_SHARED 1
#define MAP_PRIVATE 2
#define MAP_TYPE 0xf
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS

#define MAP_FAILED ((void*)-1)

/* Flags for msync. */
#define MS_ASYNC 1
#define MS_SYNC 2
#define MS_INVALIDATE 4
#endif
mmap::~mmap()
{
    close();
}

bool mmap::open(const std::string& file, const int& mode, const int64_t& off)
{
    /* if (!std::filesystem::exists(file))
     {
         return false;
     }*/
    if(mode != param::ReadOnly)
    {
         std::cerr << "目前仅完整支持读取功能" << std::endl;
        return false;
    }
    m_param.path = std::filesystem::path(file);
    m_param.flag = static_cast<mmap::param::flags>(mode);
    m_param.offset = off;
    m_is_wide = m_param.path.string() != m_param.path.u8string();
    m_filesize = file::tools::size(m_param.path.string());
    return open();
}
bool mmap::open(const mmap::param& p)
{
    m_param = p;
    return open();
}

bool mmap::open()
{
    open_file();
    return map_file();  // May modify p.hint
}

mmap::cur* mmap::at(const size_t& offset)
{
    if (m_mmap && offset < m_filesize)
    {
        return m_mmap + offset;
    }
    return nullptr;
}
bool mmap::read(mmap::cur* dst, const size_t& size, const size_t& offset)
{
    if (m_mmap && dst && size + offset < m_filesize)
    {
        memcpy(dst, m_mmap + offset, size);
        return true;
    }
    return false;
}
bool mmap::write(mmap::cur* src, const size_t& size, const size_t& offset)
{
    bool status = false;
    if (m_mmap)
    {
        std::unique_lock lock(m_w_mutex);
        // if (!strcpy_s(m_mmap + offset, size, src))
        if (memcpy(m_mmap + offset, src, size))
        {
            m_filesize += size;
            status = true;
        }
    }

    return status;
}
void mmap::close()
{
    if (unmap_file())
    {
        clear();
    }
}

void mmap::cleanup_and_throw(const char* msg)
{
#ifdef WIN32
    DWORD error = GetLastError();
    if (m_h_map_file != NULL)
        ::CloseHandle(m_h_map_file);
    if (m_h_file != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_h_file);
    SetLastError(error);
#else
    int error = errno;
    if (m_h_file != 0)
        ::close(m_h_file);
    errno = error;
#endif
    throw std::runtime_error(msg);
}
bool mmap::map_file()
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
void mmap::try_map_file()
{
#ifdef WIN32
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4293)
#endif
    const DWORD dwFileOffsetLow = (sizeof(int64_t) <= sizeof(DWORD)) ? (DWORD)m_param.offset : (DWORD)(m_param.offset & 0xFFFFFFFFL);
    const DWORD dwFileOffsetHigh = (sizeof(int64_t) <= sizeof(DWORD)) ? (DWORD)0 : (DWORD)((m_param.offset >> 32) & 0xFFFFFFFFL);

    const int64_t maxSize = m_param.offset + (int64_t)m_size;

    const DWORD dwMaxSizeLow = (sizeof(int64_t) <= sizeof(DWORD)) ? (DWORD)maxSize : (DWORD)(maxSize & 0xFFFFFFFFL);
    const DWORD dwMaxSizeHigh = (sizeof(int64_t) <= sizeof(DWORD)) ? (DWORD)0 : (DWORD)((maxSize >> 32) & 0xFFFFFFFFL);
    // Create mapping
    DWORD protect = m_param.flag == param::ReadOnly ? PAGE_READONLY : PAGE_READWRITE;
    DWORD desired = m_param.flag == param::ReadOnly ? 0 | FILE_MAP_READ : 0 | FILE_MAP_WRITE;

#ifdef _MSC_VER
#pragma warning(pop)
#endif
    m_h_map_file = ::CreateFileMapping(m_h_file, NULL, protect, dwMaxSizeHigh, dwMaxSizeLow, NULL);
    if (m_h_map_file == NULL)
        cleanup_and_throw("failed create mapping");

    void* data = ::MapViewOfFile(m_h_map_file, desired, dwFileOffsetHigh, dwFileOffsetLow, (SIZE_T)m_size);
    if (!data)
        cleanup_and_throw("failed mapping view");
#else
    void* data = ::BOOST_IOSTREAMS_FD_MMAP(const_cast<char*>(m_param.hint), m_size, readonly ? PROT_READ : (PROT_READ | PROT_WRITE), priv ? MAP_PRIVATE : MAP_SHARED, m_h_file, m_param.offset);
    if (data == MAP_FAILED)
        cleanup_and_throw("failed mapping file");
#endif
    m_mmap = static_cast<char*>(data);
}
bool mmap::resize(size_t size)
{
    return false;
}
bool mmap::unmap_file()
{
#ifdef WIN32
    bool error = false;
    if (m_mmap)
        error = !::UnmapViewOfFile(m_mmap) || error;
    if (m_h_file != INVALID_HANDLE_VALUE)
        error = !::CloseHandle(m_h_map_file) || error;
    if (m_fd > 0)
        ::close(m_fd);
    m_fd = 0;
    m_h_map_file = NULL;
    return !error;
#else
    return ::munmap(m_mmap, m_filesize) == 0;
#endif
}
void mmap::clear()
{
    m_mmap = 0;
    m_filesize = 0;
#ifdef WIN32
    m_h_file = INVALID_HANDLE_VALUE;
    m_h_map_file = NULL;
#else
    m_h_file = 0;
#endif
}
std::uintmax_t mmap::filesize()
{
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(m_h_file, &fileSize))
    {
        throw std::system_error(GetLastError(), std::system_category(), "Failed querying file size");
    }
    return fileSize.QuadPart;
}
bool mmap::open_file()
{
#if WIN32
    // Open file
    int oflag = m_param.flag == param::ReadOnly ? O_RDONLY : O_RDWR | _O_CREAT;
    int prot = m_param.flag == param::ReadOnly ? PROT_READ : PROT_WRITE;
    int ipriv = m_param.flag == param::Private ? MAP_PRIVATE : MAP_SHARED;
    m_fd = _open(m_param.path.string().c_str(), oflag);
    if (m_fd <= 0)
    {
        return false;
    }
    struct _stat64 stat;
    int ret = _fstati64(m_fd, &stat);

    if (stat.st_size == 0
        /* Usupported protection combinations */
        || prot == PROT_EXEC)
    {
        return false;
    }
    else
    {
        m_size = stat.st_size;
    }


    m_h_file = ((ipriv & MAP_ANONYMOUS) == 0) ? (HANDLE)_get_osfhandle(m_fd) : INVALID_HANDLE_VALUE;

    if ((ipriv & MAP_ANONYMOUS) == 0 && m_h_file == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    /*DWORD dwDesiredAccess = readonly ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE);
    DWORD dwCreationDisposition = (m_param.new_file_size != 0 && !readonly) ? CREATE_ALWAYS : OPEN_EXISTING;
    DWORD dwFlagsandAttributes = readonly ? FILE_ATTRIBUTE_READONLY : FILE_ATTRIBUTE_TEMPORARY;
    m_h_file = m_is_wide ? ::CreateFileW(m_param.path.wstring().c_str(), dwDesiredAccess, FILE_SHARE_READ, NULL, dwCreationDisposition, dwFlagsandAttributes, NULL)
                         : ::CreateFileA(m_param.path.string().c_str(), dwDesiredAccess, FILE_SHARE_READ, NULL, dwCreationDisposition, dwFlagsandAttributes, NULL);
    if (m_h_file == INVALID_HANDLE_VALUE)
        cleanup_and_throw("failed opening file");

    // Set file size
    if (m_param.new_file_size != 0 && !readonly)
    {
        LONG sizehigh = (m_param.new_file_size >> (sizeof(LONG) * 8));
        LONG sizelow = (m_param.new_file_size & 0xffffffff);
        DWORD result = ::SetFilePointer(m_h_file, sizelow, &sizehigh, FILE_BEGIN);
        if ((result == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR) || !::SetEndOfFile(m_h_file))
            cleanup_and_throw("failed setting file size");
    }

    // Determine file size. Dynamically locate GetFileSizeEx for compatibility
    // with old Platform SDK (thanks to Pavel Vozenilik).
    typedef BOOL(WINAPI * func)(HANDLE, PLARGE_INTEGER);
    HMODULE hmod = ::GetModuleHandleA("kernel32.dll");
    func get_size = reinterpret_cast<func>(::GetProcAddress(hmod, "GetFileSizeEx"));
    if (get_size)
    {
        LARGE_INTEGER info;
        if (get_size(m_h_file, &info))
        {
            std::intmax_t size = ((static_cast<std::intmax_t>(info.HighPart) << 32) | info.LowPart);
            m_size = static_cast<std::size_t>(m_param.length != SIZE_MAX ? std::min<std::intmax_t>(m_param.length, size) : size);
        }
        else
        {
            cleanup_and_throw("failed querying file size");
            return;
        }
    }
    else
    {
        DWORD hi;
        DWORD low;
        if ((low = ::GetFileSize(m_h_file, &hi)) != INVALID_FILE_SIZE)
        {
            std::intmax_t size = (static_cast<std::intmax_t>(hi) << 32) | low;
            m_size = static_cast<std::size_t>(m_param.length != SIZE_MAX ? std::min<std::intmax_t>(m_param.length, size) : size);
        }
        else
        {
            cleanup_and_throw("failed querying file size");
            return;
        }
    }*/
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
    m_h_file = ::open(m_param.path.c_str(), flags, S_IRWXU);
    if (errno != 0)
        cleanup_and_throw("failed opening file");

    //--------------Set file size---------------------------------------------//

    if (m_param.new_file_size != 0 && !readonly)
        if (BOOST_IOSTREAMS_FD_TRUNCATE(m_h_file, m_param.new_file_size) == -1)
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
        success = ::BOOST_IOSTREAMS_FD_FSTAT(m_h_file, &info) != -1;
        m_size = info.st_size;
    }
    if (!success)
        cleanup_and_throw("failed querying file size");
#endif
    return true;
}