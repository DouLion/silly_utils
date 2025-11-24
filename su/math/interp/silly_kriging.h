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
#ifndef SILLY_KRIGING_INTERP_H
#define SILLY_KRIGING_INTERP_H
#include <geo/silly_geo.h>
class suKrigingInterp
{
public:
    double GetZ(const double& x, const double& y) const;

    void SetPoints(const std::vector<suPoint>& points);

private:
    std::vector<suPoint> m_points;
};

#endif  // SILLY_KRIGING_INTERP_H
