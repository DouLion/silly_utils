/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/24 15:43
 * @version: 1.0.1
 * @description:
 */

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

#define TZX_GRID_MEMCPY_TYPE(buff, val) \
    memcpy(buff, &val, sizeof(val));    \
    buff += sizeof(val);

TzxGrid::TzxGrid()
{
    m_prefix[0] = TZX_GRID_V2;
    m_prefix[1] = TZX_GRID_1;
    m_prefix[2] = TZX_GRID_2;
    m_prefix[3] = TZX_GRID_3;
    m_header_len = 0;
    m_header_len += (sizeof(m_prefix) + sizeof(m_total) + sizeof(m_left) * 4 + sizeof(m_xdelta) * 2);
    m_header_len += (sizeof(m_name) + sizeof(m_units) + sizeof(m_rows) + sizeof(m_cols));
}

silly_tzx_grid TzxGrid::copy() const
{
    silly_tzx_grid ret;
    ret.copy_info(*this);
    for (const auto& m : m_frames)
    {
        ret.m_frames.push_back(m.copy());
    }

    return ret;
}

silly_tzx_grid TzxGrid::copy(const size_t& i) const
{
    silly_tzx_grid ret;
    ret.copy_info(*this);
    if (!m_frames.empty())
    {
        const size_t di = SU_MIN(i, m_frames.size() - 1);
        ret.m_frames.push_back(ret.m_frames[di].copy());
    }

    return ret;
}

void TzxGrid::copy_info(const silly_tzx_grid& rh)
{
    m_left = rh.m_left;
    m_right = rh.m_right;
    m_top = rh.m_top;
    m_bottom = rh.m_bottom;
    m_xdelta = rh.m_xdelta;
    m_ydelta = rh.m_ydelta;

    m_rows = rh.m_rows;
    m_cols = rh.m_cols;
}

suFMatrix& TzxGrid::frame(const size_t& i)
{
    static suFMatrix ret;
    if (i < m_frames.size())
    {
        return m_frames[i];
    }
    return ret;
}
suFMatrix TzxGrid::frame(const size_t& i) const
{
    static suFMatrix ret;
    if (i < m_frames.size())
    {
        return m_frames[i];
    }
    return ret;
}

size_t TzxGrid::frame_num() const
{
    return m_frames.size();
}

bool TzxGrid::set(const size_t& i, const silly_tzx_grid& rh)
{
    if (i < m_frames.size())
    {
        if (same(rh) && !rh.m_frames.empty())
        {
            m_frames[i].release();
            m_frames[i] = rh.m_frames[0].copy();
            return true;
        }
    }
    return false;
}
bool TzxGrid::add(const silly_tzx_grid& rh)
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
            m_rows = rh.m_rows;
            m_cols = rh.m_cols;
            m_frames.push_back(rh.m_frames[0]);
            return true;
        }
    }
    else if (same(rh))
    {
        m_frames.push_back(rh.m_frames[0]);
        return true;
    }
    return false;
}

bool TzxGrid::add(const suFMatrix& grid)
{
    if (grid.rows() == m_rows && grid.cols() == m_cols)
    {
        m_frames.push_back(grid);
        return true;
    }
    return false;
}
bool TzxGrid::set(const std::vector<suFMatrix>& grids)
{
    size_t currLen = m_frames.size();
    for (const auto& g : grids)
    {
        if (!add(g))
        {
            m_frames.resize(currLen);
            return false;
        }
    }

    return true;
}

suRect TzxGrid::rect() const
{
    suRect ret;
    ret.min.x = m_left;
    ret.min.y = m_bottom;
    ret.max.x = m_right;
    ret.max.y = m_top;
    return ret;
}
void TzxGrid::rect(const suRect& boundary)
{
    m_left = boundary.min.x;
    m_right = boundary.max.x;
    m_top = boundary.max.y;
    m_bottom = boundary.min.y;
    if (m_rows > 0)
    {
        m_ydelta = (m_top - m_bottom) / m_rows;
    }
    if (m_cols > 0)
    {
        m_xdelta = (m_right - m_left) / m_cols;
    }
}

size_t TzxGrid::rows() const
{
    return m_rows;
}
void TzxGrid::rows(const size_t& r)
{
    m_rows = r;
    if (m_rows > 0)
    {
        m_ydelta = (m_top - m_bottom) / m_rows;
    }
}

