//
// Created by dell on 2023/8/14.
//

#include <geo/silly_pyramid.h>
#include "silly_pyramid_index.h"
using namespace silly::pyramid;
using namespace silly::file;
index::index()
{
    m_head[0] = 'I';
    m_head[1] = 'H';
    m_head[2] = 'D';
    m_head[3] = 'R';
    m_version[0] = 0x01;
    m_version[1] = 0x00;
    m_version[2] = 0x00;
    m_version[3] = 0x00;
}

bool index::open(const char* file, const memory_map::access_mode& mode, const bool& usemmap)
{
    if (!base::open(file, mode, usemmap))
    {
        return false;
    }

    if (mode == memory_map::access_mode::ReadOnly)
    {
        // 读取时需要从文件中接卸
        return parse();
    }
    else
    {
        // 写入时需要根据给定数据自动初始化好索引
        // return m_pack.init();
    }
    return true;
}

bool index::seek(block& blk)
{
    if (blk.zoom >= len::MAX_ZOOM)
    {
        return false;
    }
    if (!m_pack.layers[blk.zoom].in(blk.row, blk.col))
    {
        return false;
    }

    size_t offset = m_pack.layers[blk.zoom].offset(blk.row, blk.col);
    position pos;
    base::read(offset, (char*)&pos, sizeof(position));
    blk.pos = pos.offset;
    blk.size = pos.size;
    return blk.size > 0;
}

bool index::parse()
{
    size_t p = len::HEAD + len::VER;
    if (PYRAMID_MATCH_VERSION(PYRAMID_VERSION_2, m_version))
    {
        for (uint8_t l = 0; l < len::MAX_ZOOM; ++l)
        {
            std::vector<size_t> buff(5);
            read(p, (char*)&buff[0], sizeof(size_t) * 5);
            m_pack.layers[l].brow = buff[0];
            m_pack.layers[l].bcol = buff[1];
            m_pack.layers[l].erow = buff[2];
            m_pack.layers[l].ecol = buff[3];
            m_pack.layers[l].pos0 =  buff[4];
            m_pack.layers[l].fill();
            SLOG_DEBUG("Layer: {}, BR: {}, ER: {}, BC: {}, EC: {}", (int)l, (int)m_pack.layers[l].brow, (int)m_pack.layers[l].erow, (int)m_pack.layers[l].bcol, (int)m_pack.layers[l].ecol)
            p += 5 * sizeof(size_t);
        }
    }
    else if (PYRAMID_MATCH_VERSION(PYRAMID_VERSION_1, m_version) || PYRAMID_MATCH_VERSION(PYRAMID_VERSION_11, m_version))
    {
        read(p, (char*)(&m_pack.blayer), sizeof(m_pack.blayer));
        p += sizeof(m_pack.blayer);
        read(p, (char*)(&m_pack.elayer), sizeof(m_pack.elayer));
        p += sizeof(m_pack.elayer);

        for (uint8_t l = m_pack.blayer; l <= m_pack.elayer; ++l)
        {
            uint32_t buff[4];
            read(p, (char*)&buff, sizeof(uint32_t) * 4);
            m_pack.layers[l].brow = ((uint32_t*)(buff))[0];
            m_pack.layers[l].bcol = ((uint32_t*)(buff))[1];
            m_pack.layers[l].erow = ((uint32_t*)(buff))[2];
            m_pack.layers[l].ecol = ((uint32_t*)(buff))[3];

            m_pack.layers[l].fill();
            SLOG_DEBUG("Layer: {}, BR: {}, ER: {}, BC: {}, EC: {}", (int)l, (int)m_pack.layers[l].brow, (int)m_pack.layers[l].erow, (int)m_pack.layers[l].bcol, (int)m_pack.layers[l].ecol)

            p+= sizeof(uint32_t) * 4;
        }
        for (uint8_t l = m_pack.blayer; l <= m_pack.elayer; ++l)
        {
            m_pack.layers[l].pos0 = p;
            p+= m_pack.layers[l].rows * m_pack.layers[l].cols * (sizeof(position));
        }

    }
    else
    {
        SLOG_ERROR("无效的版本号")
        return false;
    }

    return true;
}

bool index::write(const block& blk)
{
    if (blk.zoom >= len::MAX_ZOOM)
    {
        return false;
    }
    if (!m_pack.layers[blk.zoom].in(blk.row, blk.col))
    {
        return false;
    }

    size_t offset = m_pack.layers[blk.zoom].offset(blk.row, blk.col);
    position pos;
    if(pos.offset < 0)
    {
        int a = 0;
    }
    pos.offset = blk.pos;
    pos.size = blk.size;
    base::write(offset, (char*)&pos, sizeof(position));

    return true;
}

