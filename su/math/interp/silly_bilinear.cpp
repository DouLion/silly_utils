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

 double suBilinearInterp::calc(const double& q00, const double& q01, const double& q10, const double& q11, const double& dx, const double& dy)
{
    // 计算权重
    const double& alpha = dx;  // x 方向的权重
    const double& beta = dy;   // y 方向的权重

    // 双线性插值公式
    return (1 - alpha) * (1 - beta) * q00 + alpha * (1 - beta) * q01 + (1 - alpha) * beta * q10 + alpha * beta * q11;
}