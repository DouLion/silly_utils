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
#include <log/silly_log.h>
#include <files/silly_file.h>
#define VSI_ROOT "/vsimem/tzx_huge_stitcher"
constexpr int BAND_MAP[4] = {3, 2, 1, 0};

bool suHugeStitcher::check(const std::string& data, int& width, int& height, int& bands)
{
#if SU_THIRD_SUPPORT_GDAL
    GDALDataset* tmp_dataset = nullptr;
    width = 0;
    height = 0;
    bands = 0; {
        std::scoped_lock loc(m_mutex);
        GUIntBig _data_size = data.size();
        GByte* _G_data = reinterpret_cast<GByte*>(const_cast<char*>(data.data()));
        VSIFCloseL(VSIFileFromMemBuffer(VSI_ROOT, _G_data, _data_size, FALSE));
        tmp_dataset = (GDALDataset*)GDALOpen(VSI_ROOT, GA_ReadOnly); // 打开栅格图像
        VSIUnlink(VSI_ROOT);
    }
    if (tmp_dataset)
    {
        width = tmp_dataset->GetRasterXSize();  // 图像宽度
        height = tmp_dataset->GetRasterYSize(); // 图像高度
        bands = tmp_dataset->GetRasterCount();  // 波段数（JPEG通常为3）
        GDALClose(tmp_dataset);
        return true;
    }
#endif
    return false;
}

