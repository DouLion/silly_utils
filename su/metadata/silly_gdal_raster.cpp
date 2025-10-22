/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-14
 * @file: silly_gdal_raster.c
 * @description: silly_gdal_raster实现
 * @version: v1.0.1 2025-08-14 dou li yang
 */
#include "silly_gdal_raster.h"
bool silly_gdal_raster::Open(const std::filesystem::path& file)
{
#if SU_THIRD_SUPPORT_GDAL
    // CPLSetConfigOption("GDAL_CACHEMAX", "512");  // 设置 GDAL 缓存为 512MB
    m_pPoDataset = (GDALDataset*)GDALOpen(file.string().c_str(), GA_ReadOnly);
    if (!m_pPoDataset)
    {
        std::cerr << "无法打开输入文件: " << file.u8string() << std::endl;
        return false;
    }

    // 获取影像信息
    m_Bands = m_pPoDataset->GetRasterCount();
    if (m_Bands == 0)
    {
        std::cerr << "没有波段数据!" << std::endl;
        Close();
        return false;
    }

    m_pPoBand0 = m_pPoDataset->GetRasterBand(1);
    if (!m_pPoBand0)
    {
        std::cerr << "没有波段数据!" << std::endl;
        Close();
        return false;
    }

    m_DataType = m_pPoBand0->GetRasterDataType();
    if (!(m_DataType == GDT_Float32 || m_DataType == GDT_Float64))
    {
        std::cerr << "波段数据类型不是 Float32 或 Float64!" << std::endl;
        Close();
        return false;
    }

    double m_adfGeoTransform[6] = {};
    if (m_pPoDataset->GetGeoTransform(m_adfGeoTransform) != CE_None)
    {
        std::cerr << "无法获取地理变换参数!" << std::endl;
        Close();
        return false;
    }

    // 边界检查
    m_width = m_pPoDataset->GetRasterXSize();
    m_height = m_pPoDataset->GetRasterYSize();
    m_x0 = m_adfGeoTransform[0];
    m_y0 = m_adfGeoTransform[3];
    m_xdelta = m_adfGeoTransform[1];
    m_ydelta = m_adfGeoTransform[5];
    double x1 = m_x0 + m_width * m_xdelta;
    double y1 = m_y0 + m_height * m_ydelta;
    UP2DOWN = false;
    if (m_ydelta < 0.0)
    {
        UP2DOWN = true;
    }

    m_rect.min.x = std::min(m_x0, x1);
    m_rect.min.y = std::min(m_y0, y1);
    m_rect.max.x = std::max(m_x0, x1);
    m_rect.max.y = std::max(m_y0, y1);
    return true;
#endif
    return false;
}
void silly_gdal_raster::Close()
{
#if SU_THIRD_SUPPORT_GDAL
    if (m_pPoBand0)
    {
        m_pPoBand0 = nullptr;
    }
    if (m_pPoDataset)
    {
        GDALClose(m_pPoDataset);
        m_pPoDataset = nullptr;
    }

#endif
}
suDMatrix silly_gdal_raster::ROI(const suRect& rect) const
{
    suDMatrix ret;
#if SU_THIRD_SUPPORT_GDAL
    if (!m_pPoBand0 && m_Bands <= 0)
    {
        return ret;
    }
    // 计算目标地理范围对应的像素坐标
    /* m_xdelta = m_adfGeoTransform[1];
     m_ydelta = m_adfGeoTransform[5];*/
    double dx = std::abs(m_xdelta);
    double dy = std::abs(m_ydelta);
    if (!m_rect.intersect(rect))
    {
        std::cerr << "不相交" << std::endl;
        return ret;
    }

    suRect useR = m_rect.intersection(rect);

    // 地理坐标 -> 像素坐标（行列）
    const int startX = std::round((useR.min.x - m_rect.min.x) / dx);
    int startY = std::round((m_rect.max.y - useR.max.y) / dy);
    int xOff = std::round((useR.min.x - rect.min.x) / dx);
    int yOff = std::round((rect.max.y - useR.max.y) / dy);

    if (dy > 0)
    {
        // TODO, 从下而上
    }

    int roiW = std::round((rect.max.x - rect.min.x) / dx);
    int roiH = std::round((rect.max.y - rect.min.y) / dy);

    // 修正超出边界的裁剪区域
    int readW = std::round((useR.max.x - useR.min.x) / dx);
    int readH = std::round((useR.max.y - useR.min.y) / dy);

    // 分配缓存（自动根据数据类型计算大小）
    int nBytes = GDALGetDataTypeSizeBytes(m_DataType) * readW * readH;
    void* pData = CPLMalloc(nBytes);

    CPLErr err = m_pPoBand0->RasterIO(GF_Read, startX, startY, readW, readH, pData, readW, readH, m_DataType, 0, 0, nullptr);
    if (err != CE_None)
    {
        std::cerr << "读取波段失败!" << std::endl;
        CPLFree(pData);
    }

    if (GDT_Float32 == m_DataType)
    {
        ret.create(roiH, roiW);
        float* p = static_cast<float*>(pData);
        for (int r = 0; r < readH; ++r)
        {
            for (int c = 0; c < readW; ++c)
            {
                int rR = r + yOff;
                int rC = c + xOff;
                ret[rR][rC] = p[r * readW + c];
                // ret[r][c] = p[rR * readW + rC];
            }
        }
    }
    else if (GDT_Float64 == m_DataType)
    {
        ret.create(roiH, roiW);
        double* p = static_cast<double*>(pData);
        for (int r = 0; r < readH; ++r)
        {
            for (int c = 0; c < readW; ++c)
            {
                int rR = r + yOff;
                int rC = c + xOff;
                ret[rR][roiW - rC - 1] = p[r * readW + c];
            }
        }
    }
    CPLFree(pData);
#endif
    return ret;
}

