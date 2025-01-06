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

mmap::mmap(void){SLOG_INFO(std::string("{:p}"), (void*)this)} mmap::~mmap(void)
{
    close();
}

bool mmap::open(const std::string& file, const int& mode, const int64_t& off)
{
    /* if (!std::filesystem::exists(file))
     {
         return false;
     }*/
    if (mode != param::ReadOnly)
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
    if (m_h_map != INVALID_HANDLE_VALUE)
        ::CloseHandle(m_h_map);
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
    // 1. 打开文件
    m_h_file = CreateFile(m_param.path.wstring().c_str(),       // 文件路径
                          GENERIC_READ,                         // 访问模式：只读
                          FILE_SHARE_READ | FILE_SHARE_DELETE,  // 共享模式：允许其他进程读取
                          NULL,                                 // 安全属性：默认
                          OPEN_EXISTING,                        // 创建方式：打开现有文件
                          FILE_ATTRIBUTE_NORMAL,                // 文件属性：普通文件
                          NULL                                  // 模板文件：无
    );

    if (m_h_file == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Failed to open file: " + m_param.path.string());
    }

    // 2. 获取文件大小
    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(m_h_file, &file_size))
    {
        CloseHandle(m_h_file);
        throw std::runtime_error("Failed to get file size");
    }

    // 3. 创建文件映射对象
    m_h_map = CreateFileMapping(m_h_file,       // 文件句柄
                                NULL,           // 安全属性：默认
                                PAGE_READONLY,  // 保护属性：只读
                                0,              // 最大对象大小的高 32 位
                                0,              // 最大对象大小的低 32 位（0 表示使用文件的实际大小）
                                NULL            // 映射对象名称：无
    );

    if (m_h_map == NULL)
    {
        CloseHandle(m_h_file);
        throw std::runtime_error("Failed to create file mapping");
    }

    // 4. 映射文件到内存
    void* mapped_data = MapViewOfFile(m_h_map,        // 映射对象句柄
                                      FILE_MAP_READ,  // 访问模式：只读
                                      0,              // 文件偏移量的高 32 位
                                      0,              // 文件偏移量的低 32 位
                                      0               // 映射区域的大小（0 表示映射整个文件）
    );

    if (mapped_data == NULL)
    {
        CloseHandle(m_h_map);
        CloseHandle(m_h_file);
        throw std::runtime_error("Failed to map view of file");
    }
    m_mmap = static_cast<char*>(mapped_data);
}
bool mmap::resize(size_t size)
{
    return false;
}
bool mmap::unmap_file()
{
#ifdef WIN32
    bool error = false;
    // 6. 清理资源
    UnmapViewOfFile(m_mmap);  // 解除内存映射
    CloseHandle(m_h_map);     // 关闭映射对象句柄
    CloseHandle(m_h_file);    // 关闭文件句柄
    clear();
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
    m_h_map = INVALID_HANDLE_VALUE;
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
    SLOG_INFO(std::string("打开 : {:p} ") + m_param.path.string(), (void*)this)
#if WIN32
    try_map_file();
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