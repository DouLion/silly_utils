/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/24 15:43
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/6/24. 实现
//

#include "silly_tzx_grid.h"
#include <files/silly_file.h>
#include <lz4.h>
/// <summary>
/// 数据段必须以这四个字符开头
/// </summary>
static constexpr char TZX_GRID_0 = '_';
static constexpr char TZX_GRID_1 = 'T';
static constexpr char TZX_GRID_2 = 'Z';
static constexpr char TZX_GRID_3 = 'X';

/// <summary>
/// 版本1 压缩grid内容
/// </summary>
static constexpr char TZX_GRID_V1 = '1';

/// 版本2 支持多组网格
static constexpr char TZX_GRID_V2 = '2';
static constexpr int TZX_GRID_VERL = 4;

#define TZX_GRID_MEMCPY_TYPE(buff, val) \
    memcpy(buff, &val, sizeof(val));    \
    buff += sizeof(val);

silly_tzx_grid::silly_tzx_grid()
{
    m_prefix[0] = TZX_GRID_V2;
    m_prefix[1] = TZX_GRID_1;
    m_prefix[2] = TZX_GRID_2;
    m_prefix[3] = TZX_GRID_3;
}

silly_tzx_grid silly_tzx_grid::copy() const
{
    silly_tzx_grid result;
    result.m_total = m_total;
    result.m_left = m_left;
    result.m_right = m_right;
    result.m_top = m_top;
    result.m_bottom = m_bottom;
    result.m_xdelta = m_xdelta;
    result.m_ydelta = m_ydelta;

    result.m_row = m_row;
    result.m_col = m_col;
    // result.m_data.resize(m_data.size());
    result.m_data.clear();
    for (const auto& m : m_data)
    {
        result.m_data.push_back(m.copy());
    }

    return result;
}
silly_tzx_grid silly_tzx_grid::copy(const size_t& i) const
{
    silly_tzx_grid result;
    result.m_total = m_total;
    result.m_left = m_left;
    result.m_right = m_right;
    result.m_top = m_top;
    result.m_bottom = m_bottom;
    result.m_xdelta = m_xdelta;
    result.m_ydelta = m_ydelta;

    result.m_row = m_row;
    result.m_col = m_col;
    if (!m_data.empty())
    {
        result.m_data.resize(1);
        size_t di = SU_MIN(i, m_data.size() - 1);
        result.m_data[i] = m_data[i].copy();
    }

    return result;
}
su::FMatrix silly_tzx_grid::frame(const size_t& i)
{
    su::FMatrix ret;

    if (i < m_data.size())
    {
        ret = m_data[i];
    }
    return ret;
}
bool silly_tzx_grid::set(const size_t& i, const silly_tzx_grid& rh)
{

    if (i < m_data.size())
    {
        if (same(rh) && !rh.m_data.empty())
        {
            m_data[i].release();
            m_data[i] = rh.m_data[0].copy();
            return true;
        }
    }
    return false;

}
bool silly_tzx_grid::add(const silly_tzx_grid& rh)
{
    if (!valid())
    {
        if (rh.valid())
        {
            m_left = rh.m_left;
            m_right = rh.m_right;
            m_top = rh.m_top;
            m_bottom = rh.m_bottom;
            m_xdelta = rh.m_xdelta;
            m_ydelta = rh.m_ydelta;
            m_row = rh.m_row;
            m_col = rh.m_col;
            m_data.push_back(rh.m_data[0]);
            return true;
        }
    }
    else if (same(rh))
    {
        m_data.push_back(rh.m_data[0]);
        return true;
    }
    return false;
}
silly_rect silly_tzx_grid::rect() const
{
    silly_rect ret;
    ret.min.x = m_left;
    ret.min.y = m_top;
    ret.max.x = m_right;
    ret.max.y = m_bottom;
    return ret;
}
void silly_tzx_grid::rect(const silly_rect& boundary)
{
    m_left = boundary.min.x;
    m_right = boundary.max.x;
    m_top = boundary.min.y;
    m_bottom = boundary.max.y;
}