size_t TzxGrid::cols() const
{
    return m_cols;
}
void TzxGrid::cols(const size_t& c)
{
    m_cols = c;
    if (m_cols > 0)
    {
        m_xdelta = (m_right - m_left) / m_cols;
    }
}

float TzxGrid::xdelta() const
{
    return m_xdelta;
}
float TzxGrid::ydelta() const
{
    return m_ydelta;
}

bool TzxGrid::read(const suPath& file)
{
    return read(file, -1);
}

bool TzxGrid::read(const suPath& file, const int& index)
{
    bool status = false;
    release();

    if (const size_t fileSize = suPath::file_size(file); fileSize > 0)
    {
        const std::string buff = sufile::read(file);
        char* p = read_head(buff);
        if (!(TZX_GRID_1 == buff[1] && TZX_GRID_2 == buff[2] && TZX_GRID_3 == buff[3] && p))
        {
            return status;
        }
        if (TZX_GRID_V1 == buff[0])
        {
            status = unserialize_v1(p);
        }
        else if (TZX_GRID_V2 == buff[0])
        {
            status = unserialize_v2(p, index);
        }
        else if (TZX_GRID_0 == buff[0])
        {
            status = unserialize(p);
        }
    }
    return status;
}

bool TzxGrid::save_v1(const suPath& file)
{
    std::string buff;
    m_prefix[0] = TZX_GRID_V1;
    if (serialize_v1(buff))
    {
        return sufile::write(file, buff) > 0;
    }
    return false;
}
bool TzxGrid::save_v2(const suPath& file)
{
    std::string buff;
    m_prefix[0] = TZX_GRID_V2;
    if (serialize_v2(buff))
    {
        return sufile::write(file, buff) > 0;
    }
    return false;
}
bool TzxGrid::save(const suPath& file)
{
    return save_v2(file);
}

bool TzxGrid::serialize(std::string& buff)
{
    bool status = false;
    buff.clear();
    if (m_frames.empty() || !m_frames[0].rows() || !m_frames[0].cols())  // 以网格点数据为主
    {
        return status;
    }
    m_total = 0;
    m_total += (sizeof(m_prefix) + sizeof(m_total) + sizeof(m_left) * 4 + sizeof(m_xdelta) * 2);
    m_total += (sizeof(m_name) + sizeof(m_units) + sizeof(m_rows) + sizeof(m_cols));
    m_total += sizeof(float) * m_frames[0].rows() * m_frames[0].cols();

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

    memcpy(p, &m_rows, sizeof(m_rows));
    p += sizeof(m_rows);
    memcpy(p, &m_cols, sizeof(m_cols));
    p += sizeof(m_cols);

    memcpy(p, m_frames[0].data(), m_rows * m_cols * sizeof(float));
    status = true;

    return status;
}

bool TzxGrid::serialize_v1(std::string& buff)
{
    bool status = false;
    buff.clear();
    if (!m_frames[0].rows() || !m_frames[0].cols())  // 以网格点数据为主
    {
        return status;
    }
    // 先计算压缩数据大小
    std::string srcBin;
    size_t srcLen = m_frames[0].rows() * m_frames[0].cols() * sizeof(float);
    srcBin.assign(reinterpret_cast<char*>(m_frames[0].data()), srcLen);
    std::string cpsBin;
    if (!lz4_cps_data(srcBin, cpsBin))
    {
        cpsBin.clear();
        return false;
    }
    size_t cpsLen = cpsBin.size();
    m_total = sizeof(m_prefix) + sizeof(m_total) + sizeof(m_left) * 4 + sizeof(m_xdelta) * 2 + sizeof(m_name) + sizeof(m_units) + sizeof(m_rows) + sizeof(m_cols) + sizeof(cpsLen) + cpsLen;
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

    TZX_GRID_MEMCPY_TYPE(p, m_rows)

    TZX_GRID_MEMCPY_TYPE(p, m_cols)

    TZX_GRID_MEMCPY_TYPE(p, cpsLen)

    memcpy(p, cpsBin.data(), cpsLen);
    status = true;
    return status;
}
bool TzxGrid::serialize_v2(std::string& buff)
{
    if (m_frames.empty())
    {
        return false;
    }
    int frameBuffLen = m_rows * m_cols * sizeof(float);
    m_buff.resize(m_frames.size());
    m_total = 0;
    for (int i = 0; i < m_frames.size(); i++)
    {
        std::string tmpBuff(frameBuffLen, 0);
        std::memcpy(tmpBuff.data(), m_frames[i].data(), frameBuffLen);
        std::string tmpCpsBin;
        if (!lz4_cps_data(tmpBuff, m_buff[i]))
        {
            m_buff[i].clear();
        }
        m_total += sizeof(size_t);
        m_total += m_buff[i].size();
    }

    m_total += (sizeof(m_prefix) + sizeof(m_total) + sizeof(m_left) * 4 + sizeof(m_xdelta) * 2);
    m_total += (sizeof(m_name) + sizeof(m_units) + sizeof(m_rows) + sizeof(m_cols));

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

    memcpy(p, &m_rows, sizeof(m_rows));
    p += sizeof(m_rows);
    memcpy(p, &m_cols, sizeof(m_cols));
    p += sizeof(m_cols);

    for (auto& tmpB : m_buff)
    {
        size_t tmpL = tmpB.size();
        TZX_GRID_MEMCPY_TYPE(p, tmpL);
        memcpy(p, tmpB.data(), tmpL);
        p += tmpL;
    }

    return true;
}

