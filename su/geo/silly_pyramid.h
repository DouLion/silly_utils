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
    uint64_t sr;  // 屏幕的行
    uint64_t sc;  // 屏幕的列
};

struct screen_rect
{
    uint8_t layer;
    screen_point max;
    screen_point min;
};

struct pyramid_index
{
    uint8_t layer;
    uint64_t col;
    uint64_t row;
};

class silly_google_pyramid
{
  public:
    /// <summary>
    /// 获取指定块的经纬范围
    /// </summary>
    /// <param name="l"></param>
    /// <param name="c"></param>
    /// <param name="r"></param>
    /// <returns></returns>
    static silly_rect get_rect(const pyramid_index& index);

    /// <summary>
    /// 坐标点在指定层所在的块信息
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="point"></param>
    /// <returns></returns>
    static pyramid_index get_index(const size_t& layer, const silly_point& point);
};

class silly_pyramid_proj
{
  public:
    /// <summary>
    /// 经纬度所在的块
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <returns></returns>
    static pyramid_index geo2pyramid(const uint8_t& layer, const silly_point& geo, const uint64_t& tilesize = 256);

    /// <summary>
    /// 瓦片所在的经纬度范围
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <returns></returns>
    static silly_rect pyramid2geo(const pyramid_index& index, const uint64_t& tilesize = 256);

    /// <summary>
    /// 经纬度对应的屏幕位置
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="nth2">瓦片宽高均为 2^nth2</param>
    /// <returns></returns>
    static screen_point geo2screen(const uint8_t& layer, const silly_point& geo, const uint64_t& tilesize = 256);

    /// <summary>
    /// 经纬度对应的屏幕位置
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="nth2">瓦片宽高均为 2^nth2</param>
    /// <returns></returns>
    static screen_rect geo2screen(const uint8_t& layer, const silly_rect& rect, const uint64_t& tilesize = 256);

    /// <summary>
    /// 屏幕位置对应的经纬度
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="nth2">瓦片宽高均为 2^nth2</param>
    /// <returns></returns>
    static silly_point screen2geo(const uint8_t& layer, const screen_point& point, const uint64_t& tilesize = 256);

    /// <summary>
    /// 屏幕位置对应的瓦片
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="nth2">瓦片宽高均为 2^nth2</param>
    /// <returns></returns>
    static pyramid_index screen2pyramid(const uint8_t& layer, const screen_point& point, const uint64_t& tilesize = 256);


    /// <summary>
    /// 瓦片索引对应的屏幕范围
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="geo"></param>
    /// <param name="nth2">瓦片宽高均为 2^nth2</param>
    /// <returns></returns>
    static screen_rect pyramid2screen(const pyramid_index& index, const uint64_t& tilesize = 256);
};

#endif  // SILLY_UTILS_SILLY_PYRAMID_H
