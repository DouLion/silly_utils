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
#include <map>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <numeric>
class AlgoL
{
public:
    static double Calc(const double& x, const std::vector<double>& Xs, const std::vector<double>& Ys, const int& type = 0);

  /**
   * @brief 线性插值,获取x对应的y值
   * @param x
   * @param x0
   * @param x1
   * @param y0
   * @param y1
   * @return
   */
    static double LineInterp(const double& x, const double& x0, const double& x1, const double& y0, const double& y1);
};

#endif  // ALGO_L_H
