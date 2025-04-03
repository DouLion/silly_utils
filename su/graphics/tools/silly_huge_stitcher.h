/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-05
 * @file: silly_huge_stitcher.h
 * @description:  超大图像拼接
 * @version: v1.0.1 2024-11-05 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_HUGE_STITCHER_H
#define SILLY_UTILS_SILLY_HUGE_STITCHER_H
#include <gdal_priv.h>
#include <gdal.h>
#include <cpl_conv.h>
#include <cpl_vsi.h>

#include <graphics/silly_png.h>

class silly_huge_stitcher
{
  public:
    struct pos
    {
        // 图像上的偏移位置
        int64_t xOffset = 0; 
        int64_t yOffset = 0;
    };
    struct img
    {
        std::string data;
        int64_t width = 0;
        int64_t height = 0;
    };
    struct block
    {
        pos src;  // 从data图像上截取的位置
        pos dst;  // 写入目标的位置
        // 裁剪区域的宽高
        int64_t xSize = 0; 
        int64_t ySize = 0;
        img dIMG;
    };

  public:
    /// <summary>
    ///
    /// </summary>
    /// <param name="file">输出文件路径</param>
    /// <param name="nXSize">宽</param>
    /// <param name="nYSize">高</param>
    /// <param name="nBands">PNG通道数</param>
    /// <returns></returns>
    bool create(const std::string& file, const int& nXSize, const int& nYSize, const int& nBands);

    /// <summary>
    /// 检查图像的宽高波段数
    /// </summary>
    /// <param name="data"></param>
    /// <param name="width"></param>
    /// <param name="heght"></param>
    /// <param name="bands"></param>
    /// <returns></returns>
    bool check(const std::string& data, int& width, int& height, int& bands);

    /// <summary>
    /// 矩形裁剪源图像,贴到目标图像上,不会做缩放
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool attach(const block& blk);

    /// <summary>
    /// 释放占用资源,需要手动调用
    /// </summary>
    void release();

  private:
    GDALDataset* m_dateset = nullptr;
    bool m_init = false;
    int m_bands = 0;
    int m_width = 0;
    int m_height = 0;
    std::mutex m_mutex;
};

#endif  // SILLY_UTILS_SILLY_HUGE_STITCHER_H

/* 用法示例1: 根据经纬范围,切割tile,拼接完整图像
 silly::geo::utils::init_gdal_env();
std::string file = R"(Z:\DATA\Dats\MapRoot\public\union_bitmap_pyramid\basemap\00_P)";
std::string png = R"(Z:\stitcher2.png)";
silly::pyramid::handle hdl;
hdl.begin_read(file);
// 29.9433,115.5065
// 28.9825,116.7450
// 109.736122881,125.512817797,21.825408231,33.637908231 [EPSG:4326]
uint8_t layer = 8;
silly_rect rect;
rect.min.x = 109.736122881;
rect.max.x = 125.512817797;
rect.min.y = 21.825408231;
rect.max.y = 33.637908231;
rect.correct();  // 自动调整最大最小值
size_t tilesize = 256;
screen_point lt = silly_tile::geo2screen(layer, {rect.min.x, rect.max.y});
screen_point rb = silly_tile::geo2screen(layer, {rect.max.x, rect.min.y});
size_t height = 0, width = 0;
silly_tile::size(lt, rb, width, height);
SLOG_INFO("宽:{}, 高:{}", width, height)
size_t brow = lt.pixY / tilesize, erow = rb.pixY / tilesize;
size_t bcol = lt.pixX / tilesize, ecol = rb.pixX / tilesize;
silly_huge_stitcher stitcher;
stitcher.create(png, width, height, 3);
int xB = lt.pixX % tilesize, yB = lt.pixY % tilesize;
for (auto r = brow; r <= erow; ++r)
{
    for (auto c = bcol; c <= ecol; ++c)
    {
        silly::pyramid::block blk;
        blk.zoom = layer;
        blk.row = r;
        blk.col = c;
        if (hdl.read(blk) && !blk.data.empty())
        {
            silly_huge_stitcher::block sb;
            // 需要根据位置计算截取目标的宽高,x方向和y方向的计算方式一致
            {
                int64_t tileX0 = c * tilesize, tileX1 = (c + 1) * tilesize;
                sb.xSize = SU_MIN(tileX1, rb.pixX) - SU_MAX(tileX0, lt.pixX);

                if (tileX0 < lt.pixX)
                {
                    sb.src.xOffset = xB;
                    sb.dst.xOffset = 0;
                }
                else
                {
                    sb.src.xOffset = 0;
                    sb.dst.xOffset = (tilesize - xB) + (c - bcol - 1) * tilesize;
                }
            }
            {
                int64_t tileY0 = r * tilesize, tileY1 = (r + 1) * tilesize;
                sb.ySize = SU_MIN(tileY1, rb.pixY) - SU_MAX(tileY0, lt.pixY);

                if (tileY0 < lt.pixY)
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
*/

/* 用法示例2,根据经纬范围,保留完整tile,拼接为大图像

silly::geo::utils::init_gdal_env();
std::string file = R"(Z:\DATA\Dats\MapRoot\public\union_bitmap_pyramid\basemap\00_P)";
std::string png = R"(Z:\stitcher.png)";
silly::pyramid::handle hdl;
hdl.begin_read(file);
// 29.9433,115.5065
// 28.9825,116.7450
uint8_t layer = 8;
silly_rect rect;
rect.min.x = 109.736122881;
rect.max.x = 125.512817797;
rect.min.y = 21.825408231;
rect.max.y = 33.637908231;
size_t tilesize = 256;
tile_index lt = silly_tile::geo2tile(layer, {rect.min.x, rect.max.y});
tile_index rb = silly_tile::geo2tile(layer, {rect.max.x, rect.min.y});
size_t height = 0, width = 0;
silly_tile::size(lt, rb, width, height);

silly_huge_stitcher stitcher;
stitcher.create(png, width, height, 3);
for (auto r = lt.row; r <= rb.row; ++r)
{
    for (auto c = lt.col; c <= rb.col; ++c)
    {
        silly::pyramid::block blk;
        blk.zoom = layer;
        blk.row = r;
        blk.col = c;
        if (hdl.read(blk) && !blk.data.empty())
        {
            silly_huge_stitcher::block sb;
            sb.dst.xOffset = (c - lt.col) * tilesize;
            sb.dst.yOffset = (r - lt.row) * tilesize;
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

*/
