/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-11-24
 * @file: silly_idw.cpp
 * @description: 反距离插值
 * @version: v1.0.1 2025-11-24 dou li yang
 */
#ifndef SILLY_IDW_INTERP_H
#define SILLY_IDW_INTERP_H
#include <geo/silly_geo.h>

class suIDWInterp
{
public:
    ///
    /// @param x
    /// @param y
    /// @param ign 如果 ign . 0, 则忽略超过这个距离的点
    /// @return
    double GetZ(const double& x, const double& y, const double& ign = 0) const;

    void SetPoints(const std::vector<suPoint>&);

private:
    std::vector<suPoint> m_points;
};

#endif  // SILLY_IDW_INTERP_H