bool TzxGrid::unserialize(char* p)
{
    bool status = false;

    m_frames.resize(1);
    m_frames[0].create(m_rows, m_cols, true);
    memcpy(m_frames[0].data(), p, m_rows * m_cols * sizeof(float));

    status = true;
    return status;
}

bool TzxGrid::unserialize_v1(char* p)
{
    if (!p)
    {
        return false;
    }
    size_t cpsLen = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(size_t);

    std::string cpsBin;
    cpsBin.assign(p, cpsLen);
    std::string dCpsBin;
    bool status = lz4_dcps_data(cpsBin, dCpsBin);
    size_t dCpsLen = dCpsBin.size();
    if (status && !dCpsBin.empty() && dCpsLen == m_rows * m_cols * sizeof(float))
    {
        m_frames.resize(1);
        m_frames[0].create(m_rows, m_cols, true);
        memcpy(m_frames[0].data(), dCpsBin.data(), m_rows * m_cols * sizeof(float));
        return true;
    }

    return false;
}

bool TzxGrid::unserialize_v2(char* p, const int& index)
{
    if (!p)
    {
        return false;
    }
    m_frames.clear();
    size_t currLen = m_header_len;
    int i = 0;
    while (currLen < m_total)
    {
        size_t cpsLen = reinterpret_cast<size_t*>(p)[0];
        p += sizeof(size_t);
        currLen += sizeof(cpsLen) + cpsLen;

        if (index < 0 || i++ == index)
        {
            std::string cpsBin;
            cpsBin.assign(p, cpsLen);  // 读取目标帧压缩数据
            std::string dCpsBin;
            bool status = lz4_dcps_data(cpsBin, dCpsBin);
            size_t dCpsLen = dCpsBin.size();
            suFMatrix tmpMatrix;
            if (status && !dCpsBin.empty() && dCpsLen == m_rows * m_cols * sizeof(float))
            {
                tmpMatrix.create(m_rows, m_cols, true);
                memcpy(tmpMatrix.data(), dCpsBin.data(), m_rows * m_cols * sizeof(float));
            }
            m_frames.push_back(tmpMatrix);
            if (index >= 0 && i >= index)
            {
                break;
            }
        }

        p += cpsLen;
    }
    return !m_frames.empty();
}

