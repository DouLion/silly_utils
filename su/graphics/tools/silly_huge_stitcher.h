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
        int xOffset = 0;
        int yOffset = 0;
    };
    struct img
    {
        std::string data;
        int width = 0;
        int height = 0;
    };
    struct block
    {
        pos src;  // 从data图像上截取的位置
        pos dst;  // 写入目标的位置
        int xSize = 0; 
        int ySize = 0;
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
};

#endif  // SILLY_UTILS_SILLY_HUGE_STITCHER_H

/* 用法示例

silly::geo::utils::init_gdal_env();
std::string file = R"(Z:\DATA\Dats\MapRoot\public\union_bitmap_pyramid\basemap\00_P)";
std::string png = R"(Z:\stitcher.png)";
silly::pyramid::handle hdl;
hdl.begin_read(file);
// 29.9433,115.5065
// 28.9825,116.7450
uint8_t layer = 10;
silly_rect rect;
rect.min.x = 115.5065;
rect.max.x = 116.7450;
rect.min.y = 28.9825;
rect.max.y = 29.9433;
size_t tilesize = 256;
pyramid_index lt = silly_pyramid_proj::geo2pyramid(layer, {rect.min.x, rect.max.y});
pyramid_index rb = silly_pyramid_proj::geo2pyramid(layer, {rect.max.x, rect.min.y});
size_t height=0, width = 0;
silly_pyramid_proj::size(lt, rb, width, height);

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
            sb.xSize = tilesize;
            sb.ySize = tilesize;
            stitcher.attach(sb);
            int a = 0;
        }
    }
}
stitcher.release();
hdl.close();

*/
