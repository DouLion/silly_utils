/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-05
 * @file: silly_huge_stitcher.cpp
 * @description: silly_huge_stitcher实现
 * @version: v1.0.1 2024-11-05 dou li yang
 */
#include "silly_huge_stitcher.h"
#define VSI_ROOT "/vsimem/tzx_huge_stitcher"
const int BAND_MAP[4] = {3, 2, 1, 0};

bool silly_huge_stitcher::attach(const block& blk)
{
    if (!m_init)
    {
        return false;
    }
    GUIntBig _data_size = blk.dIMG.data.size();
    GByte* _G_data = reinterpret_cast<GByte*>(const_cast<char*>(blk.dIMG.data.data()));
    VSIFCloseL(VSIFileFromMemBuffer(VSI_ROOT, _G_data, _data_size, FALSE));
    GDALDataset* tmp_dataset = (GDALDataset*)GDALOpen(VSI_ROOT, GA_ReadOnly);  // 打开栅格图像
    VSIUnlink(VSI_ROOT);
    if (!tmp_dataset)
    {
        return false;
    }
    {
        int width = tmp_dataset->GetRasterXSize();  // 图像宽度
        int height = tmp_dataset->GetRasterYSize();  // 图像高度
        int bands = tmp_dataset->GetRasterCount();   // 波段数（JPEG通常为3）
        if (width != blk.dIMG.width || height != blk.dIMG.height || bands != m_bands)
        {
            std::cerr << "信息不匹配" << std::endl;
            return false;
        }
    }
    int size = blk.xSize * blk.ySize * m_bands;
    std::vector<GByte> _G_buff(size);
    bool status = false;
    do
    {
        // RasterIO 本身包含自动缩放?
        // RasterIO (GDALRWFlag eRWFlag, int nXOff, int nYOff, int nXSize, int nYSize, void *pData,
        // int nBufXSize, int nBufYSize, GDALDataType eBufType, int nBandCount,
        // const int *BAND_MAP, GSpacing nPixelSpace, GSpacing nLineSpace, GSpacing nBandSpace, GDALRasterIOExtraArg *psExtraArg)

        // 从源图像中读取
        if (CPLErr::CE_None != tmp_dataset->RasterIO(GF_Read, blk.src.xOffset, blk.src.yOffset, blk.xSize, blk.ySize, _G_buff.data(), blk.xSize, blk.ySize, GDT_Byte, m_bands, (int*)BAND_MAP, m_bands, blk.xSize * m_bands, 1))
        {
            break;
        }
        if (CPLErr::CE_None != m_dateset->RasterIO(GF_Write, blk.dst.xOffset, blk.dst.yOffset, blk.xSize, blk.ySize, _G_buff.data(), blk.xSize, blk.ySize, GDT_Byte, m_bands, (int*)BAND_MAP, m_bands, blk.xSize * m_bands, 1))
        {
            break;
        }
        status = true;
    } while (0);
    GDALClose(tmp_dataset);
    tmp_dataset = nullptr;
    return status;
}

bool silly_huge_stitcher::create(const std::string& file, const int& nXSize, const int& nYSize, const int& nBands)
{
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (driver == nullptr)
    {
        SLOG_ERROR("先初始化GDAL环境,或者当前GDAL不支持GTiff处理");
        return false;
    }
    m_dateset = driver->Create(file.c_str(), nXSize, nYSize, nBands, GDT_Byte, nullptr);
    if (m_dateset)
    {
        m_init = true;
        m_width = nXSize;
        m_height = nYSize;
        m_bands = nBands;
        return true;
    }
    return false;
}

void silly_huge_stitcher::release()
{
    GDALClose(m_dateset);
    m_dateset = nullptr;
    m_bands = 0;
    m_width = 0;
    m_height = 0;
    m_init = false;
}