bool silly_tzx_grid::read(const std::filesystem::path& file)
{
    bool status = false;
#ifndef NDEBUG
    //  std::cout << "读取TZX_GRID失败 : " << path << std::endl;
#endif
    release();

    if (const size_t fileSize = sufile::size(file))
    {
        const std::string buff = sufile::read(file);
        status = unserialize(buff);
    }
    return status;
}

bool silly_tzx_grid::save(const std::filesystem::path& file)
{
    bool status = false;
    std::string buff;
    if (serialize_v2(buff))
    {
        return sufile::write(file, buff) > 0;
    }
    return status;
}

bool silly_tzx_grid::serialize(std::string& buff)
{
    bool status = false;
    buff.clear();
    if (m_data.empty() || !m_data[0].row() || !m_data[0].col())  // 以网格点数据为主
    {
        return status;
    }
    m_total = 0;
    m_total += (sizeof(m_prefix) + sizeof(m_total) + sizeof(m_left) * 4 + sizeof(m_xdelta) * 2);
    m_total += (sizeof(m_name) + sizeof(m_units) + sizeof(m_row) + sizeof(m_col));
    m_total += sizeof(float) * m_data[0].row() * m_data[0].col();

    buff.resize(m_total);
    char* p = buff.data();

    memcpy(p, m_prefix, sizeof(m_prefix));
    p += sizeof(m_prefix);
    memcpy(p, &m_total, sizeof(m_total));
    p += sizeof(m_total);

    memcpy(p, &m_left, sizeof(m_left));
    p += sizeof(m_left);
    memcpy(p, &m_top, sizeof(m_top));
    p += sizeof(m_top);
    memcpy(p, &m_right, sizeof(m_right));
    p += sizeof(m_right);
    memcpy(p, &m_bottom, sizeof(m_bottom));
    p += sizeof(m_bottom);

    memcpy(p, &m_xdelta, sizeof(m_xdelta));
    p += sizeof(m_xdelta);
    memcpy(p, &m_ydelta, sizeof(m_ydelta));
    p += sizeof(m_ydelta);

    memcpy(p, m_name, sizeof(m_name));
    p += sizeof(m_name);
    memcpy(p, m_units, sizeof(m_units));
    p += sizeof(m_units);

    memcpy(p, &m_row, sizeof(m_row));
    p += sizeof(m_row);
    memcpy(p, &m_col, sizeof(m_col));
    p += sizeof(m_col);

    memcpy(p, m_data[0].data(), m_row * m_col * sizeof(float));
    status = true;

    return status;
}

