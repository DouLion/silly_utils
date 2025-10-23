/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-14
 * @file: silly_dem.cpp
 * @description: silly_dem 头文件
 * @version: v1.0.1 2025-08-14 dou li yang
 */
#ifndef SILLY_DEM_H
#define SILLY_DEM_H
#include "math/silly_matrix.h"
#include <geo/silly_geo.h>
#include <geo/proj/silly_proj.h>
class suDem
{
  public:
    struct Info
    {
        double dx = 0;
        double dy = 0;
        double fill = -9999.0;
        size_t width = 0;
        size_t height = 0;
        suRect bound;
        double central = 0;  // 有效值 [75, 135] 间隔3, 否则认为高斯坐标系
    };

  public:
    /*
     * 根据central 的值判断是否是高斯
     */
    bool IsGauss() const;
    /**
     *
     * @param rh
     * @param cell_size
     * @param l0
     */
    void Gauss2Lonlat(const suDem& rh, const double& cell_size, const double& l0);

    /**
     *
     * @param rh
     * @param cell_size
     * @param l0
     */
    void Lonlat2Gauss(const suDem& rh, const double& cell_size, const double& l0);
    void Cover(const suDem& rh);

    /**
     * 将环外的点置为fill
     * @param ring
     */
    void Extra(const suRing& ring);

    /**
     *  提取环内的点
     * @param rh
     * @param ring
     */
    void Extra(const suDem& rh, const suRing& ring);

    suPoint ColRow(const suPoint& p) const;

    double At(const suPoint& p) const;
    double Round(const suPoint& p) const;
    double BiLiner(const suPoint& p) const;
    double BiCubic(const suPoint& p) const;

    /**
     * 提取剖面高程
     * @param line
     * @return
     */
    std::vector<std::pair<double, double>> ProfileElev(const suLine& line) const;

    /**
     * 提取等间距的剖面高程
     * @param line
     * @param dist 等间距的
     * @return
     */
    std::vector<std::pair<double, double>> ProfileElev(const suLine& line, const double& dist) const;

    /**
     * 梯度幅值
     * @return 梯度幅值矩阵
     */
    suDMatrix SlopeGradient(const int& method = 5) const;

    /**
     * 计算梯度角度值
     * @return 梯度角度值矩阵
     */
    suDMatrix SlopeDegree(const int& method = 5) const;

    /**
     * 计算梯度弧度值
     * @return 梯度弧度值矩阵
     */
    suDMatrix SlopeRadian(const int& method = 5) const;

    /**
     * 梯度百分比值
     * @return 梯度百分比矩阵
     */
    suDMatrix SlopePercent(const int& method = 5) const;

    void Release();

  public:
    Info info;
    suDMatrix raster;
};
#endif  // SILLY_DEM_H
