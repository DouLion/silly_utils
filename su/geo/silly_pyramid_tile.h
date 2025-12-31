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
#include <geo/proj/silly_proj.h>
#include <geo/silly_geo.h>
constexpr int PYRAMID_TILE_SIZE = 256; // 瓦片的默认大小
class suPyramidTile
{
public:
    /**
     * @brief: 经纬度转瓦片坐标,仅确定是哪一个瓦片
     * @param[in ]: llp 经纬度坐标
     * @param[in ]: zoom 缩放级别
     * @return 瓦片坐标
     */
    static suPoint LonLatToTilePos(const suPoint& llp, const int& zoom);

    /**
     * @brief 经纬度在整个场景内的坐标
     * @param[in ]: lon 经度
     * @param[in ]: lat 纬度
     * @param[in ]: zoom 缩放级别
     * @param[out]: x 瓦片坐标x
     * @param[out]: y 瓦片坐标y
     * @param[in ]: tile_size
     */
    static suPoint LonLatToScenePos(const suPoint& llp, const int& zoom, const int& tile_size= PYRAMID_TILE_SIZE)
    {
        suPoint ret = LonLatToTilePos(llp, zoom);
        ret.x *= tile_size;
        ret.y *= tile_size;
        return ret;
    }

    /**
     * @brief: 以llp为中心的场景边界
     * @param[in ]: llp 经纬度坐标
     * @param[in ]: zoom 缩放级别
     * @param[in ]: sw 场景宽度
     * @param[in ]: sh 场景高度
     * @param[in ]: tile_size 瓦片大小
     * @return 场景边界
     */
    static suRect CalcSceneBound(const suPoint& llp, const int& zoom, const int& sw, const int& sh, const int& tile_size= PYRAMID_TILE_SIZE);


    /// 墨卡托和经纬度互转
    static void MercatorToLonLat(const double& mx, const double& my, double &lon, double &lat);
    static void LonLatToMercator(const double& lon, const double& lat, double &mx, double &my);
};


inline void suPyramidTile::MercatorToLonLat(const double& mx, const double& my, double &lon, double &lat)
{
    return MERCATOR2LONLAT(mx, my, lon, lat);
}
inline void suPyramidTile::LonLatToMercator(const double& lon, const double& lat, double &mx, double &my)
{
    return LONLAT2MERCATOR(lon, lat, mx, my);
}

#endif //SILLY_PYRAMID_TILE_H
