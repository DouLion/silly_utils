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
bool suGdalRaster::Open(const suPath& file)
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

    // TODO: 多波段数据默认获取第一个波段
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
    m_dx = m_adfGeoTransform[1];
    m_dy = m_adfGeoTransform[5];
    double x1 = m_x0 + m_width * m_dx;
    double y1 = m_y0 + m_height * m_dy;
    m_UP2DOWN = false;
    if (m_dy < 0.0)
    {
        m_UP2DOWN = true;
    }

    m_rect.min.x = std::min(m_x0, x1);
    m_rect.min.y = std::min(m_y0, y1);
    m_rect.max.x = std::max(m_x0, x1);
    m_rect.max.y = std::max(m_y0, y1);
    return true;
#endif
    return false;
}
void suGdalRaster::Close()
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
suDem suGdalRaster::ROI(const suRect& bound) const
{
    suDem ret;
#if SU_THIRD_SUPPORT_GDAL
    if (!m_pPoBand0 && m_Bands <= 0)
    {
        return ret;
    }
    // 计算目标地理范围对应的像素坐标
    ret.info.dx = std::abs(m_dx);
    ret.info.dy = std::abs(m_dy);
    if (!m_rect.intersect(bound))
    {
        std::cerr << "不相交" << std::endl;
        return ret;
    }
    ret.info.bound = m_rect.intersection(bound);

    // 此处后续需要谨慎, 根据 相交区域计算出的坐标应该不会超出 原始DEM的宽高
    const int minX = std::floor((ret.info.bound.min.x - m_rect.min.x) / ret.info.dx);
    const int maxX = std::ceil((ret.info.bound.max.x - m_rect.min.x) / ret.info.dx);
    const int minY = std::floor((m_rect.max.y - ret.info.bound.max.y) / ret.info.dy);
    const int maxY = std::ceil((m_rect.max.y - ret.info.bound.min.y) / ret.info.dy);
    ret.info.bound.min.x = m_rect.min.x + minX * ret.info.dx;
    ret.info.bound.max.x = m_rect.min.x + maxX * ret.info.dx;
    ret.info.bound.min.y = m_rect.max.y - maxY * ret.info.dy;
    ret.info.bound.max.y = m_rect.max.y - minY * ret.info.dy;

    if (!m_UP2DOWN)
    {
        // TODO, 从下而上
    }

    ret.info.width = (maxX - minX);
    ret.info.height = (maxY - minY);

    // 分配缓存（自动根据数据类型计算大小）
    const int byteNum = GDALGetDataTypeSizeBytes(m_DataType) * ret.info.width * ret.info.height;
    void* pData = CPLMalloc(byteNum);

    CPLErr err = m_pPoBand0->RasterIO(GF_Read, minX, minY, ret.info.width, ret.info.height, pData, ret.info.width, ret.info.height, m_DataType, 0, 0, nullptr);
    if (err != CE_None)
    {
        std::cerr << "读取波段失败!" << std::endl;
        CPLFree(pData);
    }
    ret.raster.create(ret.info.height, ret.info.width);
    float* pRet = ret.raster.data();
    if (GDT_Float32 == m_DataType)
    {
        float* ptr = static_cast<float*>(pData);

        for (int r = 0; r < ret.info.height; ++r)
        {
            for (int c = 0; c < ret.info.width; ++c)
            {
                *pRet = *ptr;
                ++pRet;
                ++ptr;
            }
        }
    }
    else if (GDT_Float64 == m_DataType)
    {
        double* ptr = static_cast<double*>(pData);
        std::memcpy(pRet, ptr, ret.info.width * ret.info.height * sizeof(double));
    }
    CPLFree(pData);
#endif
    return ret;
}

double suGdalRaster::Get(const suPoint& p) const
{
    double ret = std::numeric_limits<double>::quiet_NaN();
    ;
#if SU_THIRD_SUPPORT_GDAL
    if (!m_pPoBand0 && m_Bands <= 0)
    {
        return ret;
    }
    if (!m_rect.contains(p))
    {
        return ret;
    }
    // 读取该像素的值
    int byteNum = GDALGetDataTypeSizeBytes(m_DataType);
    std::vector<char> buffer(byteNum);
    int px = std::round((p.x - m_x0) / m_dx);
    int py = std::round((p.y - m_y0) / m_dy);
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
double suGdalRaster::GetBiLinear(const suPoint& p) const
{
    return std::numeric_limits<double>::quiet_NaN();
    ;
}
double suGdalRaster::GetBiCubic(const suPoint& p) const
{
    return std::numeric_limits<double>::quiet_NaN();
    ;
}

suRect suGdalRaster::Bound() const
{
    return m_rect;
}
double suGdalRaster::DX() const
{
    return m_dx;
}
double suGdalRaster::DY() const
{
    return m_dy;
}
int suGdalRaster::Width() const
{
    return m_width;
}
int suGdalRaster::Height() const
{
    return m_height;
}