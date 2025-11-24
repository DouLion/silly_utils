/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-13
 * @file: silly_egm.cpp
 * @description: silly_egm实现
 * @version: v1.0.1 2024-12-13 dou li yang
 */
#include "silly_egm.h"
#include <math/interp/silly_bilinear.h>

suEarthGravityModel::~suEarthGravityModel()
{
    close();
}

bool suEarthGravityModel::open(const std::filesystem::path& file)
{
    if (m_mmap.open(file))
    {
        return read_header();
    }
    return false;
}

bool suEarthGravityModel::close()
{
    m_mmap.close();
    return true;
}

double suEarthGravityModel::geoid(const double& lon, const double& lat) const
{
    // 2. 计算浮点索引（相对于 origin）
    // dr: 从 origin_lat=90 向下，lat 减小 → dr 增大
    // dc: 从 origin_lon=0 向右，nlon 增大 → dc 增大
    double dr = (m_header.origin_lat - lat) / m_lat_ratio;   // 纬度索引（从上到下）
    double dc = (lon - m_header.origin_lon) / m_lon_ratio;  // 经度索引（从左到右）

    // 3. 取整并计算四个角点索引
    int r0 = static_cast<int>(std::floor(dr));
    int c0 = static_cast<int>(std::floor(dc));
    int r1 = SU_MIN(r0 + 1, m_header.rows - 1);
    int c1 = SU_MIN(c0 + 1, m_header.cols - 1);

    // 4. 计算小数部分（用于双线性插值）
    double ddr = dr - r0;  // 纬度方向小数（0~1）
    double ddc = dc - c0;  // 经度方向小数（0~1）

    // 5. 计算四个点的文件偏移（字节索引）
    auto calc_index = [&](int r, int c) -> size_t {
        return (static_cast<size_t>(r) * m_header.cols + c) * m_header.dlen + m_header.doff;
    };

    size_t idx00 = calc_index(r0, c0);
    size_t idx01 = calc_index(r0, c1);
    size_t idx10 = calc_index(r1, c0);
    size_t idx11 = calc_index(r1, c1);

    // 6. 读取四个点的原始值（uint16，小端序）
    auto read_val = [&](const size_t& idx) -> double {
        unsigned char buf[2] = {0};
        if (!m_mmap.read(buf, m_header.dlen, idx)) {
            std::cerr << "Failed to read uint16 from mmap file" << std::endl;
            return 0; // 或抛出异常
        }
        if (m_header.dlen == 1 )
        {
            return buf[0];;
        }
        if (m_header.dlen == 2)
        {
            return ((buf[0] << 8 | buf[1]));
        }
        return 0;
    };

    double v00 = read_val(idx00);
    double v01 = read_val(idx01);
    double v10 = read_val(idx10);
    double v11 = read_val(idx11);

    // 7. 解码 geoid 高度：value * scale - offset
    double g00 = v00 * m_header.scale + m_header.offset;
    double g01 = v01 * m_header.scale + m_header.offset;
    double g10 = v10 * m_header.scale + m_header.offset;
    double g11 = v11 * m_header.scale + m_header.offset;

    double result = suBilinearInterp::calc(g00, g01, g10, g11, ddc, ddr);

    return result;
}

bool suEarthGravityModel::read_header()
{
    bool status = false;
    /*
P5
# Geoid file in PGM format for the GeographicLib::Geoid class
# Description WGS84 EGM2008, 1-minute grid
# URL http://earth-info.nga.mil/GandG/wgs84/gravitymod/egm2008
# DateTime 2009-08-31 06:54:00
# MaxBilinearError 0.025
# RMSBilinearError 0.001
# MaxCubicError 0.003
# RMSCubicError 0.001
# Offset -108
# Scale 0.003
# Origin 90N 0E
# AREA_OR_POINT Point
# Vertical_Datum WGS84
21600  10801
65535
     */

    constexpr size_t buffer_size = 2048;
    std::string buffer;
    buffer.resize(buffer_size);
    std::regex re1(R"(^(\d+)\s+(\d+)$)");
    if (m_mmap.read(reinterpret_cast<suMemMapFile::Ptr>(buffer.data()), buffer_size, 0))
    {
        std::string line;
        std::vector<std::string> info_lines;
        std::istringstream stream(buffer);
        while (std::getline(stream, line))
        {
            m_header.doff += (line.size() + 1);
            info_lines.push_back(line);
            std::smatch match;
            if (std::regex_match(line, match, re1))
            {
                if (match.size() == 3)
                {
                    m_header.cols = std::stoi(match[1].str());
                    m_header.rows = std::stoi(match[2].str());
                    status = true;
                    break;
                }
            }
        }
        std::getline(stream, line);
        m_header.doff += (line.size() + 1);
        info_lines.push_back(line);
        int maxv = std::stod(info_lines.back());
        m_header.dlen = maxv > 255 ? 2 : 1;

        // 经度方向 0 和360是重合的, 而纬度方向 -90 和 90 不重合,
        // 如果范围不是全球, 则m_lon_ratio 计算时 也是(m_header.cols - 1)
        m_lat_ratio = (m_header.max_lat - m_header.min_lat) / (m_header.rows - 1);
        m_lon_ratio = (m_header.max_lon - m_header.min_lon) / (m_header.cols);
    }
    return status;
}

double suEarthGravityModel::orthometric(const double& lon, const double& lat, const double& ellipsoid) const
{
    double geoid_ = geoid(lon, lat);
    if (std::isnan(geoid_))
    {
        return std::numeric_limits<double>::quiet_NaN();  // 返回NaN表示无效
    }
    return ellipsoid - geoid_;
}
