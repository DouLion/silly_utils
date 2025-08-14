/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/11/17 14:03
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_GEOTIFF_H
#define SILLY_UTILS_SILLY_GEOTIFF_H
#include <files/silly_file.h>
#include <metadata/silly_dem.h>


class sugeotiff : public sudem
{
public:
    struct header
    {
        uint32_t  width = 0;
        uint32_t  height = 0;
        uint16_t samples_per_pixel = 1;
        uint16_t bits_per_sample = 64;
        double tiepoints[6]= {};
        double pixscale[3] ={};
        // 见geovalues.h
        int model_type = 2; // 1 投影坐标系, 2 经纬坐标系 , 3地心(XYZ)坐标系
        int raster_type = 1; // 1. Standard pixel-fills-grid-cell 2 Pixel-at-grid-vertex
    };
public:

    bool write(const std::filesystem::path& outfile);

    std::string err() const;

private:
    header m_header;
    std::string m_err;
};

#endif  // SILLY_UTILS_SILLY_GEOTIFF_H
