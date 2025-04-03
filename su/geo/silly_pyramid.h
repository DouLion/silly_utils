/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/11 18:25
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PYRAMID_H
#define SILLY_UTILS_SILLY_PYRAMID_H
#include "geo/silly_geo.h"

#define SILLY_GLOBAL_LEFT -180.0
#define SILLY_GLOBAL_RIGHT 180.0
#define SILLY_GLOBAL_TOP 90.0
#define SILLY_GLOBAL_BOTTOM -90.0

/**
           90

 -180                 180

           -90

**/

struct screen_point
{
    uint8_t layer;
    int64_t pixX;   // 屏幕的列, 像素位置
    int64_t pixY;  // 屏幕的行, 像素位置
};

struct screen_rect
{
    uint8_t layer;
    screen_point max;
    screen_point min;
};

struct tile_index
{
    uint8_t layer;
    uint64_t col;
    uint64_t row;
};

class silly_tile
{
  public:

    /// <summary>
    /// 根据tile块索引计算宽高, [min, max]
    /// </summary>
    /// <param name="pi1"></param>
    /// <param name="pi2"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <param name="tilesize"></param>
    static void size(const tile_index& pi1, const tile_index& pi2, size_t& width, size_t& height, const uint64_t& tilesize = 256);


    /// <summary>
    /// 根据屏幕像素计算宽高
    /// </summary>
    /// <param name="sp1"></param>
    /// <param name="sp2"></param>
    /// <param name="width"></param>
    /// <param name="height"></param>
    static void size(const screen_point& sp1, const screen_point& sp2, size_t& width, size_t& height);

    /// <summary>
    /// 经纬度所在的块
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <returns></returns>
    static tile_index geo2tile(const uint8_t& layer, const silly_point& geo, const uint64_t& tilesize = 256);

    /// <summary>
    /// 瓦片所在的经纬度范围
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <returns></returns>
    static silly_rect tile2geo(const tile_index& index, const uint64_t& tilesize = 256);

    /// <summary>
    /// 经纬度对应的屏幕位置
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="tilesize">瓦片宽高均为 tilesize</param>
    /// <returns></returns>
    static screen_point geo2screen(const uint8_t& layer, const silly_point& geo, const uint64_t& tilesize = 256);

    /// <summary>
    /// 经纬度对应的屏幕位置
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="tilesize">瓦片宽高均为 tilesize</param>
    /// <returns></returns>
    static screen_rect geo2screen(const uint8_t& layer, const silly_rect& rect, const uint64_t& tilesize = 256);

    /// <summary>
    /// 屏幕位置对应的经纬度
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="tilesize">瓦片宽高均为 tilesize</param>
    /// <returns></returns>
    static silly_point screen2geo(const uint8_t& layer, const screen_point& point, const uint64_t& tilesize = 256);

    /// <summary>
    /// 屏幕位置对应的瓦片
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="tilesize">瓦片宽高均为 tilesize</param>
    /// <returns></returns>
    static tile_index screen2tile(const uint8_t& layer, const screen_point& point, const uint64_t& tilesize = 256);


    /// <summary>
    /// 瓦片索引对应的屏幕范围
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="tilesize">瓦片宽高均为 tilesize</param>
    /// <returns></returns>
    static screen_rect tile2screen(const tile_index& index, const uint64_t& tilesize = 256);
};

#endif  // SILLY_UTILS_SILLY_PYRAMID_H
