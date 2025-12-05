//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_info.h"
TzxPyramidInfo::TzxPyramidInfo()
{
    m_head[0] = 'I';
    m_head[1] = 'I';
    m_head[2] = 'N';
    m_head[3] = 'F';
    m_version[0] = 0x01;
    m_version[1] = 0x00;
    m_version[2] = 0x00;
    m_version[3] = 0x00;
}

bool TzxPyramidInfo::open(const suPath& file, const eMMFMode& mode, const bool& usemmap)
{
    if (TzxPyramidBase::open(file, mode, usemmap))
    {
        if (m_mode == eMMFMode::Read)
        {
            read();
            close();
        }

        return true;
    }
    return false;
}

bool TzxPyramidInfo::read()
{
    size_t p = 0;
    TzxPyramidBase::read(p, m_head, sizeof(m_head));
    p += sizeof(m_head);
    TzxPyramidBase::read(p, m_version, sizeof(m_version));
    p += sizeof(m_version);
    TzxPyramidBase::read(p, m_source, sizeof(m_source));
    p += sizeof(m_source);
    TzxPyramidBase::read(p, m_projection, sizeof(m_projection));
    p += sizeof(m_projection);
    TzxPyramidBase::read(p, m_bound, sizeof(m_bound));
    p += sizeof(m_bound);
    /* if ((uint64_t*)m_version == 0x00010000)
     {
     }
     else if ((uint64_t)m_version == 0x00020000)*/
    {
        return TzxPyramidBase::read(p, m_format, SU_PYRAMID_INF_FMT_LEN);
    }
}
void TzxPyramidInfo::write_info()
{
    char buff[SU_PYRAMID_INF_TOTAL_LEN] = {0};
    size_t p = 0;
    memcpy(buff, m_head, sizeof(m_head));
    p += sizeof(m_head);
    memcpy(buff + p, m_version, sizeof(m_version));
    p += sizeof(m_version);
    memcpy(buff + p, m_source, sizeof(m_source));
    p += sizeof(m_source);
    memcpy(buff + p, m_projection, sizeof(m_projection));
    p += sizeof(m_projection);
    memcpy(buff + p, m_bound, sizeof(m_bound));
    p += sizeof(m_bound);
    /* if ((uint64_t)m_version == 0x00010000)
     {
     }
     else if ((uint64_t)m_version == 0x00020000)*/
    {
        memcpy(buff + p, m_format, sizeof(m_format));
    }
    TzxPyramidBase::write(0, buff, SU_PYRAMID_INF_TOTAL_LEN);
}

void TzxPyramidInfo::close()
{
    if (m_mode == eMMFMode::Write)
    {
        write_info();
    }
    TzxPyramidBase::close();
}

void TzxPyramidInfo::source(const std::string& src)
{
    memcpy(m_source, src.c_str(), sizeof(m_source));
}
void TzxPyramidInfo::project(const std::string& proj)
{
    memcpy(m_projection, proj.c_str(), sizeof(m_projection));
}
void TzxPyramidInfo::bound(const suRect& bd)
{
    return bound(bd.stringify());
}
void TzxPyramidInfo::bound(const std::string& bound)
{
    memcpy(m_bound, bound.data(), sizeof(m_bound));
}
void TzxPyramidInfo::format(const ePyramidTileFormat& fmt)
{
    switch (fmt)
    {
        case ePyramidTileFormat::jpeg:
            memcpy(m_format, "jpeg", 4);
            break;
        case ePyramidTileFormat::png:
            memcpy(m_format, "png", 3);
            break;
        case ePyramidTileFormat::terrain:
            memcpy(m_format, "terrain", 7);
            break;
        case ePyramidTileFormat::pbf:
            memcpy(m_format, "pbf", 3);
            break;
        default:
            throw std::invalid_argument("未支持的瓦片格式");
    }
}

void TzxPyramidInfo::format(const std::string fmt)
{
    memcpy(m_format, fmt.c_str(), fmt.size());
}

std::string TzxPyramidInfo::source()
{
    return m_source;
}
std::string TzxPyramidInfo::project()
{
    return m_projection;
}
suRect TzxPyramidInfo::bound()
{
    suRect rect;
    rect.parse(m_bound);
    return rect;
}
std::string TzxPyramidInfo::format()
{
    return m_format;
}