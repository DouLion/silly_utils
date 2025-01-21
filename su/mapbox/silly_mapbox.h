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
    class merge_opt
    {
        std::map<std::string, std::string> renames;
    };

    /// 合并同一块的tile
    static mvt_tile merge(const std::vector<mvt_tile>& tiles);
};

#endif  // SILLY_UTILS_SILLY_MAPBOX_H
