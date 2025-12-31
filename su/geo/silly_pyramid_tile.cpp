/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-31
 * @file: silly_pyramid_tile
 * @description: silly_pyramid_tile实现
 * @version: v1.0.1 2025-12-31 dou li yang
 */
#include "silly_pyramid_tile.h"

suPoint suPyramidTile::LonLatToTilePos(const suPoint& llp, const int& zoom)
{
    suPoint ret;
    double lon = llp.x;
    double lat = llp.y;

    {// 确保经纬度在范围内
        const int TLat = std::fpclassify(lat);
        const int TLon = std::fpclassify(lon);
        bool bad_lon = false;

        if (TLat == FP_INFINITE || TLat == FP_NAN) {
            lat = 89.9;
        }

        if (TLon == FP_INFINITE || TLon == FP_NAN) {
            lon = 720.0;
            bad_lon = true;
        }
        // 限制纬度
        lat = std::clamp(lat, -89.9, 89.9);

        // 限制经度 (仅在不是坏的经度时)
        if (!bad_lon) {
            // 使用 std::clamp 使逻辑更清晰、更不易出错
            lon = std::clamp(lon, -360.0, 360.0);
        }
    }
    const auto n = static_cast<double>(1LL << zoom);

    const double mercator_lat = std::log(std::tan(MATH::PI_ / 4.0 + DEG2RAD(lat) / 2.0));
    const double mercator_y_term = 1.0 - (mercator_lat / MATH::PI_);

    // Y 坐标计算
    double tile_y = n * mercator_y_term / 2.0;
    // 使用 std::clamp，它比 if/else 更简洁、意图更明确
    tile_y = std::clamp(tile_y, 0.0, n - 1.0);

    // X 坐标计算
    const double scale_factor = 360.0 + std::pow(2.0, -24.0);
    double tile_x = n * ((lon + 180.0) / scale_factor);

    // 最终赋值
    ret.x = tile_x;
    ret.y = tile_y;
    return ret;
}

suRect suPyramidTile::CalcSceneBound(const suPoint& llp, const int& zoom, const int& sw, const int& sh, const int& tile_size)
{
    suRect ret;
    suPoint scp = LonLatToScenePos(llp, zoom, tile_size);
    ret.min.x = scp.x - sw / 2.0;
    ret.min.y = scp.y - sh / 2.0;
    ret.max.x = scp.x + sw / 2.0;
    ret.max.y = scp.y + sh / 2.0;
    return ret;
}