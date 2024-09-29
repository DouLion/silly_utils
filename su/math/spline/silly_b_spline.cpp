//
// Created by dell on 2024/1/24.
//

#include "silly_b_spline.h"
/*
double silly_b_spline::N_i_p(int i, int p, double u, const std::vector<double>& knots)
{
    if (p == 0)
    {  // 当阶数为0时，即为分段常数
        return (u >= knots[i] && u < knots[i + 1]) ? 1.0 : 0.0;
    }
    double d1 = (u - knots[i]) / (knots[i + p] - knots[i]);
    double d2 = (knots[i + p + 1] - u) / (knots[i + p + 1] - knots[i + 1]);
    return d1 * N_i_p(i, p - 1, u, knots) + d2 * N_i_p(i + 1, p - 1, u, knots);
}

std::vector<silly_point> silly_b_spline::interpolation(const std::vector<silly_point>& points, int degree, int numPoints)
{
    int n = points.size();                      // 控制点数量
    std::vector<double> knots(n + degree + 1);  // 结点向量
    for (int i = 0; i <= degree; ++i)
        knots[i] = 0.0;
    for (int i = degree + 1; i < n + degree + 1; ++i)
        knots[i] = 1.0;

    std::vector<silly_point> result(numPoints);

    // 插值计算
    for (int j = 0; j < numPoints; ++j)
    {
        double t = static_cast<double>(j) / (numPoints - 1);  // 参数t在[0, 1]之间变化
        double x = 0.0, y = 0.0;
        for (int i = 0; i < n; ++i)
        {
            double basis = N_i_p(i, degree, t, knots);
            x += basis * points[i].x;
            y += basis * points[i].y;
        }
        result[j] = {x, y};
    }

    return result;
}*/
