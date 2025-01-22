/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 13:46
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_MAPBOX_H
#define SILLY_UTILS_SILLY_MAPBOX_H
#include <mapbox/mbtiles/silly_mvt.h>

class silly_mapbox
{
  public:

    /// <summary>
    /// 多个地图同一块(zxy相同)的tile合并
    /// 图层名不可以重复
    /// Merge tile from tiles with same zxy.
    /// Refuse layers with same name.
    /// TODO: 后面需要考虑通图层名的合并 如河南的river 与 山东的river会有一部分重合
    /// </summary>
    /// <param name="tiles"></param>
    /// <returns></returns>
    static mvt_tile merge(const std::vector<mvt_tile>& tiles);
    
    /// <summary>
    /// 将other合并到out中
    /// </summary>
    /// <param name="other"></param>
    /// <param name="out"></param>
    /// <returns></returns>
    static bool merge(const mvt_tile& other, mvt_tile& out);
};

#endif  // SILLY_UTILS_SILLY_MAPBOX_H
