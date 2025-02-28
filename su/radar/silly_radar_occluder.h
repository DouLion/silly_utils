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
  public:
    /// <summary>
    /// 读取dem文件
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    bool get_dem(const std::string& file);

    /// <summary>
    /// 网格dem转极坐标
    /// </summary>
    /// <param name="center">中心坐标(经纬度)</param>
    /// <param name="radius">采样半径,雷达扫描半径(km)</param>
    /// <returns></returns>
    bool dem2polar(const silly_point& center, const double& radius);
    // bool silly_radar_occluder::dem2polar(const std::string& file, const silly_point& center, const double& radius);

    /// <summary>
    /// 提取雷达遮挡范围矢量
    /// </summary>
    /// <param name="height">雷达高程</param>
    /// <param name="deg">俯仰角度(度),指向天空为正,指向地面为负</param>
    /// <param name="radius">雷达工作扫描半径(km)</param>
    silly_poly occluder_poly(const double& height, const double& deg, const double& radius);

  public:
    std::vector<std::vector<double>> m_polar_dem;  // 极坐标DEM
    silly_point m_center;

    // dem二维矩阵数据
    std::vector<double> m_dem_data;
    size_t x_size{0};          // x轴大小
    size_t y_size{0};          // y轴大小
    silly_point left_top;      // 左上角坐标经纬度
    silly_point right_bottom;  // 右下角坐标经纬度
    silly_point left_bottom;   // 左下角坐标经纬度
    silly_point right_top;     // 右上角坐标经纬度
    double x_res{0};           // x轴分辨率 // 单位度
    double y_res{0};           // y轴分辨率 // 单位度
};

#endif  // SILLY_UTILS_SILLY_RADAR_OCCLUDER_H
