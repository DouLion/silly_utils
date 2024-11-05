/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_map_layer.h
 * @description: silly_map_layer 类声明
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_MAP_LAYER_H
#define SILLY_UTILS_SILLY_MAP_LAYER_H

#include <su_marco.h>


class silly_map_layer
{
  public:

  static int32_t layer_no(const uint32_t& map_scale);
};

#endif  // SILLY_UTILS_SILLY_MAP_LAYER_H