void TzxGrid::puzzle(const std::vector<silly_tzx_grid>& grids, const suRect& boundary, const float& d)
{
    if (grids.empty())
    {
        return;
    }
    m_left = boundary.min.x;
    m_top = boundary.max.y;
    m_right = boundary.max.x;
    m_bottom = boundary.min.y;
    m_ydelta = d;
    m_xdelta = d;
    m_rows = std::round((m_top - m_bottom) / m_ydelta);
    m_cols = std::round((m_right - m_left) / m_xdelta);
    size_t minSize = 999;
    for (const auto& grid : grids)
    {
        minSize = std::min(minSize, grid.frame_num());
    }
    m_frames.resize(minSize);
    for (auto& frame : m_frames)
    {
        frame.create(m_rows, m_cols);

        for (int r = 0; r < m_rows; r++)
        {
            for (int c = 0; c < m_cols; c++)
            {
                float x = m_left + c * m_xdelta;
                float y = m_top - r * m_ydelta;
                // 使用两个值之间的最大值
                float value = -999999.0;  // invalid data

                for (auto& grid : grids)
                {
                    if (std::abs(grid.m_xdelta - d) > 0.0001)
                    {
                        continue;
                    }

                    int tx = std::round((x - grid.m_left) / grid.m_xdelta);
                    int ty = std::round((grid.m_top - y) / grid.m_ydelta);
                    if (tx >= 0 && tx < grid.m_cols && ty >= 0 && ty < grid.m_rows)
                    {
                        value = SU_MAX(value, grid.m_frames[0][ty][tx]);
                    }
                }
                frame[r][c] = value;
            }
        }
    }
    
}
silly_tzx_grid& TzxGrid::operator=(const silly_tzx_grid& rh)
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

        this->m_rows = rh.m_rows;
        this->m_cols = rh.m_cols;
        this->m_frames = rh.m_frames;
    }

    return *this;
}
bool TzxGrid::same(const silly_tzx_grid& rh) const
{
    bool ret = true;
    ret &= (this->m_cols > 0 && rh.m_rows > 0);
    ret &= (this->m_left == rh.m_left);
    ret &= (this->m_right == rh.m_right);
    ret &= (this->m_top == rh.m_top);
    ret &= (this->m_bottom == rh.m_bottom);
    ret &= (this->m_xdelta == rh.m_xdelta);
    ret &= (this->m_ydelta == rh.m_ydelta);

    ret &= (this->m_rows == rh.m_rows);
    ret &= (this->m_cols == rh.m_cols);
    return ret;
}
bool TzxGrid::valid() const
{
    if (m_frames.empty())
    {
        return false;
    }
    if (m_cols == 0 || m_rows == 0)
    {
        return false;
    }
    return true;
}

bool TzxGrid::lz4_cps_data(const std::string& src, std::string& dst) const
{
    if (!src.empty())
    {
        int tryLen = m_rows * m_cols * sizeof(float) * 1.5;
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

bool TzxGrid::lz4_dcps_data(const std::string& src, std::string& dst) const
{
    if (!src.empty())
    {
        int tryLen = m_rows * m_cols * sizeof(float) * 1.5;
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

void TzxGrid::release()
{
    for (auto& m : m_frames)
    {
        m.release();
    }
    m_frames.clear();
}
char* TzxGrid::read_head(const std::string& buff)
{
    size_t len = buff.size();
    char* p = const_cast<char*>(buff.data());
    memcpy(m_prefix, p, sizeof(m_prefix));
    p += sizeof(m_prefix);

    m_total = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(m_total);

    if (m_total != len)
    {
        m_total = 0;
        p = nullptr;
        return p;
    }

    m_left = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_left);

    m_top = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_top);

    m_right = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_right);

    m_bottom = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_bottom);

    m_xdelta = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_xdelta);

    m_ydelta = reinterpret_cast<float*>(p)[0];
    p += sizeof(m_ydelta);

    memcpy(m_name, p, sizeof(m_name));
    p += sizeof(m_name);

    memcpy(m_units, p, sizeof(m_units));
    p += sizeof(m_units);

    m_rows = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(m_rows);

    m_cols = reinterpret_cast<size_t*>(p)[0];
    p += sizeof(m_cols);

    return p;
}

std::string TzxGrid::write_header() const
{
    size_t hLen = 0;
    hLen += (sizeof(m_prefix) + sizeof(m_total) + sizeof(m_left) * 4 + sizeof(m_xdelta) * 2);
    hLen += (sizeof(m_name) + sizeof(m_units) + sizeof(m_rows) + sizeof(m_cols));
    std::string ret = std::string(hLen, 0);
    char* p = ret.data();
    TZX_GRID_MEMCPY_TYPE(p, m_total)

    TZX_GRID_MEMCPY_TYPE(p, m_left)

    TZX_GRID_MEMCPY_TYPE(p, m_top)

    TZX_GRID_MEMCPY_TYPE(p, m_right)

    TZX_GRID_MEMCPY_TYPE(p, m_bottom)

    TZX_GRID_MEMCPY_TYPE(p, m_xdelta)

    TZX_GRID_MEMCPY_TYPE(p, m_ydelta)

    TZX_GRID_MEMCPY_TYPE(p, m_name)

    TZX_GRID_MEMCPY_TYPE(p, m_units)

    TZX_GRID_MEMCPY_TYPE(p, m_rows)

    TZX_GRID_MEMCPY_TYPE(p, m_cols)

    return ret;
}