bool suHugeStitcher::attach(const Block& blk)
{
    bool status = false;
    if (!m_init)
    {
        return status;
    }
#if SU_THIRD_SUPPORT_GDAL
    GDALDataset* tmp_dataset = nullptr; {
        std::scoped_lock loc(m_mutex);
        GUIntBig _data_size = blk.dIMG.data.size();
        GByte* _G_data = reinterpret_cast<GByte*>(const_cast<char*>(blk.dIMG.data.data()));
        VSIFCloseL(VSIFileFromMemBuffer(VSI_ROOT, _G_data, _data_size, FALSE));
        tmp_dataset = (GDALDataset*)GDALOpen(VSI_ROOT, GA_ReadOnly); // 打开栅格图像
        VSIUnlink(VSI_ROOT);
    }

    if (!tmp_dataset)
    {
        return false;
    }

    do
    {
        {
            // 判断基本信息是否一致
            int width = tmp_dataset->GetRasterXSize();  // 图像宽度
            int height = tmp_dataset->GetRasterYSize(); // 图像高度
            int bands = tmp_dataset->GetRasterCount();  // 波段数（JPEG通常为3）
            if (width != blk.dIMG.width || height != blk.dIMG.height || bands != m_bands)
            {
                std::cerr << "信息不匹配" << std::endl;
                break;
            }
        }
        int size = blk.xSize * blk.ySize * m_bands;
        std::vector<GByte> _G_buff(size);
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
#endif
    return status;
}

bool suHugeStitcher::create(const suPath& file, const int& nXSize, const int& nYSize, const int& nBands)
{
#if SU_THIRD_SUPPORT_GDAL
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if (driver == nullptr)
    {
        SLOG_ERROR("先初始化GDAL环境,或者当前GDAL不支持GTiff处理");
        return false;
    }
    m_dateset = driver->Create(suPath(file).string().c_str(), nXSize, nYSize, nBands, GDT_Byte, nullptr);
    if (m_dateset)
    {
        m_init = true;
        m_width = nXSize;
        m_height = nYSize;
        m_bands = nBands;
        return true;
    }
#endif
    return false;
}

void suHugeStitcher::release()
{
#if SU_THIRD_SUPPORT_GDAL
    GDALClose(m_dateset);
    m_dateset = nullptr;
    m_bands = 0;
    m_width = 0;
    m_height = 0;
    m_init = false;
#endif
}

#ifndef NDEBUG
#include <tzx/pyramid/silly_pyramid_handle.h>
#include <geo/silly_pyramid_tile.h>

void TileSize(const suPoint& pi1, const suPoint& pi2, size_t& width, size_t& height, const uint64_t& tilesize = 256)
{
    int64_t rows = std::abs(static_cast<int64_t>(pi1.y) - static_cast<int64_t>(pi2.y)) + 1;
    int64_t cols = std::abs(static_cast<int64_t>(pi1.x) - static_cast<int64_t>(pi2.x)) + 1;
    width = static_cast<size_t>(cols) * tilesize;
    height = static_cast<size_t>(rows) * tilesize;
}

void suHugeStitcher::test_case1()
{
    suGeoUtils::init_gdal_env();
    supath file = R"(\\192.168.0.80\MapRoot\public\union_bitmap_pyramid\basemap\00_P)";
    supath png = R"(./stitcher1.png)";
    TzxPyramidHandle hdl;
    hdl.begin_read(file);
    // 29.9433,115.5065
    // 28.9825,116.7450
    // 109.736122881,125.512817797,21.825408231,33.637908231 [EPSG:4326]
    uint8_t layer = 8;
    suRect rect;
    rect.min.x = 109.736122881;
    rect.max.x = 125.512817797;
    rect.min.y = 21.825408231;
    rect.max.y = 33.637908231;
    rect.correct(); // 自动调整最大最小值
    size_t tilesize = 256;
    suPoint lt = suPyramidTile::LonLatToFXYZ({rect.min.x, rect.max.y}, layer);
    suPoint rb = suPyramidTile::LonLatToFXYZ({rect.max.x, rect.min.y}, layer);
    size_t height = 0, width = 0;
    TileSize(lt, rb, width, height);
    SLOG_INFO("宽:{}, 高:{}", width, height)
    size_t brow = lt.y / tilesize, erow = rb.y / tilesize;
    size_t bcol = lt.x / tilesize, ecol = rb.x / tilesize;
    suHugeStitcher stitcher;
    stitcher.create(png, width, height, 3);
    int xB = static_cast<int64_t>(lt.x) % tilesize, yB = static_cast<int64_t>(lt.y) % tilesize;
    for (auto r = brow; r <= erow; ++r)
    {
        for (auto c = bcol; c <= ecol; ++c)
        {
            TzxPyramidBlock blk;
            blk.zoom = layer;
            blk.row = r;
            blk.col = c;
            if (hdl.read(blk) && !blk.data.empty())
            {
                suHugeStitcher::Block sb;
                // 需要根据位置计算截取目标的宽高,x方向和y方向的计算方式一致
                {
                    int64_t tileX0 = c * tilesize, tileX1 = (c + 1) * tilesize;
                    sb.xSize = SU_MIN(tileX1, rb.x) - SU_MAX(tileX0, lt.x);

                    if (tileX0 < lt.x)
                    {
                        sb.src.xOffset = xB;
                        sb.dst.xOffset = 0;
                    }
                    else
                    {
                        sb.src.xOffset = 0;
                        sb.dst.xOffset = (tilesize - xB) + (c - bcol - 1) * tilesize;
                    }
                } {
                    int64_t tileY0 = r * tilesize, tileY1 = (r + 1) * tilesize;
                    sb.ySize = SU_MIN(tileY1, rb.y) - SU_MAX(tileY0, lt.y);

                    if (tileY0 < lt.y)
                    {
                        sb.src.yOffset = yB;
                        sb.dst.yOffset = 0;
                    }
                    else
                    {
                        sb.src.yOffset = 0;
                        sb.dst.yOffset = (tilesize - yB) + (r - brow - 1) * tilesize;
                    }
                }
                sb.dIMG.data = blk.data;
                sb.dIMG.width = tilesize;
                sb.dIMG.height = tilesize;
                if (!stitcher.attach(sb))
                {
                    SLOG_ERROR("{},{}", r, c)
                    std::cout << sb.src.xOffset << std::endl;
                    std::cout << sb.xSize << std::endl;
                    std::cout << sb.dst.xOffset << std::endl;
                }
            }
        }
    }
    stitcher.release();
    hdl.close();
}

void suHugeStitcher::test_case2()
{
    suGeoUtils::init_gdal_env();
    supath file = R"(\\192.168.0.80\MapRoot\public\union_bitmap_pyramid\basemap\00_P)";
    supath png = R"(./stitcher2.png)";
    TzxPyramidHandle hdl;
    hdl.begin_read(file);
    // 29.9433,115.5065
    // 28.9825,116.7450
    uint8_t layer = 8;
    suRect rect;
    rect.min.x = 109.736122881;
    rect.max.x = 125.512817797;
    rect.min.y = 21.825408231;
    rect.max.y = 33.637908231;
    size_t tilesize = 256;
    suPoint lt = suPyramidTile::LonLatToFXYZ({rect.min.x, rect.max.y}, layer);
    suPoint rb = suPyramidTile::LonLatToFXYZ({rect.max.x, rect.min.y}, layer);
    size_t height = 0, width = 0;
    TileSize(lt, rb, width, height);

    suHugeStitcher stitcher;
    stitcher.create(png, width, height, 3);
    for (auto r = static_cast<int64_t>(lt.y); r <= static_cast<int64_t>(rb.y); ++r)
    {
        for (auto c = static_cast<int64_t>(lt.x); c <= static_cast<int64_t>(rb.x); ++c)
        {
            TzxPyramidBlock blk;
            blk.zoom = layer;
            blk.row = r;
            blk.col = c;
            if (hdl.read(blk) && !blk.data.empty())
            {
                suHugeStitcher::Block sb;
                sb.dst.xOffset = (c - static_cast<int64_t>(lt.x)) * tilesize;
                sb.dst.yOffset = (r - static_cast<int64_t>(lt.y)) * tilesize;
                sb.dIMG.data = blk.data;
                sb.dIMG.width = tilesize;
                sb.dIMG.height = tilesize;
                sb.src.xOffset = 1;
                sb.src.yOffset = 1;
                sb.xSize = tilesize - 1;
                sb.ySize = tilesize - 1;
                stitcher.attach(sb);
                int a = 0;
            }
        }
    }
    stitcher.release();
    hdl.close();
}

#endif