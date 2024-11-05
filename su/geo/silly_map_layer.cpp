/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_map_layer.cpp
 * @description: silly_map_layer实现
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#include "silly_map_layer.h"
// 层级对应的地图比例尺
std::map<uint32_t, uint32_t> SILLY_LAYER_MAP_SCALE = {
    {0, 500000000},
    {1, 300000000},
    {2, 200000000},
    {3, 100000000},
    {4, 50000000},
    {5, 30000000},
    {6, 20000000},
    {7, 10000000},
    {8, 5000000},
    {9, 2000000},
    {10, 1000000},
    {11, 500000},
    {12, 250000},
    {13, 150000},
    {14, 100000},
    {15, 50000},
    {16, 25000},
    {17, 10000},
    {18, 5000},
    {19, 2000},
    {20, 1000}
};
int32_t silly_map_layer::layer_no(const uint32_t& map_scale)
{
    int32_t ret = 0;
    for (auto& [no, scale] : SILLY_LAYER_MAP_SCALE)
    {
        if (map_scale > scale)
        {
            break;
        }
        ret = no;

    }
    return ret;
}