bool index::close()
{
    if (m_mode == memory_map::access_mode::ReadWrite)
    {
        write();
    }

    return base::close();
}
void index::write()
{

    if(PYRAMID_MATCH_VERSION(m_version, PYRAMID_VERSION_2))
    {
        char buffer[len::IDXFIXED] = {0};
        memcpy(buffer, m_head, len::HEAD);
        memcpy(buffer + len::HEAD, m_version, len::VER);
        size_t p = len::HEAD + len::VER;
        for(uint8_t l = 0; l < len::MAX_ZOOM; ++l)
        {
            std::vector<size_t> buff;
            buff.push_back(m_pack.layers[l].brow);
            buff.push_back(m_pack.layers[l].bcol);
            buff.push_back(m_pack.layers[l].erow);
            buff.push_back(m_pack.layers[l].ecol);
            buff.push_back(m_pack.layers[l].pos0);
            memcpy(buffer+ p, (char*)&buff[0], sizeof(size_t) * 5);
            p+= sizeof(size_t) * 5;
        }
        base::write(0, buffer, len::IDXFIXED);
    }
    else if(PYRAMID_MATCH_VERSION(m_version, PYRAMID_VERSION_1) || PYRAMID_MATCH_VERSION(m_version, PYRAMID_VERSION_11))
    {
        char buffer[10240] ={0};
        memcpy(buffer, m_head, len::HEAD);
        memcpy(buffer + len::HEAD, m_version, len::VER);
        size_t p = len::HEAD + len::VER;
        memcpy(buffer+ p, &m_pack.blayer, sizeof(m_pack.blayer));
        p+= sizeof(m_pack.blayer);
        memcpy(buffer + p, &m_pack.elayer, sizeof(m_pack.elayer));
        p+= sizeof(m_pack.elayer);
        for(uint8_t i = m_pack.blayer; i <= m_pack.elayer; ++i)
        {
            uint32_t buff[4];
            buff[0] = m_pack.layers[i].brow;
            buff[1] = m_pack.layers[i].bcol;
            buff[2] = m_pack.layers[i].erow;
            buff[3] = m_pack.layers[i].ecol;
            memcpy(buffer + p, (char*)&buff, sizeof(uint32_t) * 4);
            p+= sizeof(uint32_t) * 4;
        }
        base::write(0, buffer, p);
    }

}
bool index::init()
{
    if(m_pack.init())
    {
        if(PYRAMID_MATCH_VERSION(m_version, PYRAMID_VERSION_1) || PYRAMID_MATCH_VERSION(m_version, PYRAMID_VERSION_11))
        {
            size_t pos = len::HEAD + len::VER + 2+ (m_pack.elayer - m_pack.blayer + 1) * sizeof(uint32_t) * 4;
            for(uint8_t i = m_pack.blayer; i <= m_pack.elayer; ++i)
            {
                m_pack.layers[i].pos0 = pos;
                pos += m_pack.layers[i].rows * m_pack.layers[i].cols * (sizeof(position));
            }

        }
        return true;
    }
    return false;
}

////////////////////////////////////////////////////
/// idx_pack
////////////////////////////////////////////////////
bool idx_pack::layer::in(size_t row, size_t col) const
{
    return row >= brow && row <= erow && col >= bcol && col <= ecol;
}
position idx_pack::layer::seek(size_t row, size_t col) const
{
    size_t pos = cols * (row - brow) + col - bcol;
    if (pos >= index.size())
        return {0, 0};

    return index[cols * (row - brow) + col - bcol];
}
size_t idx_pack::layer::offset(size_t row, size_t col) const
{
    size_t pos = cols * (row - brow) + col - bcol;
    return pos * sizeof(position) + pos0;
}
void idx_pack::layer::fill()
{
    rows = erow - brow + 1;
    cols = ecol - bcol + 1;
}

void lonlat2tile(double lon, double lat, int zoom, long long *x, long long *y) {
    // Place infinite and NaN coordinates off the edge of the Mercator plane

    // Place infinite and NaN coordinates off the edge of the Mercator plane
    int lat_class = fpclassify(lat);
    int lon_class = fpclassify(lon);
    bool bad_lon = false;

    if (lat_class == FP_INFINITE || lat_class == FP_NAN) {
        lat = 89.9;
    }
    if (lon_class == FP_INFINITE || lon_class == FP_NAN) {
        // Keep these far enough from the plane that they don't get
        // moved back into it by 360-degree offsetting
        lon = 720;
        bad_lon = true;
    }

    // Must limit latitude somewhere to prevent overflow.
    // 89.9 degrees latitude is 0.621 worlds beyond the edge of the flat earth,
    // hopefully far enough out that there are few expectations about the shape.
    if (lat < -89.9) {
        lat = -89.9;
    }
    if (lat > 89.9) {
        lat = 89.9;
    }

    if (lon < -360 && !bad_lon) {
        lon = -360;
    }
    if (lon > 360 && !bad_lon) {
        lon = 360;
    }

    double lat_rad = lat * M_PI / 180;
    unsigned long long n = 1LL << zoom;

    // Adjusted formula for y-coordinate calculation
    double my = log(tan(M_PI / 4 + lat_rad / 2));
    long long lly = n * (1 - (my / M_PI)) / 2;

    long long llx = n * ((lon + 180) / (360. + std::pow(2, -24)));

    // Ensure lly is within valid range [0, n)
    if (lly < 0) {
        lly = 0;
    } else if (lly >= n) {
        lly = n - 1;
    }

    *x = llx;
    *y = lly;
}
bool idx_pack::init()
{
    // 根据经纬度范围初始化索引
    size_t pos0 = len::IDXFIXED;
    for (int i = blayer; i <= elayer; ++i)
    {
        long long brow, bcol, erow, ecol;


        lonlat2tile(bound.min.x, bound.max.y, i, &bcol, &brow);
        lonlat2tile(bound.max.x, bound.min.y, i, &ecol, &erow);

        if (erow < brow || ecol < bcol)
        {
            return false;
        }
        layers[i].brow = brow;
        layers[i].bcol = bcol;
        layers[i].erow = erow;
        layers[i].ecol = ecol;
        layers[i].pos0 = pos0;
        layers[i].fill();
        SLOG_DEBUG("Layer: {}, BR: {}, ER: {}, BC: {}, EC: {}", (int)i, (int)layers[i].brow, (int)layers[i].erow, (int)layers[i].bcol, (int)layers[i].ecol)
        pos0 += layers[i].rows * layers[i].cols * sizeof(position);

    }

    return true;
}
