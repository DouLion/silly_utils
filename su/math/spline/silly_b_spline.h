/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/24 11:00
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_B_SPLINE_H
#define SILLY_UTILS_SILLY_B_SPLINE_H
#include <geo/silly_geo.h>

class silly_b_spline
{
  public:
    // 计算二项式系数
    static double binom(int n, int k)
    {
        if (k == 0 || k == n)
            return 1.0;
        double r = 1.0;
        for (int i = 1; i <= k; ++i)
        {
            r *= (n + 1 - i) / i;
        }
        return r;
    }

    // 计算B样条基函数
    static double basisFunc(int i, int k, const std::vector<double>& U, double u)
    {
        if (k == 0)
        {  // 当k=0时，N(i,k)(u)=1如果U[i]<=u<U[i+1],否则为0
            return (U[i] < U[i + 1]) ? (U[i] <= u && u < U[i + 1]) : (U[i] == u);
        }
        else
        {
            double leftTerm = (u - U[i]) * basisFunc(i, k - 1, U, u) / (U[i + k] - U[i]);
            double rightTerm = (U[i + k + 1] - u) * basisFunc(i + 1, k - 1, U, u) / (U[i + k + 1] - U[i + 1]);
            return leftTerm + rightTerm;
        }
    }

    // 计算B样条曲线上的点
    static silly_point splinePoint(const std::vector<silly_point>& controlPoints, int k, const std::vector<double>& U, double u)
    {
        double x = 0.0, y = 0.0;
        for (int i = 0; i < controlPoints.size(); ++i)
        {
            double N_i_k = basisFunc(i, k, U, u);
            x += controlPoints[i].lgtd * N_i_k;
            y += controlPoints[i].lttd * N_i_k;
        }
        return silly_point(x, y);
    }

    // 计算三次B样条插值点
    static std::vector<silly_point> cubic_interpolation(const std::vector<silly_point>& controlPoints, int numPoints)
    {
        if (controlPoints.size() < 5)  // 少于四个控制点直接不处理
        {
            return controlPoints;
        }

        int n = controlPoints.size();
        int degree = 3;                             // 三次B样条
        std::vector<double> knots(n + degree + 1);  // 结点向量

        // 构造均匀结点向量
        for (int i = 0; i <= degree; ++i)
            knots[i] = 0.0;
        for (int i = degree + 1; i < n + degree + 1; ++i)
            knots[i] = i - degree;

        std::vector<silly_point> result(numPoints);

        // 插值计算
        for (int j = 0; j < numPoints; ++j)
        {
            double t = static_cast<double>(j) / (numPoints - 1);  // 参数t在[0, 1]之间变化
            result[j] = splinePoint(controlPoints, degree, knots, t);
        }

        return result;
    }
};

#endif  // SILLY_UTILS_SILLY_B_SPLINE_H
