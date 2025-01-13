//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_base.h"

using namespace silly::pyramid;
using namespace silly::file;
bool base::open(const char* file, const memory_map::access_mode& mode, const bool& usemmap)
{
    m_mode = mode;
    if (memory_map::access_mode::ReadOnly == mode)
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
    else if (memory_map::access_mode::ReadWrite == mode)
    {
        stream_open(file, std::ios::out | std::ios::binary);
    }

    return m_opened;
}

bool base::close()
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
    return true;
}

bool base::read(size_t seek_offset, char* data, const size_t& read_size, const size_t& offset_in_data)
{
    if (m_normal)
    {
        return stream_read(seek_offset, data, read_size, offset_in_data);
    }
    else
    {
        return mmap_read(seek_offset, data, read_size, offset_in_data);
    }
}

bool base::write(size_t seek_offset, char* data, const size_t& write_size, const size_t& offset_in_data)
{
    if (m_normal)
    {
        return stream_write(seek_offset, data, write_size, offset_in_data);
    }
    return false;
}

void base::seek(const size_t& pos, const int& flag)
{
    if (m_stream)
    {
        if(m_mode == memory_map::access_mode::ReadWrite)
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
        m_normal = true;
    }
    else
    {
        m_opened = false;
        SLOG_ERROR("open file failed:{}", file);
    }
    return m_opened;
}

bool base::mmap_open(const char* file)
{
    m_normal = false;
    m_opened = m_mmap.open(file, m_mode);

    return m_opened;
}

bool base::stream_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    std::scoped_lock lock(m_mutex);
    if (m_stream && m_opened)
    {
        seek(seek_offset);
        m_stream.read(data + offset, size);
        return true;
    }
    return false;
}

bool base::mmap_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    if (m_opened)
    {
        memory_map::cur* cur = m_mmap.at(seek_offset + size);  // 追踪到数据尾部,防止访问越界
        if (cur)
        {
            memcpy(data, cur - size, size);
            return true;
        }
    }
    return false;
}

bool base::stream_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    if (m_stream && m_opened)
    {
        seek(seek_offset);
        m_stream.write(data + offset, size);
    }
    return false;
}

bool base::mmap_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    return false;
}

void base::stream_close()
{
    if (m_mode == memory_map::access_mode::ReadWrite)  // 写打开时需要将信息保存回去
    {
        write();
    }
    if (m_stream && m_opened)
    {
        m_stream.close();
    }
}

void base::mmap_close()
{
    //if (m_opened)
    {
        m_mmap.close();
    }
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
    write(0, m_head, len::HEAD, 0);
    write(len::HEAD, (char*)(&m_version), len::VER, 0);
}
void base::write()
{
    write_info();
}
size_t base::end()
{
    if (m_normal)
    {
        if(m_mode == memory_map::access_mode::ReadWrite)
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
