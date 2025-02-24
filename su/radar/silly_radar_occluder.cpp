/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-14
 * @file: silly_radar_occluder.cpp
 * @description: silly_radar_occluder实现
 * @version: v1.0.1 2025-02-14 dou li yang
 */
#include "silly_radar_occluder.h"

silly_point polar2cartesian(const silly_point& center, const double& r, const double& deg)
{
    return silly_point(center.x + r * std::cos(DEG2RAD(deg)), center.y + r * std::sin(DEG2RAD(deg)));
}

bool silly_radar_occluder::dem2polar(const std::string& dem, const silly_point& center, const double& radius)
{
    return false;
}

silly_poly silly_radar_occluder::occluder_poly(const double& height, const double& deg, const double& radius)
{
    silly_poly poly;
    double n = radius * 1000.0 / m_polar_dem.front().size();
    double dp = 2 * SU_PI / m_polar_dem.size(); // 极坐标角度变化值
    double rad = DEG2RAD(deg);
    for (int i = 0; i < m_polar_dem.size(); ++i)
    {
        for (int j = 0; j < m_polar_dem[i].size(); ++j)
        {
            double h = n * j * std::atan(rad) + height;
            if (h < m_polar_dem[i][j])
            {
                poly.outer_ring.points.push_back(polar2cartesian(m_center, j*n, i * dp));
            }
        }
    }

    return poly;

}
