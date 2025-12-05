/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-14
 * @file: silly_gdal_raster.cpp
 * @description: 1. 主要使用此类加载高程数据;
 *               2. 其他类型数据需要扩展,使其支持多波段数据的读取;
 *               3. 始终需要明确点表示的是网格的中心位置还是其他,
 *                  这个会涉及到矩阵返回的确定,尽管偏差会很小,现
 *                  在默认所有的高程数据中,每个网格都用其中心点表
 *                  示其坐标位置
 *
 * @version: v1.0.1 2025-08-14 dou li yang
 */
#ifndef SILLY_GDAL_RASTER_H
#define SILLY_GDAL_RASTER_H
#include <files/silly_file.h>
#include <metadata/silly_dem.h>
#include <gdal.h>
#include <gdal_priv.h>

class suGdalRaster
{
  public:
    suGdalRaster() = default;
    ~suGdalRaster()
    {
        Close();
    }
    bool Open(const suPath& file);
    void Close();

    suRect Bound() const;
    double DX() const;
    double DY() const;
    int Width() const;
    int Height() const;

    /**
     * 根据给定的矩形范围提取出目标区域
     * 并且返回与原始dx,dy保持一致的矩形框
     * 以减少后续计算中的偏差
     * @param bound 给定的矩形范围
     * @param fixed 与返回的高程矩阵完全一致的矩形范围, 仅包含相交区域
     * @return
     */
    suDem ROI(const suRect& bound) const;

    /**
     * 获取指定点的高程
     * @param p 点位置
     * @return
     */
    double Get(const suPoint& p) const;
    double GetBiLinear(const suPoint& p) const;
    double GetBiCubic(const suPoint& p) const;

  protected:
    double m_x0 = 0;
    double m_y0 = 0;
    int m_Bands = 0;
    int m_width = 0;
    int m_height = 0;
    suRect m_rect;
    double m_dx = 0;
    double m_dy = 0;
    bool m_UP2DOWN = true;
#if SU_THIRD_SUPPORT_GDAL
    GDALDataset* m_pPoDataset = nullptr;
    GDALRasterBand* m_pPoBand0 = nullptr;

    GDALDataType m_DataType = GDT_Unknown;

#endif
};

#endif  // SILLY_GDAL_RASTER_H