bool silly_tzx_grid::serialize_v1(std::string& buff)
{
    bool status = false;
    buff.clear();
    if (!m_data[0].row() || !m_data[0].col())  // 以网格点数据为主
    {
        return status;
    }
    // 先计算压缩数据大小
    std::string srcBin;
    size_t srcLen = m_data[0].row() * m_data[0].col() * sizeof(float);
    srcBin.assign(reinterpret_cast<char*>(m_data[0].data()), srcLen);
    size_t cpsLen = 0;
    std::string cpsBin;
    if (!lz4_cps_data(srcBin, cpsBin))
    {
        cpsBin.clear();
        return false;
    }
    cpsLen = cpsBin.size();
    m_total = sizeof(m_prefix) + sizeof(m_total) + sizeof(m_left) * 4 + sizeof(m_xdelta) * 2 + sizeof(m_name) + sizeof(m_units) + sizeof(m_row) + sizeof(m_col) + sizeof(cpsLen) + cpsLen;
    buff.resize(m_total);
    char* p = buff.data();

    memcpy(p, m_prefix, sizeof(m_prefix));
    p += sizeof(m_prefix);

    TZX_GRID_MEMCPY_TYPE(p, m_total)

    TZX_GRID_MEMCPY_TYPE(p, m_left)

    TZX_GRID_MEMCPY_TYPE(p, m_top)

    TZX_GRID_MEMCPY_TYPE(p, m_right)

    TZX_GRID_MEMCPY_TYPE(p, m_bottom)

    TZX_GRID_MEMCPY_TYPE(p, m_xdelta)

    TZX_GRID_MEMCPY_TYPE(p, m_ydelta)

    memcpy(p, m_name, sizeof(m_name));
    p += sizeof(m_name);
    memcpy(p, m_units, sizeof(m_units));
    p += sizeof(m_units);

    TZX_GRID_MEMCPY_TYPE(p, m_row)

    TZX_GRID_MEMCPY_TYPE(p, m_col)

    TZX_GRID_MEMCPY_TYPE(p, m_total)

    memcpy(p, cpsBin.data(), cpsLen);
    status = true;
    return status;
}
bool silly_tzx_grid::serialize_v2(std::string& buff)
{
    if (m_data.empty())
    {
        return false;
    }
    int frameBuffLen = m_row * m_col * sizeof(float);
    m_buff.resize(m_data.size());
    m_total = 0;
    for (int i = 0; i < m_data.size(); i++)
    {
        std::string tmpBuff(frameBuffLen, 0);
        std::memcpy(tmpBuff.data(), m_data[i].data(), frameBuffLen);
        std::string tmpCpsBin;
        if (!lz4_cps_data(tmpBuff, m_buff[i]))
        {
            m_buff[i].clear();
        }
        m_total += sizeof(size_t);
        m_total += m_buff[i].size();
    }

    m_total += (sizeof(m_prefix) + sizeof(m_total) + sizeof(m_left) * 4 + sizeof(m_xdelta) * 2);
    m_total += (sizeof(m_name) + sizeof(m_units) + sizeof(m_row) + sizeof(m_col));

    buff.resize(m_total);
    char* p = buff.data();

    memcpy(p, m_prefix, sizeof(m_prefix));
    p += sizeof(m_prefix);
    memcpy(p, &m_total, sizeof(m_total));
    p += sizeof(m_total);

    memcpy(p, &m_left, sizeof(m_left));
    p += sizeof(m_left);
    memcpy(p, &m_top, sizeof(m_top));
    p += sizeof(m_top);
    memcpy(p, &m_right, sizeof(m_right));
    p += sizeof(m_right);
    memcpy(p, &m_bottom, sizeof(m_bottom));
    p += sizeof(m_bottom);

    memcpy(p, &m_xdelta, sizeof(m_xdelta));
    p += sizeof(m_xdelta);
    memcpy(p, &m_ydelta, sizeof(m_ydelta));
    p += sizeof(m_ydelta);

    memcpy(p, m_name, sizeof(m_name));
    p += sizeof(m_name);
    memcpy(p, m_units, sizeof(m_units));
    p += sizeof(m_units);

    memcpy(p, &m_row, sizeof(m_row));
    p += sizeof(m_row);
    memcpy(p, &m_col, sizeof(m_col));
    p += sizeof(m_col);

    for (auto& tmpB : m_buff)
    {
        size_t tmpL = tmpB.size();
        TZX_GRID_MEMCPY_TYPE(p, tmpL);
        memcpy(p, tmpB.data(), tmpL);
        p += tmpL;
    }

    return true;
}

bool silly_tzx_grid::unserialize(const std::string& buff)
{
    bool status = false;

    if (!(TZX_GRID_1 == buff[1] && TZX_GRID_2 == buff[2] && TZX_GRID_3 == buff[3]))
    {
        return status;
    }
    if (TZX_GRID_V1 == buff[0])
    {
        return unserialize_v1(buff);
    }
    if (TZX_GRID_V2 == buff[0])
    {
        return unserialize_v2(buff);
    }
    if (TZX_GRID_0 != buff[0])
    {
        return false;
    }

    char* p = const_cast<char*>(buff.data()) + TZX_GRID_VERL;
    m_total = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    if (m_total != buff.size())
    {
        return false;
    }
    m_left = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);
    m_top = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);
    m_right = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);
    m_bottom = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);

    m_xdelta = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);
    m_ydelta = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);

    memcpy(m_name, p, sizeof(m_name));
    p += sizeof(m_name);
    memcpy(m_units, p, sizeof(m_units));
    p += sizeof(m_units);

    m_row = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    m_col = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    m_data.resize(1);
    m_data[0].create(m_row, m_col, true);
    memcpy(m_data[0].data(), p, m_row * m_col * sizeof(float));

    status = true;
    return status;
}

