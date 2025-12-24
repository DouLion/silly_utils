/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: ALGO_L.h
 * @description: 关系曲线的一些算法
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef ALGO_L_H
#define ALGO_L_H
#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>
class AlgoL
{
public:
    static double Calc(const double& x, const std::vector<double>& Xs, const std::vector<double>& Ys, const int& type = 0);
};

#endif  // ALGO_L_H
