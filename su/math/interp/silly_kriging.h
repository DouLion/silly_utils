/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-03-13
 * @file: silly_kriging.h
 * @description: 克里金插值的一种实现
 * @version: v1.0.1 2025-03-13 dou li yang
 */
#ifndef SILLY_KRIGING_H
#define SILLY_KRIGING_H
#include <geo/silly_geo.h>

class OrdinaryKriging
{

    /**
     * 根据已知一维离散点,预测点的值
     * @param known 已知点集合
     * @param predict 待预测的点
     * @param nugget 块金值: 当两点距离趋近于 0 时，允许的最大误差
     * @param sill 基台值: 描述已知点集数值本身的方差,数据是否均匀
     * @param range 变程: 超过 range,两个点之间就不相关了
     * @return  克里金方差,0表示完全确定, 越大越不确定(->正无穷)
     */
    static double PredictedKriging(const std::vector<suPoint>& known, suPoint& predict, double nugget, double sill, double range);

};

#endif  // SILLY_KRIGING_H