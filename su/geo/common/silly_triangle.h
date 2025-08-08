/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_triangle.cpp
 * @description: silly_triangle 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_TRIANGLE_H
#define SILLY_TRIANGLE_H
#include <geo/common/silly_point.h>

class silly_triangle
{
  public:
    silly_triangle() = default;
    ~silly_triangle() = default;
    silly_triangle(silly_point a, silly_point b, silly_point c) : a(a), b(b), c(c)
    {
    }

    /// 有向面积
    double oriented_area() const
    {
        return (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2;
    }

    double area() const
    {
        return std::abs(oriented_area());
    }

    /// 外接圆
    silly_circle circumscribed_circle() const
    {
        // 计算外接圆的中心
        double A = a.x * a.x + a.y * a.y;
        double B = b.x * b.x + b.y * b.y;
        double C = c.x * c.x + c.y * c.y;
        double D = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);

        silly_circle circle;
        if (D == 0)
        {
            // 如果D为0，则无法计算外接圆
            throw std::runtime_error("无法计算外接圆");
        }

        circle.center.x = (a.x * (B - C) + b.x * (C - A) + c.x * (A - B)) / (2 * D);
        circle.center.y = (a.y * (B - C) + b.y * (C - A) + c.y * (A - B)) / (2 * D);

        // 计算半径
        circle.radius = sqrt((circle.center.x - a.x) * (circle.center.x - a.x) + (circle.center.y - a.y) * (circle.center.y - a.y));

        return circle;
    }

    /// 顶点a的角度
    double angle_a() const
    {
        return std::acos((b.x - a.x) * (c.x - a.x) + (b.y - a.y) * (c.y - a.y)) * 180 / silly::math::pi;
    }

    /// 顶点b的角度
    double angle_b() const
    {
        return std::acos((a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y)) * 180 / silly::math::pi;
    }

    /// 顶点c的角度
    double angle_c() const
    {
        return std::acos((a.x - c.x) * (b.x - c.x) + (a.y - c.y) * (b.y - c.y)) * 180 / silly::math::pi;
    }

  public:
    silly_point a;
    silly_point b;
    silly_point c;
};

#endif  // SILLY_TRIANGLE_H
