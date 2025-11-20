//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_base.h"

bool TzxPyramidBase::open(const std::filesystem::path& f, const eMMFMode& mode, const bool& usemmap)
{
    m_mode = mode;
    m_normal = !usemmap;
    auto file = sufile::realpath(f);
    if (eMMFMode::Read == mode)
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
    else if (eMMFMode::Write == mode)
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

void TzxPyramidBase::close()
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

bool TzxPyramidBase::read(const size_t& seek_offset, char* data, const size_t& read_size)
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

bool TzxPyramidBase::write(const size_t& seek_offset, const char* data, const size_t& write_size)
{
    if (m_normal && m_stream.is_open())
    {
        stream_write(seek_offset, data, write_size);
        return true;
    }
    return false;
}
size_t TzxPyramidBase::append(const char* data, const size_t& write_size)
{
    if (m_normal && m_stream.is_open())
    {
        m_stream.seekg(0, std::ios::end);
        m_stream.write(data, write_size);
        return m_stream.tellp();
    }
    return 0;
}

void TzxPyramidBase::seek(const size_t& pos)
{
    if (m_stream)
    {
        if (m_mode == eMMFMode::Write)
        {
            m_stream.seekp(pos);
        }
        else
        {
            m_stream.seekg(pos);
        }
    }
}

bool TzxPyramidBase::stream_open(const std::filesystem::path& file, const std::ios_base::openmode& mode)
{
    m_stream.open(file, mode);
    if (m_stream.is_open())
    {
        m_opened = true;
    }
    else
    {
        m_opened = false;
        SLOG_ERROR("open file failed:{}", file.u8string());
    }
    return m_opened;
}

bool TzxPyramidBase::mmap_open(const std::filesystem::path& file)
{
    m_normal = false;
    m_opened = m_mmap.open(file, m_mode);

    return m_opened;
}

bool TzxPyramidBase::stream_read(const size_t& seek_offset, char* data, const size_t& size)
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

bool TzxPyramidBase::mmap_read(const size_t& seek_offset, char* data, const size_t& size)
{
    if (m_opened)
    {
        suMemMapFile::cur* cur = m_mmap.ptr(seek_offset + size);  // 追踪到数据尾部,防止访问越界
        if (cur)
        {
            memcpy(data, cur - size, size);
            return true;
        }
    }
    return false;
}

void TzxPyramidBase::stream_write(const size_t& seek_offset, const char* data, const size_t& size)
{
    seek(seek_offset);
    m_stream.write(data, size);
}

bool TzxPyramidBase::mmap_write(const size_t& seek_offset, const char* data, const size_t& size)
{
    return false;
}

void TzxPyramidBase::stream_close()
{
    m_stream.close();
}

void TzxPyramidBase::mmap_close()
{
    m_mmap.close();
}

bool TzxPyramidBase::read_info()
{
    if (!read(0, m_head, SU_PYRAMID_HEAD_LEN))
    {
        return false;
    }

    if (!read(SU_PYRAMID_HEAD_LEN, (char*)(&m_version), SU_PYRAMID_VER_LEN))
    {
        return false;
    }
    return true;
}

void TzxPyramidBase::write_info()
{
    write(0, m_head, SU_PYRAMID_HEAD_LEN);
    write(SU_PYRAMID_HEAD_LEN, (char*)(&m_version), SU_PYRAMID_VER_LEN);
}
size_t TzxPyramidBase::end()
{
    if (m_normal)
    {
        if (m_mode == eMMFMode::Write)
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

void TzxPyramidBase::version(const char ver[4])
{
    std::scoped_lock lock(m_mutex);
    memcpy(m_version, ver, SU_PYRAMID_VER_LEN);
    write_info();
}