double silly_gdal_raster::Pick(const suPoint& p) const
{
    double ret = NAN;
#if SU_THIRD_SUPPORT_GDAL
    if (!m_pPoBand0 && m_Bands <= 0)
    {
        return ret;
    }
    if (!m_rect.contain(p))
    {
        return ret;
    }
    // 读取该像素的值
    int nBytes = GDALGetDataTypeSizeBytes(m_DataType);
    std::vector<char> buffer(nBytes);
    int px = std::round((p.x - m_x0) / m_xdelta);
    int py = std::round((p.y - m_y0) / m_ydelta);
    CPLErr err = m_pPoBand0->RasterIO(GF_Read, px, py, 1, 1, buffer.data(), 1, 1, m_DataType, 0, 0, nullptr);
    if (err != CE_None)
    {
        std::cerr << "读取像素失败" << std::endl;
        return ret;
    }
    if (m_DataType == GDT_Float32)
    {
        ret = static_cast<double>(*reinterpret_cast<float*>(buffer.data()));
    }
    else if (m_DataType == GDT_Float64)
    {
        ret = *reinterpret_cast<double*>(buffer.data());
    }
    else if (m_DataType == GDT_Int16)
    {
        ret = static_cast<double>(*reinterpret_cast<int16_t*>(buffer.data()));
    }
    else if (m_DataType == GDT_UInt16)
    {
        ret = static_cast<double>(*reinterpret_cast<uint16_t*>(buffer.data()));
    }
    else if (m_DataType == GDT_Int32)
    {
        ret = static_cast<double>(*reinterpret_cast<int32_t*>(buffer.data()));
    }
    else if (m_DataType == GDT_UInt32)
    {
        ret = static_cast<double>(*reinterpret_cast<uint32_t*>(buffer.data()));
    }
#endif
    return ret;
}

suRect silly_gdal_raster::Bound() const
{
    return m_rect;
}
double silly_gdal_raster::XDelta() const
{
    return m_xdelta;
}
double silly_gdal_raster::YDelta() const
{
    return m_ydelta;
}
int silly_gdal_raster::Width() const
{
    return m_width;
}
int silly_gdal_raster::Height() const
{
    return m_height;
}