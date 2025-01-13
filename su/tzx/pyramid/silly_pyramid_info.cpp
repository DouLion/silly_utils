//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_info.h"
using namespace silly::pyramid;
info::info()
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

bool info::open(const char* file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    if (base::open(file, mode, usemmap))
    {
        if (m_mode == silly::file::memory_map::access_mode::Read)
        {
            read();
            close();
        }

        return true;
    }
    return false;
}
bool info::open(const std::string& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    return open(file.c_str(), mode, usemmap);
}
bool info::open(const std::filesystem::path& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap)
{
    return open(file.string().c_str(), mode, usemmap);
}

bool info::read()
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
        return base::read(p, m_format, len::INFO_FMT);
    }
}
void info::write_info()
{
    char buff[len::INFO_TOTAL] = {0};
    size_t p = 0;
    memcpy(buff, m_head, len::HEAD);
    p += len::HEAD;
    memcpy(buff + p, m_version, len::VER);
    p += len::VER;
    memcpy(buff + p, m_source, strlen(m_source));
    p += len::INFO_SRC;
    memcpy(buff + p, m_projection, strlen(m_projection));
    p += len::INFO_PROJ;
    memcpy(buff + p, m_bound, strlen(m_bound));
    p += len::INFO_BOUND;
    /* if ((uint64_t)m_version == 0x00010000)
     {
     }
     else if ((uint64_t)m_version == 0x00020000)*/
    {
        memcpy(buff + p, m_format, strlen(m_format));
    }
    base::write(0, buff, len::INFO_TOTAL);
}

void info::close()
{
    if (m_mode == silly::file::memory_map::access_mode::Write)
    {
        write_info();
    }
    base::close();
 }

void info::source(const std::string& src)
{
    memcpy(m_source, src.c_str(), len::INFO_SRC);
}
void info::project(const std::string& proj)
{
    memcpy(m_projection, proj.c_str(), len::INFO_PROJ);
}
void info::bound(const silly_rect& bd)
{
    return bound(bd.stringify());
}
void info::bound(const std::string& bound)
{
    memcpy(m_bound, bound.data(), len::INFO_BOUND);
}
void info::format(const info::tile_format& fmt)
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
            throw std::invalid_argument("未支持的瓦片格式");
    }
}

void info::format(const std::string fmt)
{
    memcpy(m_format, fmt.c_str(), fmt.size());
}

std::string info::source()
{
    return m_source;
}
std::string info::project()
{
    return m_projection;
}
silly_rect info::bound()
{
    silly_rect rect;
    rect.destringify(m_bound);
    return rect;
}
std::string info::format()
{
    return m_format;
}