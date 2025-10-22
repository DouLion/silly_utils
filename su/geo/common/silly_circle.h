/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suCircle.cpp
 * @description: suCircle 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_CIRCLE_H
#define SILLY_CIRCLE_H
#include <geo/common/silly_point.h>
class suCircle
{
  public:
    suCircle() = default;
    ~suCircle() = default;
    suCircle(const suPoint& cp, const double& r) : center(cp), radius(r)
    {
    }

    double area() const
    {
        return SU_PI * radius * radius;
    }

  public:
    suPoint center;
    double radius = 0.0;
};

#endif  // SILLY_CIRCLE_H
