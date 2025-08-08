/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_ellipse.cpp
 * @description: silly_ellipse 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_ELLIPSE_H
#define SILLY_ELLIPSE_H
#include <geo/common/silly_point.h>

class silly_ellipse
{
public:
    silly_ellipse() = default;
    ~silly_ellipse() = default;
    silly_ellipse(silly_point p, double x, double y) : center(p), rx(x), ry(y)
    {
    }

    double area() const
    {
        return silly::math::pi * rx * ry;
    }

    /// 周长
    /// Srinivasa Ramanujan 提出的两个近似公式, 1会更准确
    double circumference(const int& Ramanujan = 1) const
    {
        if (Ramanujan == 1)
        {
            return silly::math::pi * (3 * (rx + ry) - sqrt((3 * rx + ry) * (rx + 3 * ry)));
        }

        return 2 * silly::math::pi * sqrt((rx * rx + ry * ry) / 2);
    }

public:
    silly_point center;  // 中心点
    // 长半轴 和 短半轴, 不分
    double rx = 0.0;
    double ry = 0.0;
};


#endif //SILLY_ELLIPSE_H
