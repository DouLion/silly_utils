/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-14
 * @file: silly_radar_occluder.h
 * @description: 雷达遮挡物分析
 * @version: v1.0.1 2025-02-14 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_RADAR_OCCLUDER_H
#define SILLY_UTILS_SILLY_RADAR_OCCLUDER_H
#include <geo/silly_geo_utils.h>

class silly_radar_occluder
{
    struct dem
    {
        std::vector<double> data;
        size_t rows;
        size_t cols;
        silly_rect rect;
        double l0 = -9999; // -9999表示经纬度,否则表示高斯中心线
    };
    struct site
    {
        double radius = 0;     // 半径 单位(米)
        silly_point position;  // 位置
        double elevation = 0;  // 高程
        double altitude = 0;   // 相对地面高度,这个可以没有
    };
  public:
    /// <summary>
    /// 设置雷达站点基本信息
    /// </summary>
    /// <param name="info"></param>
    void set(const site& info);

    /// <summary>
    /// 设置dem信息
    /// </summary>
    /// <param name="info"></param>
    void set(const dem& info);

    /// <summary>
    /// 提取雷达遮挡范围矢量
    /// </summary>
    /// <param name="pitch">俯仰角度(度),指向天空为正,指向地面为负</param>
    /// <param name="elev">雷达高程</param>
    /// <param name="radius">雷达工作扫描半径(米)</param>
    silly_poly occluder_poly(const double& pitch, const double& elev = 0.0, const double& radius = 0.0);

  private:
    void check_beam(size_t deg, double pitch, double elev, double radius, double* stopLen);

  private:
    std::vector<std::vector<double>> m_polar_dem;  // 极坐标DEM std::vector<弧度, std::vector<半径>>
    silly_point m_center;
    double m_elevation = 0;
    double m_radius = 0;
    double m_l0 = -9999;

    double m_dStep = 0.5; // 角度步长  度 
    double m_rStep = 25.0; // 半径步长 25 米
    size_t m_dSize = 720;
    size_t m_rSize = 0;
};

#endif  // SILLY_UTILS_SILLY_RADAR_OCCLUDER_H
