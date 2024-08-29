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
    status = true;
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
    SetFilePointer(m_h_file, m_size, NULL, FILE_BEGIN);
    SetEndOfFile(m_h_file);
    FlushViewOfFile(m_mmap, m_size);
    UnmapViewOfFile(m_mmap);
    m_mmap = nullptr;
    CloseHandle(m_h_map_file);
    CloseHandle(m_h_file);
#endif
}
void silly_mmap::linux_close()
{
}
