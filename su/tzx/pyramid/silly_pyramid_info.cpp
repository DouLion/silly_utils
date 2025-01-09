//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_info.h"

silly::pyramid::info::info()
{
    m_head[0] = 'I';
    m_head[1] = 'I';
    m_head[2] = 'N';
    m_head[3] = 'F';
    m_version[0] = 0x00;
    m_version[1] = 0x02;
    m_version[2] = 0x00;
    m_version[3] = 0x00;
}

bool silly::pyramid::info::read()
{
    size_t p = 0;
    base::read(p, m_head, len::HEAD);
    p += len::HEAD;
    base::read(p, m_version, len::VER);
    p += len::VER;
    base::read(p, m_source, len::INFO_SRC);
    p += len::INFO_SRC;
    base::read(p, m_projection, len::INFO_PROJ);
    p += len::INFO_PROJ;
    base::read(p, m_bound, len::INFO_BOUND);
    p += len::INFO_BOUND;
    /* if ((uint64_t*)m_version == 0x00010000)
     {
     }
     else if ((uint64_t)m_version == 0x00020000)*/
    {
        base::read(p, m_format, len::INFO_FMT);
    }

    return false;
}
void silly::pyramid::info::write()
{
    char buff[len::INFO_TOTAL] = {0};
    size_t p = 0;
    memcpy(buff, m_head, len::HEAD);
    p+= len::HEAD;
    memcpy(buff + p, m_version, len::VER);
    p += len::VER;
    memcpy(buff + p, m_source, len::INFO_SRC);
    p += len::INFO_SRC;
    memcpy(buff + p, m_projection, len::INFO_PROJ);
    p += len::INFO_PROJ;
    memcpy(buff + p, m_bound, len::INFO_BOUND);
    p += len::INFO_BOUND;
    /* if ((uint64_t)m_version == 0x00010000)
     {
     }
     else if ((uint64_t)m_version == 0x00020000)*/
    {
        memcpy(buff + p, m_format, len::INFO_FMT);
    }
    base::write(0, buff, len::INFO_TOTAL);
}
void silly::pyramid::info::source(const std::string& src)
{
    memcpy(m_source, src.c_str(), len::INFO_SRC);
}
void silly::pyramid::info::project(const std::string& proj)
{
    memcpy(m_projection, proj.c_str(), len::INFO_PROJ);
}
void silly::pyramid::info::bound(const std::string& bound)
{
    memcpy(m_bound, bound.data(), len::INFO_BOUND);
}
void silly::pyramid::info::format(const silly::pyramid::info::tile_format& fmt)
{
    switch (fmt)
    {
        case tile_format::jpeg:
            memcpy(m_format, "jpeg", 4);
            break;
        case tile_format::png:
            memcpy(m_format, "png", 3);
            break;
        case tile_format::terrain:
            memcpy(m_format, "terrain", 7);
            break;
        case tile_format::pbf:
            memcpy(m_format, "pbf", 3);
            break;
        default:
            throw std::runtime_error("未支持的瓦片格式");
    }
}
