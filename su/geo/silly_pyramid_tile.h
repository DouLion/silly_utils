/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-31
 * @file: silly_pyramid_tile.h
 * @description: 金字塔瓦片相关的处理
 * @version: v1.0.1 2025-12-31 dou li yang
 */
#ifndef SILLY_PYRAMID_TILE_H
#define SILLY_PYRAMID_TILE_H
#include <tiff.h>
#include <geo/silly_geo.h>
constexpr int PYRAMID_TILE_SIZE = 256; // 瓦片的默认大小
constexpr double MAX_LAT = 85.05113; // 墨卡托 投影的纬度范围
constexpr int TILE_EXTEND_SIZE = 4096;
using PyXYZ = std::tuple<uint32_t, uint32_t, uint8_t>;
using PyXY = std::tuple<uint32_t, uint32_t>;
class suPyramidTile
{
    /**
     * @brief 采用谷歌的金字塔坐标, 左上角为原点(0, 0)
     */
public:
    /**
     * @brief: 经纬度转 浮点的xyz,
     * @param[in ]: llp 经纬度坐标
     * @param[in ]: z 金字塔层级
     * @return 瓦片坐标 在(0,0) 到 (2^z, 2^z) 之间
     */
    static suPoint LonLatToFXYZ(const suPoint& llp, const int& z)
    {
        double lon = llp.x;
        double lat = std::clamp(llp.y, -MAX_LAT, MAX_LAT); // 防止极点溢出

        double n = static_cast<double>(1 << z); // 2^z

        // X: 经度线性映射
        double x = (lon + 180.0) / 360.0 * n;

        // Y: Web Mercator 投影
        double latRad = lat * MATH::PI_ / 180.0;
        double y = (1.0 - std::log(std::tan(latRad) + 1.0 / std::cos(latRad)) / MATH::PI_) / 2.0 * n;

        return suPoint{x, y};
    }

    /**
     * 浮点的xyz 转经纬度
     * @param xy
     * @param z
     * @return
     */
    static suPoint FXYZToLonLat(const suPoint& xy, const int& z)
    {
        // 注意：xy 是 (0~2^z) 范围，需先归一化到 [0,1]
        double n = static_cast<double>(1 << z);
        double mx = xy.x / n;   // ∈ [0, 1]
        double my = xy.y / n;   // ∈ [0, 1]

        double lon = mx * 360.0 - 180.0;

        // 稳定反解 Web Mercator
        double lat = 90.0 - 360.0 * std::atan(std::exp((my - 0.5) * 2.0 * MATH::PI_)) / MATH::PI_;

        return suPoint{lon, lat};
    }

    /**
     * @brief 经纬度在整个场景内的坐标
     * @param[in ]: lon 经度
     * @param[in ]: lat 纬度
     * @param[in ]: zoom 金字塔层级
     * @param[out]: x 瓦片坐标x
     * @param[out]: y 瓦片坐标y
     * @param[in ]: tile_size
     */
    static suPoint LonLatToScenePos(const suPoint& llp, const int& zoom, const int& tile_size= PYRAMID_TILE_SIZE)
    {
        suPoint ret = LonLatToFXYZ(llp, zoom);
        ret.x *= tile_size;
        ret.y *= tile_size;
        return ret;
    }

    static suPoint LonLat2TileO(const suPoint& llp, const int& zoom)
    {
        suPoint tile0 = LonLatToFXYZ(llp, zoom);
        tile0.x = std::floor(tile0.x);
        tile0.y = std::floor(tile0.y);
        return tile0;

    }


    /**
     * 场景坐标 转 经纬度
     * @param sp 场景坐标
     * @param zoom 金字塔层级
     * @param tile_size
     * @return
     */
    static suPoint ScenePosToLonLat(const suPoint& sp, const int& zoom, const int& tile_size= PYRAMID_TILE_SIZE)
    {
        double mx = sp.x/ tile_size;
        double my = sp.y/ tile_size;
        return FXYZToLonLat({mx, my}, zoom);
    }

    /**
     * @brief: 瓦片的在常见中的原点坐标
     * @param[in ]: llp 经纬度坐标
     * @param[in ]: zoom 金字塔层级
     * @param[in ]: tile_size
     * @return 瓦片的原点坐标
     */
    static suPoint TileOriginOnScene(const suPoint& llp, const int& zoom, const int& tile_size= PYRAMID_TILE_SIZE)
    {
        suPoint ret = LonLatToFXYZ(llp, zoom);
        ret.x = std::floor(ret.x);
        ret.y = std::floor(ret.y);
        ret.x *= tile_size;
        ret.y *= tile_size;
        return ret;
    }

    /**
     * @brief: 以llp为中心的视口边界
     * @param[in ]: llp 经纬度坐标
     * @param[in ]: zoom 金字塔层级
     * @param[in ]: vpw 视口像素宽度
     * @param[in ]: vph 视口像素高度
     * @param[in ]: tile_size 瓦片大小
     * @return 场景边界
     */
    static suRect CenterLL2ViewPort(const suPoint& llp, const int& zoom, const int& vpw, const int& vph, const int& tile_size= PYRAMID_TILE_SIZE)
    {
        suRect ret;
        suPoint scp = LonLatToScenePos(llp, zoom, tile_size);
        ret.min.x = scp.x - vpw / 2.0;
        ret.min.y = scp.y - vph / 2.0;
        ret.max.x = scp.x + vpw / 2.0;
        ret.max.y = scp.y + vph / 2.0;
        return ret;
    }

    /**
     * @brief: 获取视口中心经纬度
     * @param[in ]: svp 场景边界
     * @param[in ]: zoom 金字塔层级
     * @return 场景中心经纬度
     */
    static suPoint ViewPort2CenterLL(const suRect& svp, const int& zoom, const int& tile_size= PYRAMID_TILE_SIZE)
    {
        suPoint center((svp.max.x + svp.min.x)/2.0, (svp.max.y + svp.min.y)/2.0);
        return ScenePosToLonLat(center, zoom, tile_size);
    }

    static void Verify(const suPoint& llp = {116.4074, 39.9042}, const int& zoom= 12, const int& vw = 1920, const int& vh = 1080, const int& tile_size= PYRAMID_TILE_SIZE);

};


#endif //SILLY_PYRAMID_TILE_H
