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
bool silly_egm::open(const std::string& file)
{
    if(m_mmap.open_m(file))
    {
        return read_header();
    }
    return false;
}
double silly_egm::geoid(const double& lgtd, const double& lttd)
{
    int row = static_cast<int>(((m_headers.max_lat - lttd) / (m_headers.max_lat - m_headers.min_lat)) * m_headers.rows);
    int col = static_cast<int>(((lgtd - m_headers.min_lon) / (m_headers.max_lon - m_headers.min_lon)) * m_headers.cols);
    if (row >= m_headers.rows || col >= m_headers.cols || row < 0 || col < 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    double result = 0;
    size_t index = row * m_headers.cols + col;
    index += m_doffs;
    char buffer[2] = {0};
    if (m_mmap.read_m(buffer, m_headers.dlen, index))
    {
        if (m_headers.dlen == 1)
        {
            result = static_cast<unsigned char>(buffer[0]);
        }
        else
        {
            result = static_cast<unsigned short>(buffer[0] << 8) + static_cast<unsigned short>(buffer[1]);
        }
        return result * m_headers.scale + m_headers.offset;
    }
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
    std::string  buffer;
    buffer.resize(buffer_size);
    size_t offset = 0;
    std::regex re1(R"(^(\d+)\s+(\d+)$)");
    if(m_mmap.read_m(buffer.data(), buffer_size, offset))
    {
        std::string line;

        std::istringstream stream(buffer);
        while (std::getline(stream, line))
        {
            m_doffs += (line.size() + 1);
            m_headers.infos.push_back(line);
            std::smatch match;
            if(std::regex_match(line, match, re1))
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
