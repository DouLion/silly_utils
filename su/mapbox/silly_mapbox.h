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

#ifndef SILLY_MAPBOX_H
#define SILLY_MAPBOX_H
#include <mapbox/mbtiles/silly_mvt.h>

class suMapBox
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
    static MVTTile Merge(const std::vector<MVTTile>& tiles);

    /**
     * 将src瓦片 合并到dst瓦片中
     * merge `src` tile into `dst` tile.
     * @param src 待合并的瓦片 tile to merge
     * @param dst 合并的结果, 本身可以是空的, 也可以有一些瓦片
     *            merge result, it could be empty, as well as contains some tiles
     * @return
     */
    static bool Merge(const MVTTile& src, MVTTile& dst);
};

#endif  // SILLY_MAPBOX_H
