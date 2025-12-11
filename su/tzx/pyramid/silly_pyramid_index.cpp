//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_index.h"

TzxPyramidIndex::TzxPyramidIndex()
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

bool TzxPyramidIndex::open(const suPath& file, const eMMFMode& mode, const bool& usemmap)
{
    if (!TzxPyramidBase::open(file, mode, usemmap))
    {
        return false;
    }

    if (mode == eMMFMode::Read)
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

bool TzxPyramidIndex::read(TzxPyramidBlock& blk)
{
    if (blk.zoom >= SU_PYRAMID_MAX_ZOOM)
    {
        return false;
    }
    if (!m_pack.layers[blk.zoom].in(blk.row, blk.col))
    {
        return false;
    }

    size_t offset = m_pack.layers[blk.zoom].offset(blk.row, blk.col);
    BlockPos pos;
    TzxPyramidBase::read(offset, (char*)&pos, sizeof(BlockPos));
    blk.pos = pos.offset;
    blk.size = pos.size;
    return blk.size > 0;
}

bool TzxPyramidIndex::parse()
{
    size_t p = SU_PYRAMID_HEAD_LEN + SU_PYRAMID_VER_LEN;
    if (PYRAMID_MATCH_VERSION(SU_PYRAMID_VERSION_2, m_version))
    {
        bool find_beg = true;
        for (uint8_t l = 0; l <= SU_PYRAMID_MAX_ZOOM; ++l)
        {
            std::vector<size_t> buff(5);
            TzxPyramidBase::read(p, (char*)&buff[0], sizeof(size_t) * 5);
            m_pack.layers[l].brow = buff[0];
            m_pack.layers[l].bcol = buff[1];
            m_pack.layers[l].erow = buff[2];
            m_pack.layers[l].ecol = buff[3];
            m_pack.layers[l].pos0 = buff[4];
            m_pack.layers[l].fill();
            if (find_beg)
            {  //
                if (m_pack.layers[l].rows != 0 && m_pack.layers[l].cols != 0)
                {
                    m_pack.blayer = l;
                    find_beg = false;
                }
            }
            else
            {
                if (m_pack.layers[l].rows < m_pack.layers[l - 1].rows && m_pack.layers[l].cols < m_pack.layers[l - 1].cols)
                {
                    m_pack.elayer = l;
                    break;
                }
            }
            SLOG_DEBUG("Layer: {}, BR: {}, ER: {}, BC: {}, EC: {}", (int)l, (int)m_pack.layers[l].brow, (int)m_pack.layers[l].erow, (int)m_pack.layers[l].bcol, (int)m_pack.layers[l].ecol)
            p += 5 * sizeof(size_t);
        }
        SLOG_DEBUG("\n起始层级: {}\n结束层级:{}", (int)m_pack.blayer, (int)m_pack.elayer)
    }
    else if (PYRAMID_MATCH_VERSION(SU_PYRAMID_VERSION_1, m_version) || PYRAMID_MATCH_VERSION(SU_PYRAMID_VERSION_11, m_version))
    {
        TzxPyramidBase::read(p, (char*)(&m_pack.blayer), sizeof(m_pack.blayer));
        p += sizeof(m_pack.blayer);
        TzxPyramidBase::read(p, (char*)(&m_pack.elayer), sizeof(m_pack.elayer));
        p += sizeof(m_pack.elayer);

        for (uint8_t l = m_pack.blayer; l <= m_pack.elayer; ++l)
        {
            uint32_t buff[4];
            TzxPyramidBase::read(p, (char*)&buff, sizeof(uint32_t) * 4);
            m_pack.layers[l].brow = buff[0];
            m_pack.layers[l].bcol = buff[1];
            m_pack.layers[l].erow = buff[2];
            m_pack.layers[l].ecol = buff[3];

            m_pack.layers[l].fill();
            SLOG_DEBUG("Layer: {}, BR: {}, ER: {}, BC: {}, EC: {}", (int)l, (int)m_pack.layers[l].brow, (int)m_pack.layers[l].erow, (int)m_pack.layers[l].bcol, (int)m_pack.layers[l].ecol)

            p += sizeof(uint32_t) * 4;
        }
        for (uint8_t l = m_pack.blayer; l <= m_pack.elayer; ++l)
        {
            m_pack.layers[l].pos0 = p;
            p += m_pack.layers[l].rows * m_pack.layers[l].cols * (sizeof(BlockPos));
        }
    }
    else
    {
        SLOG_ERROR("无效的版本号")
        return false;
    }

    return true;
}

bool TzxPyramidIndex::write(const TzxPyramidBlock& blk)
{
    if (blk.zoom >= SU_PYRAMID_MAX_ZOOM)
    {
        return false;
    }
    if (!m_pack.layers[blk.zoom].in(blk.row, blk.col))
    {
        return false;
    }

    size_t offset = m_pack.layers[blk.zoom].offset(blk.row, blk.col);
    BlockPos pos;
    pos.offset = blk.pos;
    pos.size = blk.size;
    TzxPyramidBase::write(offset, (char*)&pos, sizeof(BlockPos));

    return true;
}

void TzxPyramidIndex::close()
{
    if (m_mode == eMMFMode::Write)
    {
        write_info();
    }
    TzxPyramidBase::close();
}
void TzxPyramidIndex::write_info()
{
    if (PYRAMID_MATCH_VERSION(m_version, SU_PYRAMID_VERSION_2))
    {
        char buffer[SU_PYRAMID_IDX_FIXED] = {0};
        memcpy(buffer, m_head, SU_PYRAMID_HEAD_LEN);
        memcpy(buffer + SU_PYRAMID_HEAD_LEN, m_version, SU_PYRAMID_VER_LEN);
        size_t p = SU_PYRAMID_HEAD_LEN + SU_PYRAMID_VER_LEN;
        for (uint8_t i = m_pack.blayer; i <= m_pack.elayer; ++i)
        {
            std::vector<size_t> buff;
            buff.push_back(m_pack.layers[i].brow);
            buff.push_back(m_pack.layers[i].bcol);
            buff.push_back(m_pack.layers[i].erow);
            buff.push_back(m_pack.layers[i].ecol);
            buff.push_back(m_pack.layers[i].pos0);
            memcpy(buffer + p, (char*)&buff[0], sizeof(size_t) * 5);
            p += sizeof(size_t) * 5;
        }
        TzxPyramidBase::write(0, buffer, SU_PYRAMID_IDX_FIXED);
    }
    else if (PYRAMID_MATCH_VERSION(m_version, SU_PYRAMID_VERSION_1) || PYRAMID_MATCH_VERSION(m_version, SU_PYRAMID_VERSION_11))
    {
        char buffer[10240] = {0};
        memcpy(buffer, m_head, SU_PYRAMID_HEAD_LEN);
        memcpy(buffer + SU_PYRAMID_HEAD_LEN, m_version, SU_PYRAMID_VER_LEN);
        size_t p = SU_PYRAMID_HEAD_LEN + SU_PYRAMID_VER_LEN;
        memcpy(buffer + p, &m_pack.blayer, sizeof(m_pack.blayer));
        p += sizeof(m_pack.blayer);
        memcpy(buffer + p, &m_pack.elayer, sizeof(m_pack.elayer));
        p += sizeof(m_pack.elayer);
        for (uint8_t i = m_pack.blayer; i <= m_pack.elayer; ++i)
        {
            uint32_t buff[4];
            buff[0] = m_pack.layers[i].brow;
            buff[1] = m_pack.layers[i].bcol;
            buff[2] = m_pack.layers[i].erow;
            buff[3] = m_pack.layers[i].ecol;
            memcpy(buffer + p, (char*)&buff, sizeof(uint32_t) * 4);
            p += sizeof(uint32_t) * 4;
        }
        TzxPyramidBase::write(0, buffer, p);
    }
}
bool TzxPyramidIndex::build()
{
    if (m_pack.init())
    {
        if (PYRAMID_MATCH_VERSION(m_version, SU_PYRAMID_VERSION_1) || PYRAMID_MATCH_VERSION(m_version, SU_PYRAMID_VERSION_11))
        {
            size_t pos = SU_PYRAMID_HEAD_LEN + SU_PYRAMID_VER_LEN + 2 + (m_pack.elayer - m_pack.blayer + 1) * sizeof(uint32_t) * 4;
            for (uint8_t i = m_pack.blayer; i <= m_pack.elayer; ++i)
            {
                m_pack.layers[i].pos0 = pos;
                pos += m_pack.layers[i].rows * m_pack.layers[i].cols * (sizeof(BlockPos));
            }
        }
        return true;
    }
    return false;
}

////////////////////////////////////////////////////
/// idx_pack
////////////////////////////////////////////////////
bool TzxPyramidIndex::Layer::in(size_t row, size_t col) const
{
    return row >= brow && row <= erow && col >= bcol && col <= ecol;
}
TzxPyramidIndex::BlockPos TzxPyramidIndex::Layer::seek(size_t row, size_t col) const
{
    size_t pos = cols * (row - brow) + col - bcol;
    if (pos >= index.size())
        return {0, 0};

    return index[cols * (row - brow) + col - bcol];
}
size_t TzxPyramidIndex::Layer::offset(size_t row, size_t col) const
{
    size_t pos = cols * (row - brow) + col - bcol;
    return pos * sizeof(BlockPos) + pos0;
}
void TzxPyramidIndex::Layer::fill()
{
    rows = erow - brow + 1;
    cols = ecol - bcol + 1;
}

void lonlat2tile(double lon, double lat, int zoom, long long* x, long long* y)
{
    // Place infinite and NaN coordinates off the edge of the Mercator plane

    // Place infinite and NaN coordinates off the edge of the Mercator plane
    int lat_class = std::fpclassify(lat);
    int lon_class = std::fpclassify(lon);
    bool bad_lon = false;

    if (lat_class == FP_INFINITE || lat_class == FP_NAN)
    {
        lat = 89.9;
    }
    if (lon_class == FP_INFINITE || lon_class == FP_NAN)
    {
        // Keep these far enough from the plane that they don't get
        // moved back into it by 360-degree offsetting
        lon = 720;
        bad_lon = true;
    }

    // Must limit latitude somewhere to prevent overflow.
    // 89.9 degrees latitude is 0.621 worlds beyond the edge of the flat earth,
    // hopefully far enough out that there are few expectations about the shape.
    if (lat < -89.9)
    {
        lat = -89.9;
    }
    if (lat > 89.9)
    {
        lat = 89.9;
    }

    if (lon < -360 && !bad_lon)
    {
        lon = -360;
    }
    if (lon > 360 && !bad_lon)
    {
        lon = 360;
    }

    double lat_rad = lat * MATH::PI_ / 180;
    unsigned long long n = 1LL << zoom;

    // Adjusted formula for y-coordinate calculation
    double my = log(tan(MATH::PI_ / 4 + lat_rad / 2));
    long long lly = n * (1 - (my / MATH::PI_)) / 2;

    long long llx = n * ((lon + 180) / (360. + std::pow(2, -24)));

    // Ensure lly is within valid range [0, n)
    if (lly < 0)
    {
        lly = 0;
    }
    else if (lly >= n)
    {
        lly = n - 1;
    }

    *x = llx;
    *y = lly;
}
bool TzxPyramidIndex::Pack::init()
{
    // 根据经纬度范围初始化索引
    size_t pos0 = SU_PYRAMID_IDX_FIXED;
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
        pos0 += layers[i].rows * layers[i].cols * sizeof(BlockPos);
    }

    return true;
}

