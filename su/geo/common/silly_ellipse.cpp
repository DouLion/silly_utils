/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suEllipse.c
 * @description: suEllipse实现
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#include "silly_ellipse.h"
suEllipse::suEllipse(const suPoint& p1, const suPoint& p2, const double& longAxis)
{
    f1 = p1;
    f2 = p2;
    majorAxis = longAxis;


    m_FocalDist = 0.0;

}
bool suEllipse::valid() const
{
    // 检查长轴长度是否为正
   if (majorAxis <= 0)
   {
       return false;
   }

    // 计算焦距
    double centerX = (f1.x + f2.x) / 2.0;
    double centerY = (f1.y + f2.y) / 2.0;
    double focalDistance = std::sqrt(std::pow(f1.x - centerX, 2) + std::pow(f1.y - centerY, 2));
    double distanceBetweenFoci = 2 * focalDistance;

    // 椭圆有效性条件：长轴长度 > 两焦点间距离
    return majorAxis > distanceBetweenFoci;
}
double suEllipse::area() const
{
    // 计算半长轴 a（长轴的一半）
    double a = majorAxis / 2.0;

    // 计算焦距 c（焦点到中心的距离）
    double centerX = (f1.x + f2.x) / 2.0;
    double centerY = (f1.y + f2.y) / 2.0;
    double c = std::sqrt(std::pow(f1.x - centerX, 2) + std::pow(f1.y - centerY, 2));

    // 计算半短轴 b
    double b = std::sqrt(std::pow(a, 2) - std::pow(c, 2));

    // 计算面积
    return MATH::PI_ * a * b;
}
double suEllipse::circumference() const
{
    // 计算半长轴 a 和焦距 c
    double a = majorAxis / 2.0;
    double centerX = (f1.x + f2.x) / 2.0;
    double centerY = (f1.y + f2.y) / 2.0;
    double c = std::sqrt(std::pow(f1.x - centerX, 2) + std::pow(f1.y - centerY, 2));

    // 计算半短轴 b
    double b = std::sqrt(std::pow(a, 2) - std::pow(c, 2));

    // 确保 a >= b（数学约束）
    if (a < b) {
        std::swap(a, b);
    }

    // 方法1：拉马努金近似公式2（高精度）
    double h = std::pow((a - b) / (a + b), 2);
    double C = MATH::PI_ * (a + b) * (1 + (3 * h) / (10 + std::sqrt(4 - 3 * h)));
    return C;
}

double suEllipse::MinorAxis()
{
    return m_MinorAxis;
}
double suEllipse::FocalDist()
{
    if (m_FocalDist == 0.0)
    {
        double a = majorAxis / 2.0;
        double centerX = (f1.x + f2.x) / 2.0;
        double centerY = (f1.y + f2.y) / 2.0;
        m_FocalDist = std::sqrt(std::pow(f1.x - centerX, 2) + std::pow(f1.y - centerY, 2));
    }
    return m_FocalDist;
}