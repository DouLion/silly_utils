//
// Created by dell on 2023/8/10.
//

#include "silly_mmap.h"
#include <filesystem>
#include <cstring>

#if IS_WIN32
// wstring转string
std::string ws2s(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, NULL);  //"C";
    setlocale(LC_ALL, "chs");
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char* _Dest = new char[_Dsize];
    memset(_Dest, 0, _Dsize);
    wcstombs(_Dest, _Source, _Dsize);
    std::string result = _Dest;
    delete[] _Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

// string转wstring
std::wstring s2ws(const std::string& s)
{
    setlocale(LC_ALL, "chs");
    const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t* _Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest, _Source, _Dsize);
    std::wstring result = _Dest;
    delete[] _Dest;
    setlocale(LC_ALL, "C");
    return result;
}
namespace w2l
{
typedef int64_t OffsetType;
#define PROT_NONE       0
#define PROT_READ       1
#define PROT_WRITE      2
#define PROT_EXEC       4

#define MAP_FILE        0
#define MAP_SHARED      1
#define MAP_PRIVATE     2
#define MAP_TYPE        0xf
#define MAP_FIXED       0x10
#define MAP_ANONYMOUS   0x20
#define MAP_ANON        MAP_ANONYMOUS

#define MAP_FAILED      ((void *)-1)

/* Flags for msync. */
#define MS_ASYNC        1
#define MS_SYNC         2
#define MS_INVALIDATE   4

static int __map_mman_error(const DWORD err, const int deferr)
{
    if (err == 0)
        return 0;
    //TODO: implement
    return err;
}

static DWORD __map_mmap_prot_page(const int prot)
{
    DWORD protect = 0;

    if (prot == PROT_NONE)
        return protect;

    if ((prot & PROT_EXEC) != 0)
    {
        protect = ((prot & PROT_WRITE) != 0) ?
                                             PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
    }
    else
    {
        protect = ((prot & PROT_WRITE) != 0) ?
                                             PAGE_READWRITE : PAGE_READONLY;
    }

    return protect;
}

static DWORD __map_mmap_prot_file(const int prot)
{
    DWORD desiredAccess = 0;

    if (prot == PROT_NONE)
        return desiredAccess;

    if ((prot & PROT_READ) != 0)
        desiredAccess |= FILE_MAP_READ;
    if ((prot & PROT_WRITE) != 0)
        desiredAccess |= FILE_MAP_WRITE;
    if ((prot & PROT_EXEC) != 0)
        desiredAccess |= FILE_MAP_EXECUTE;

    return desiredAccess;
}

void* mmap(void* addr, size_t len, int prot, int flags, int fildes, OffsetType off)
{
    HANDLE fm, h;

    void* map = MAP_FAILED;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4293)
#endif

    const DWORD dwFileOffsetLow = (sizeof(OffsetType) <= sizeof(DWORD)) ?
                                                                        (DWORD)off : (DWORD)(off & 0xFFFFFFFFL);
    const DWORD dwFileOffsetHigh = (sizeof(OffsetType) <= sizeof(DWORD)) ?
                                                                         (DWORD)0 : (DWORD)((off >> 32) & 0xFFFFFFFFL);
    const DWORD protect = __map_mmap_prot_page(prot);
    const DWORD desiredAccess = __map_mmap_prot_file(prot);

    const OffsetType maxSize = off + (OffsetType)len;

    const DWORD dwMaxSizeLow = (sizeof(OffsetType) <= sizeof(DWORD)) ?
                                                                     (DWORD)maxSize : (DWORD)(maxSize & 0xFFFFFFFFL);
    const DWORD dwMaxSizeHigh = (sizeof(OffsetType) <= sizeof(DWORD)) ?
                                                                      (DWORD)0 : (DWORD)((maxSize >> 32) & 0xFFFFFFFFL);

#ifdef _MSC_VER
#pragma warning(pop)
#endif

    errno = 0;

    if (len == 0
        /* Usupported protection combinations */
        || prot == PROT_EXEC)
    {
        errno = EINVAL;
        return MAP_FAILED;
    }

    h = ((flags & MAP_ANONYMOUS) == 0) ?
                                       (HANDLE)_get_osfhandle(fildes) : INVALID_HANDLE_VALUE;

    if ((flags & MAP_ANONYMOUS) == 0 && h == INVALID_HANDLE_VALUE)
    {
        errno = EBADF;
        return MAP_FAILED;
    }

    fm = CreateFileMapping(h, NULL, protect, dwMaxSizeHigh, dwMaxSizeLow, NULL);

    if (fm == NULL)
    {
        errno = __map_mman_error(GetLastError(), EPERM);
        return MAP_FAILED;
    }

    if ((flags & MAP_FIXED) == 0)
    {
        map = MapViewOfFile(fm, desiredAccess, dwFileOffsetHigh, dwFileOffsetLow, len);
    }
    else
    {
        map = MapViewOfFileEx(fm, desiredAccess, dwFileOffsetHigh, dwFileOffsetLow, len, addr);
    }

    CloseHandle(fm);

    if (map == NULL)
    {
        errno = __map_mman_error(GetLastError(), EPERM);
        return MAP_FAILED;
    }

    return map;
}



int munmap(void* addr, size_t len)
{
    if (UnmapViewOfFile(addr))
        return 0;

    errno = __map_mman_error(GetLastError(), EPERM);

    return -1;
}