bool silly_tzx_grid::unserialize_v1(const std::string& buff)
{
    char* p = const_cast<char*>(buff.data()) + TZX_GRID_VERL;
    size_t len = buff.size();
    m_total = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    if (m_total != len)
    {
        return false;
    }

    m_left = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);
    m_top = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);

    m_right = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);

    m_bottom = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);

    m_xdelta = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);
    m_ydelta = reinterpret_cast<float*>(p)[0];
    p += sizeof(float);

    memcpy(m_name, p, sizeof(m_name));
    p += sizeof(m_name);
    memcpy(m_units, p, sizeof(m_units));
    p += sizeof(m_units);

    m_row = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    m_col = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    size_t cpsLen = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);

    std::string cpsBin;
    cpsBin.assign(p, cpsLen);
    std::string dCpsBin;
    bool status = lz4_dcps_data(cpsBin, dCpsBin);
    size_t dCpsLen = dCpsBin.size();
    if (status && !dCpsBin.empty() && dCpsLen == m_row * m_col * sizeof(float))
    {
        m_data.resize(1);
        m_data[0].create(m_row, m_col, true);
        memcpy(m_data[0].data(), dCpsBin.data(), m_row * m_col * sizeof(float));
        return true;
    }

    return false;
}
bool silly_tzx_grid::unserialize_v2(const std::string& buff)
{
    char* p = const_cast<char*>(buff.data()) + TZX_GRID_VERL;
    size_t len = buff.size();
    m_total = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    if (m_total != len)
    {
        return false;
    }
    size_t currLen = TZX_GRID_VERL + sizeof(m_total);

    m_left = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_left);
    currLen += sizeof(m_left);

    m_top = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_top);
    currLen += sizeof(m_top);

    m_right = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_right);
    currLen += sizeof(m_right);

    m_bottom = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_bottom);
    currLen += sizeof(m_bottom);

    m_xdelta = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_xdelta);
    currLen += sizeof(m_xdelta);

    m_ydelta = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_ydelta);
    currLen += sizeof(m_ydelta);

    memcpy(m_name, p, sizeof(m_name));
    p += sizeof(m_name);
    currLen += sizeof(m_name);

    memcpy(m_units, p, sizeof(m_units));
    p += sizeof(m_units);
    currLen += sizeof(m_units);

    m_row = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    currLen += sizeof(m_row);

    m_col = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);
    currLen += sizeof(m_col);
    m_data.clear();
    while (currLen < m_total)
    {
        size_t cpsLen = reinterpret_cast<size_t*>(p)[0];
        p += sizeof(size_t);
        currLen += sizeof(cpsLen) + cpsLen;

        std::string cpsBin;
        cpsBin.assign(p, cpsLen);
        std::string dCpsBin;
        bool status = lz4_dcps_data(cpsBin, dCpsBin);
        size_t dCpsLen = dCpsBin.size();
        su::FMatrix tmpMatrix;
        if (status && !dCpsBin.empty() && dCpsLen == m_row * m_col * sizeof(float))
        {
            tmpMatrix.create(m_row, m_col, true);
            memcpy(tmpMatrix.data(), dCpsBin.data(), m_row * m_col * sizeof(float));
        }
        m_data.push_back(tmpMatrix);
        p += cpsLen;
    }

    return !m_data.empty();
}