size_t TzxPyramidIndex::brow(const uint8_t& layer) const
{
    return m_pack.layers[layer].brow;
}
size_t TzxPyramidIndex::bcol(const uint8_t& layer) const
{
    return m_pack.layers[layer].bcol;
}
size_t TzxPyramidIndex::erow(const uint8_t& layer) const
{
    return m_pack.layers[layer].erow;
}
size_t TzxPyramidIndex::ecol(const uint8_t& layer) const
{
    return m_pack.layers[layer].ecol;
}
size_t TzxPyramidIndex::rows(const uint8_t& layer) const
{
    return m_pack.layers[layer].rows;
}
size_t TzxPyramidIndex::cols(const uint8_t& layer) const
{
    return m_pack.layers[layer].cols;
}

suRect TzxPyramidIndex::bound() const
{
    return m_pack.bound;
}

uint8_t TzxPyramidIndex::beg_layer() const
{
    return m_pack.blayer;
}

uint8_t TzxPyramidIndex::end_layer() const
{
    return m_pack.elayer;
}

void TzxPyramidIndex::beg_layer(const uint8_t& beg) noexcept
{
    m_pack.blayer = beg;
}

void TzxPyramidIndex::end_layer(const uint8_t& end) noexcept
{
    m_pack.elayer = end;
}

void TzxPyramidIndex::bound(const suRect& rect)
{
    m_pack.bound = rect;
}