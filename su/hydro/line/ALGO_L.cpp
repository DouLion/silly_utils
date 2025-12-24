/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: ALGO_L
 * @description: ALGO_L实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "ALGO_L.h"
double AlgoL::Calc(const double& x, const std::vector<double>& Xs, const std::vector<double>& Ys, const int& type)
{
    if (Xs.empty() || Ys.empty() || Xs.size() != Ys.size())
    {
        return 0;
    }
    const int num = Xs.size();
    if (num == 1)
    {
        return Ys[0];
    }

    if (x <= Xs.front())  // 左边界处理
    {
        if (type == 3 && x < Xs[0])
        {
            return 0;
        }
        if (type == 1 && std::abs(Xs[0] - Xs[1]) > 1e-10)  // 在延长线上找到插值位置
        {
            const double x0 = Xs[0];
            const double x1 = Xs[1];
            const double y0 = Ys[0];
            const double y1 = Ys[1];
            return LineInterp(x, x0, x1, y0, y1);
        }
        return Ys[0];
    }
    if (x >= Xs.back())  // 右边界处理
    {
        if (type == 3 && x > Xs.back())
        {
            return 0;
        }
        if (type == 1 || type == 4)  // 在延长线上找到插值位置
        {
            if (std::abs(Xs.back() - Xs[num - 2]) > 1e-10)
            {
                const double x0 = Xs[num - 2];
                const double x1 = Xs[num - 1];
                const double y0 = Ys[num - 2];
                const double y1 = Ys[num - 1];
                return LineInterp(x, x0, x1, y0, y1);
            }
        }
        if (type == 2)
        {
            return Ys.back() + (x - Xs.back());
        }
        return Ys.back();
    }
    // 中间的进行插值
    for (int i = 0; i < num - 1; i++)
    {
        const double x0 = Xs[i];
        const double x1 = Xs[i + 1];
        if (x >= x0 && x <= x1)
        {
            if (type == 3)
            {
                return Ys[i];
            }
            const double y0 = Ys[i];
            const double y1 = Ys[i + 1];
            return LineInterp(x, x0, x1, y0, y1);
        }
    }
    return 0;
}
double AlgoL::LineInterp(const double& x, const double& x0, const double& x1, const double& y0, const double& y1)
{
    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}