void silly_tzx_grid::puzzle(const std::vector<silly_tzx_grid>& grids, const silly_rect& boundary, const float& delta)
{
    if (grids.empty())
    {
        return;
    }
    m_left = boundary.min.x;
    m_top = boundary.max.y;
    m_right = boundary.max.x;
    m_bottom = boundary.min.y;
    m_row = std::round((m_top - m_bottom) / m_ydelta);
    m_col = std::round((m_right - m_left) / m_xdelta);
    m_data.resize(1);
    m_data[0].create(m_row, m_col);
    for (int r = 0; r < m_row; r++)
    {
        for (int c = 0; c < m_col; c++)
        {
            float x = m_left + c * m_xdelta;
            float y = m_top - r * m_ydelta;
            // 使用两个值之间的最大值
            float value = -999999.0;  // invalid data

            for (auto& grid : grids)
            {
                if (std::abs(grid.m_xdelta - delta) > 0.000001)
                {
                    continue;
                }

                int tx = std::round((x - grid.m_left) / grid.m_xdelta);
                int ty = std::round((grid.m_top - y) / grid.m_ydelta);
                if (tx >= 0 && tx < grid.m_col && ty >= 0 && ty < grid.m_row)
                {
                    value = SU_MAX(value, grid.m_data[0][ty][tx]);
                }
            }
            m_data[0][r][c] = value;
        }
    }
}
silly_tzx_grid& silly_tzx_grid::operator=(const silly_tzx_grid& rh)
{
    if (this != &rh)
    {
        this->m_total = rh.m_total;
        this->m_left = rh.m_left;
        this->m_right = rh.m_right;
        this->m_top = rh.m_top;
        this->m_bottom = rh.m_bottom;
        this->m_xdelta = rh.m_xdelta;
        this->m_ydelta = rh.m_ydelta;

        this->m_row = rh.m_row;
        this->m_col = rh.m_col;
        this->m_data = rh.m_data;
    }

    return *this;
}
bool silly_tzx_grid::same(const silly_tzx_grid& rh) const
{
    bool ret = true;
    ret &= (this->m_col > 0 && rh.m_row > 0);
    ret &= (this->m_left == rh.m_left);
    ret &= (this->m_right == rh.m_right);
    ret &= (this->m_top == rh.m_top);
    ret &= (this->m_bottom == rh.m_bottom);
    ret &= (this->m_xdelta == rh.m_xdelta);
    ret &= (this->m_ydelta == rh.m_ydelta);

    ret &= (this->m_row == rh.m_row);
    ret &= (this->m_col == rh.m_col);
    return ret;
}
bool silly_tzx_grid::valid() const
{
    if (m_data.empty())
    {
        return false;
    }
    if (m_col == 0 || m_row == 0)
    {
        return false;
    }
    return true;

}

bool silly_tzx_grid::lz4_cps_data(const std::string& src, std::string& dst) const
{
    if (!src.empty())
    {
        int tryLen =  m_row * m_col * sizeof(float) * 1.5;
        dst.resize(tryLen);
        // // int realLen = -2; （如 -1为数据损坏，-2为缓冲区不足
        int realLen = LZ4_compress_default(src.data(), dst.data(), static_cast<int>(src.size()), tryLen);
        if (realLen > 0)
        {
            dst.resize(realLen);
            return true;
        }
    }
    dst.clear();
    return false;
}

bool silly_tzx_grid::lz4_dcps_data(const std::string& src, std::string& dst) const
{
    if (!src.empty())
    {
        int tryLen = m_row * m_col * sizeof(float) * 1.5;
        dst.resize(tryLen);
        // // int realLen = -2; （如 -1为数据损坏，-2为缓冲区不足
        int realLen = LZ4_decompress_safe(src.data(), dst.data(), static_cast<int>(src.size()), tryLen);
        if (realLen > 0)
        {
            dst.resize(realLen);
            return true;
        }
    }
    dst.clear();
    return false;
}

void silly_tzx_grid::release()
{
    for (auto& m : m_data)
    {
        m.release();
    }
    m_data.clear();
}
