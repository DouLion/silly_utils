/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-14
 * @file: silly_gdal_raster.cpp
 * @description: silly_gdal_raster 头文件
 * @version: v1.0.1 2025-08-14 dou li yang
 */
#ifndef SILLY_GDAL_RASTER_H
#define SILLY_GDAL_RASTER_H
#include <files/silly_file.h>
#include <metadata/silly_dem.h>
#include <gdal.h>
#include <gdal_priv.h>

class silly_gdal_raster
{
  public:
    silly_gdal_raster() = default;
    ~silly_gdal_raster()
    {
        close();
    }

    // 禁止拷贝
    silly_gdal_raster(const silly_gdal_raster&) = delete;
    silly_gdal_raster& operator=(const silly_gdal_raster&) = delete;
    bool open(const std::filesystem::path& file);
    void close();

    su::DMatrix ROI(const silly_rect& rect) const;

    double Pick(const silly_point& p) const;

  private:
#if SU_THIRD_SUPPORT_GDAL
    GDALDataset* m_pPoDataset = nullptr;
    GDALRasterBand* m_pPoBand0 = nullptr;
    double m_x0 = 0;
    double m_y0 = 0;
    int m_Bands = 0;
    GDALDataType m_DataType = GDT_Unknown;
    int m_width = 0;
    int m_height = 0;
    silly_rect m_rect;
    double m_xdelta = 0;
    double m_ydelta = 0;
    bool UP2DOWN = true;
#endif
};

#endif  // SILLY_GDAL_RASTER_H
