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
#include <math/interpolate/silly_bilinear.h>
bool silly_egm::open(const std::string& file)
{
    if (m_mmap.open_m(file))
    {
        return read_header();
    }
    return false;
}
double silly_egm::geoid(const double& lgtd, const double& lttd)
{
    double result = 0;
    double dr = (m_headers.origin_lat - lttd) / m_lat_ratio;
    double dc = ((lgtd > 0. ? lgtd : lgtd + 360.) - m_headers.origin_lon) / m_lon_ratio;

    std::cout << dr << "," << dc << std::endl;

    int r0 = std::floor(dr);
    int c0 = std::floor(dc);
    int r1 = SU_MIN(r0 + 1, m_headers.rows - 1);
    int c1 = SU_MIN(c0 + 1, m_headers.cols - 1);

    double ddr = dr - r0;
    double ddc = dc - c0;

    double g00 = 0, g01 = 0, g10 = 0, g11 = 0;
    size_t idx00 = (r0 * m_headers.cols + c0) * m_headers.dlen + m_doffs;
    size_t idx10 = (r1 * m_headers.cols + c0) * m_headers.dlen + m_doffs;
    unsigned char buffer[4] = {0};
    m_mmap.read_m((char*)buffer, 4, idx00);
    if (m_headers.dlen == 1)
    {
        g00 = (buffer[0]);
        g01 = (buffer[1]);
    }
    else
    {
        g00 = ((buffer[0] << 8) | (buffer[1]));
        g01 = ((buffer[2] << 8) | (buffer[3]));
    }
    memset(buffer, 0, 4);
    m_mmap.read_m((char*)buffer, 4, idx10);

    if (m_headers.dlen == 1)
    {
        g10 = (buffer[0]);
        g11 = (buffer[1]);
    }
    else
    {
        g10 = ((buffer[0]) << 8) | (buffer[1]);
        g11 = ((buffer[2]) << 8) | (buffer[3]);
    }

    g00 = g00 * 0.003 - 108.0;
    g01 = g01 * 0.003 - 108.0;
    g10 = g10 * 0.003 - 108.0;
    g11 = g11 * 0.003 - 108.0;

    result = silly::interpolation::bilinear::calc(g00, g01, g10, g11, ddc, ddr);
    return result;
}
bool silly_egm::close()
{
    m_mmap.close_m();
    return true;
}
bool silly_egm::read_header()
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
    const size_t buffer_size = 2048;
    std::string buffer;
    buffer.resize(buffer_size);
    size_t offset = 0;
    std::regex re1(R"(^(\d+)\s+(\d+)$)");
    if (m_mmap.read_m(buffer.data(), buffer_size, offset))
    {
        std::string line;

        std::istringstream stream(buffer);
        while (std::getline(stream, line))
        {
            m_doffs += (line.size() + 1);
            m_headers.infos.push_back(line);
            std::smatch match;
            if (std::regex_match(line, match, re1))
            {
                if (match.size() == 3)
                {
                    m_headers.cols = std::stoi(match[1].str());
                    m_headers.rows = std::stoi(match[2].str());
                    status = true;
                    break;
                }
            }
        }
        std::getline(stream, line);
        m_doffs += (line.size() + 1);
        m_headers.infos.push_back(line);
        int maxv = std::stod(m_headers.infos.back());
        m_headers.dlen = maxv > 255 ? 2 : 1;

        m_lat_ratio = (m_headers.max_lat - m_headers.min_lat) / (m_headers.rows - 1);
        m_lon_ratio = (m_headers.max_lon - m_headers.min_lon) / (m_headers.cols);
    }
    return status;
}
silly_egm::~silly_egm()
{
    close();
}
double silly_egm::orthometric(const double& lgtd, const double& lttd, const double& ellip)
{
    double geoid_ = geoid(lgtd, lttd);
    if (std::isnan(geoid_))
    {
        return std::numeric_limits<double>::quiet_NaN();  // 返回NaN表示无效
    }
    return ellip - geoid_;
}
