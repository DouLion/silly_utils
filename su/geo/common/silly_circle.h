/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_circle.cpp
 * @description: silly_circle 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_CIRCLE_H
#define SILLY_CIRCLE_H
#include <geo/common/silly_point.h>
class silly_circle
{
public:
    silly_circle() = default;
    ~silly_circle() = default;
    silly_circle(const silly_point& cp, const double& r) : center(cp), radius(r)
    {
    }

    double area() const
    {
        return SU_PI * radius * radius;
    }

public:
    silly_point center;
    double radius = 0.0;
};


#endif //SILLY_CIRCLE_H