int _mprotect(void* addr, size_t len, int prot)
{
    DWORD newProtect = __map_mmap_prot_page(prot);
    DWORD oldProtect = 0;

    if (VirtualProtect(addr, len, newProtect, &oldProtect))
        return 0;

    errno = __map_mman_error(GetLastError(), EPERM);

    return -1;
}
}

#else

#endif

silly_mmap::silly_mmap(const std::string& file)
{
    m_file = file;
}

silly_mmap::~silly_mmap()
{
    close_m();
}

bool silly_mmap::open_m(const std::string& file, const silly_mmap::enum_mmap_open_mode mode)
{
    bool status = false;
    m_file = file;
#if IS_WIN32
    m_h_file = nullptr;
    if (mode == enum_mmap_open_mode::emomRead)
    {
        status = windows_open_read();
    }
    else if (mode == enum_mmap_open_mode::emomWrite || mode == enum_mmap_open_mode::emomRdWrt)
    {
        status = windows_open_write();
    }

#else

#endif
    return status;
}
mmap_cur* silly_mmap::at_m(const size_t& offset)
{
    if (m_mmap && offset < m_size)
    {
        return m_mmap + offset;
    }
    return nullptr;
}
bool silly_mmap::read_m(mmap_cur* dst, const size_t& size, const size_t& offset)
{
    if (m_mmap && dst && size + offset < m_size)
    {
        memcpy(dst, m_mmap + offset, size);
        return true;
    }
    return false;
}
bool silly_mmap::write_m(mmap_cur* src, const size_t& size, const size_t& offset)
{
    bool status = false;
    if (m_mmap)
    {
        std::unique_lock lock(m_w_mutex);
        // if (!strcpy_s(m_mmap + offset, size, src))
        if (memcpy(m_mmap + offset, src, size))
        {
            m_size += size;
            status = true;
        }
    }

    return status;
}
void silly_mmap::close_m()
{
    if (m_mmap)
    {
        windows_close();
    }
}
void silly_mmap::increase()
{
}

// 最大允许映射512G, 理论上在64位机器上可以映射2^64字节（约16 EB）

// #define SILLY_MMAP_MAX_SIZE 0x8000000000

#define SILLY_MMAP_MAX_SIZE 0xFFFF
bool silly_mmap::windows_open_read()
{
    bool status = false;
#if IS_WIN32
    int fd = _open(m_file.c_str(), O_RDONLY);
    struct _stat64 stat;
    int ret = _fstati64(fd, &stat);
    m_mmap = (char*)w2l::mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    m_size = stat.st_size;
    m_tail = 0;
    /*//m_h_file = CreateFile(s2ws(m_file).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    m_h_file = CreateFile(s2ws(m_file).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_h_file == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to open file" << std::endl;
        return status;
    }

    m_h_map_file = CreateFileMapping(m_h_file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (m_h_map_file == NULL)
    {
        std::cerr << "Failed to create file mapping" << std::endl;
        CloseHandle(m_h_file);
        return status;
    }

    m_mmap = static_cast<char*>(MapViewOfFile(m_h_map_file, FILE_MAP_READ, 0, 0, 0));
    if (m_mmap == NULL)
    {
        std::cerr << "Failed to map view of file" << std::endl;
        CloseHandle(m_h_map_file);
        CloseHandle(m_h_file);
        return status;
    }

    m_size = GetFileSize(m_h_file, NULL);
    m_tail = 0;
    status = true;*/
#endif
    return status;
}
bool silly_mmap::windows_open_write()
{
    bool status = false;
#if IS_WIN32
    m_h_file = CreateFile(s2ws(m_file).c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_h_file == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to open file" << std::endl;
        return status;
    }

    uint64_t low_c = SILLY_MMAP_MAX_SIZE & 0xFFFFFFFF;

    uint64_t high_c = (SILLY_MMAP_MAX_SIZE & 0xFFFFFFFF00000000) >> 32;

    m_h_map_file = CreateFileMapping(m_h_file, NULL, PAGE_READWRITE, high_c, low_c, nullptr);
    if (m_h_map_file == NULL)
    {
        std::cerr << "Failed to create file mapping" << std::endl;
        CloseHandle(m_h_file);
        return status;
    }

    m_mmap = static_cast<char*>(MapViewOfFile(m_h_map_file, FILE_MAP_WRITE, 0, 0, SILLY_MMAP_MAX_SIZE));
    if (m_mmap == NULL)
    {
        std::cerr << "Failed to map view of file" << std::endl;
        CloseHandle(m_h_map_file);
        CloseHandle(m_h_file);
        return status;
    }
    m_size = 0;
    m_tail = m_size;
    status = true;
#endif
    return status;
}

bool silly_mmap::windows_remap()
{
    return false;
}

bool silly_mmap::linux_open_read()
{
    return false;
}
bool silly_mmap::linux_open_write()
{
    return false;
}
void silly_mmap::windows_close()
{
#if IS_WIN32
   /* SetFilePointer(m_h_file, m_size, NULL, FILE_BEGIN);
    SetEndOfFile(m_h_file);
    FlushViewOfFile(m_mmap, m_size);
    UnmapViewOfFile(m_mmap);
    m_mmap = nullptr;
    CloseHandle(m_h_map_file);
    CloseHandle(m_h_file);*/
   w2l::munmap(m_mmap, 0);
#endif
}
void silly_mmap::linux_close()
{
}
