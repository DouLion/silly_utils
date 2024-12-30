/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-30
 * @file: silly_bilinear.cpp
 * @description: silly_bilinear实现
 * @version: v1.0.1 2024-12-30 dou li yang
 */
#include "silly_bilinear.h"
using namespace silly::interpolation;

bilinear::calc(double q0, double q1, double q2, double q3, double dx, double dy)
{
    // 计算权重
    double alpha = dx;  // x 方向的权重
    double beta = dy;   // y 方向的权重

    // 双线性插值公式
    return (1 - alpha) * (1 - beta) * q0 + alpha * (1 - beta) * q1 + (1 - alpha) * beta * q2 + alpha * beta * q3;
}