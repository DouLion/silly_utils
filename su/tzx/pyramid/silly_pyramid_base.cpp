//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_base.h"

using namespace silly::pyramid;
using namespace silly::file;
bool base::open(const char* file, const memory_map::access_mode& mode, const bool& usemmap)
{
    m_mode = mode;
    m_normal = !usemmap;
    if (memory_map::access_mode::Read == mode)
    {
        if (usemmap)
        {
            mmap_open(file);
        }
        else
        {
            stream_open(file, std::ios::in | std::ios::binary);
        }

        read_info();
    }
    else if (memory_map::access_mode::Write == mode)
    {
        if (usemmap)
        {
            SLOG_WARN("写入功能目前不支持内存文件映射方式(MMAP)")
        }
        stream_open(file, std::ios::out | std::ios::binary);
        // 写入头部信息,
    }

    return m_opened;
}

bool base::open(const std::string& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    return open(file.c_str(), mode, usemmap);
}
bool base::open(const std::filesystem::path& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    return open(file.string(), mode, usemmap);
}

void base::close()
{
    if (m_normal)
    {
        stream_close();
    }
    else
    {
        mmap_close();
    }
    m_opened = false;
}

bool base::read(const size_t& seek_offset, char* data, const size_t& read_size)
{
    if (m_normal)
    {
        return stream_read(seek_offset, data, read_size);
    }
    else
    {
        return mmap_read(seek_offset, data, read_size);
    }
}


bool base::write(const size_t& seek_offset, const char* data, const size_t& write_size)
{
    if (m_normal && m_stream.is_open())
    {
        stream_write(seek_offset, data, write_size);
        return true;
    }
    return false;
}
size_t base::append(const char* data, const size_t& write_size)
{
    if (m_normal && m_stream.is_open())
    {
        m_stream.seekg(0, std::ios::end);
        m_stream.write(data, write_size);
        return m_stream.tellp();
    }
    return 0;
}

void base::seek(const size_t& pos)
{
    if (m_stream)
    {
        if (m_mode == memory_map::access_mode::Write)
        {
            m_stream.seekp(pos);
        }
        else
        {
            m_stream.seekg(pos);
        }
    }
}

bool base::stream_open(const char* file, const int& mode)
{
    m_stream.open(file, mode);
    if (m_stream.is_open())
    {
        m_opened = true;
    }
    return m_opened;
}

bool base::mmap_open(const char* file)
{
    m_normal = false;
    m_opened = m_mmap.open(file, m_mode);

    return m_opened;
}

bool base::stream_read(const size_t& seek_offset, char* data, const size_t& size)
{
    std::scoped_lock lock(m_mutex);
    if (m_stream && m_opened)
    {
        seek(seek_offset);
        m_stream.read(data, size);
        return true;
    }
    return false;
}

bool base::mmap_read(const size_t& seek_offset, char* data, const size_t& size)
{
    if (m_opened)
    {
        memory_map::cur* cur = m_mmap.ptr(seek_offset + size);  // 追踪到数据尾部,防止访问越界
        if (cur)
        {
            memcpy(data, cur - size, size);
            return true;
        }
    }
    return false;
}

void base::stream_write(const size_t& seek_offset, const char* data, const size_t& size)
{
    seek(seek_offset);
    m_stream.write(data, size);
}

bool base::mmap_write(const size_t& seek_offset, const char* data, const size_t& size)
{
    return false;
}

void base::stream_close()
{
    m_stream.close();
}

void base::mmap_close()
{
    m_mmap.close();
}

bool base::read_info()
{
    if (!read(0, m_head, len::HEAD))
    {
        return false;
    }

    if (!read(len::HEAD, (char*)(&m_version), len::VER))
    {
        return false;
    }
    return true;
}

void base::write_info()
{
    write(0, m_head, len::HEAD);
    write(len::HEAD, (char*)(&m_version), len::VER);
}
size_t base::end()
{
    if (m_normal)
    {
        if (m_mode == memory_map::access_mode::Write)
        {
            return m_stream.tellp();
        }
        else
        {
            return m_stream.tellg();
        }
    }
    return 0;
}

void base::version(const char ver[4])
{
    std::scoped_lock lock(m_mutex);
    memcpy(m_version, ver, len::VER);
    write_info();
}