//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_base.h"
#include <cstring>

bool silly_pyramid_base::open(const char* file, const silly_mmap::open_mode& mode, const bool& usemmap)
{
    m_mode = mode;
    if (silly_mmap::open_mode::READONLY == mode)
    {
        if (usemmap)
        {
            mmap_open(file);
        }
        else
        {
            stream_open(file, "rb");
        }

        read_info();
    }
    else if (silly_mmap::open_mode::READWRITE == mode)
    {
        stream_open(file, "wb+");
    }

    return m_opened;
}

bool silly_pyramid_base::close()
{
    m_opened = false;
    if (m_normal)
    {
        stream_close();
    }
    else
    {
        mmap_close();
    }
    return true;
}

bool silly_pyramid_base::read(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    if (m_normal)
    {
        return stream_read(seek_offset, data, size, offset);
    }
    else
    {
        return mmap_read(seek_offset, data, size, offset);
    }
    return false;
}

bool silly_pyramid_base::write(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    if (m_normal)
    {
        return stream_write(seek_offset, data, size, offset);
    }
    return false;
}

void silly_pyramid_base::seek(const size_t& pos, const int& flag)
{
    if (m_stream)
    {
        fseek(m_stream, pos, SEEK_SET);
    }
}

bool silly_pyramid_base::stream_open(const char* file, const char* mode)
{
    m_stream = fopen(file, mode);
    if (m_stream)
    {
        m_opened = true;
    }
    return m_opened;
}

bool silly_pyramid_base::mmap_open(const char* file)
{
    m_normal = false;
    m_opened = m_mmap.mopen(file);

    return m_opened;
}

bool silly_pyramid_base::stream_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    std::scoped_lock lock(m_mutex);
    if (m_stream && m_opened)
    {
        seek(seek_offset);
        return fread(data + offset, size, 1, m_stream);
    }
    return false;
}

bool silly_pyramid_base::mmap_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    if (m_opened)
    {
        mmap_cur* cur = m_mmap.at(seek_offset + size);  // 追踪到数据尾部,防止访问越界
        if (cur)
        {
            memcpy(data, cur - size, size);
            return true;
        }
    }
    return false;
}

bool silly_pyramid_base::stream_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    if (m_stream && m_opened)
    {
        seek(seek_offset);
        fwrite(data + offset, size, 1, m_stream);
    }
    return false;
}

bool silly_pyramid_base::mmap_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
    return false;
}

void silly_pyramid_base::stream_close()
{
    if (m_mode != silly_mmap::open_mode::READONLY)  // 写打开时需要将信息保存回去
    {
        write_info();
    }
    if (m_stream && m_opened)
    {
        fclose(m_stream);
    }
}

void silly_pyramid_base::mmap_close()
{
    if (m_opened)
    {
        m_mmap.mclose();
    }
}

err_code silly_pyramid_base::read_info()
{
    if (!read(PYRAMID_DESC_OFFSET, m_desc, PYRAMID_DESC_LENGTH))
    {
        return err_code::UNKNOWN;
    }

    if (!read(PYRAMID_MVER_OFFSET, (char*)(&m_major_ver), PYRAMID_MVER_LENGTH))
    {
        return err_code::UNKNOWN;
    }
    if (!read(PYRAMID_PVER_OFFSET, (char*)(&m_primary_ver), PYRAMID_PVER_LENGTH))
    {
        return err_code::UNKNOWN;
    }

    return err_code::OK;
}

void silly_pyramid_base::write_info()
{
    write(0, m_desc, PYRAMID_DESC_LENGTH, 0);
    write(PYRAMID_MVER_OFFSET, (char*)(&m_major_ver), PYRAMID_MVER_LENGTH, 0);
    write(PYRAMID_PVER_OFFSET, (char*)(&m_primary_ver), PYRAMID_PVER_LENGTH, 0);
}
