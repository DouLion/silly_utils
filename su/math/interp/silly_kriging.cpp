/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-03-13
 * @file: silly_kriging.cpp
 * @description: silly_kriging实现
 * @version: v1.0.1 2025-03-13 dou li yang
 */
#include "silly_kriging.h"
// --- 变异函数模型 (Variogram Models) ---
// 参数: nugget (块金值), sill (基台值), range (变程)
// 这里使用球状模型 (Spherical Model)
static double sphericalVariogram(double h, double nugget, double sill, double range)
{
    if (h <= 0)
        return 0.0;
    if (h >= range)
        return nugget + sill;

    double ratio = h / range;
    return nugget + sill * (1.5 * ratio - 0.5 * std::pow(ratio, 3));
}

// 也可以使用指数模型 (Exponential Model)
static double exponentialVariogram(double h, double nugget, double sill, double range)
{
    if (h <= 0)
        return 0.0;
    return nugget + sill * (1.0 - std::exp(-3.0 * h / range));
}


// --- 高斯消元法求解线性方程组 Ax = B ---
// A 是 n x n 矩阵，B 是 n x 1 向量
// 返回解向量 x
static std::vector<double> solveLinearSystem(std::vector<std::vector<double> > A, std::vector<double> B)
{
    int n = B.size();

    // 前向消元
    for (int i = 0; i < n; ++i)
    {
        // 寻找主元 (Pivoting) 以提高数值稳定性
        int maxRow = i;
        for (int k = i + 1; k < n; ++k)
        {
            if (std::abs(A[k][i]) > std::abs(A[maxRow][i]))
            {
                maxRow = k;
            }
        }

        if (std::abs(A[maxRow][i]) < 1e-10)
        {
            throw std::runtime_error("Matrix is singular or nearly singular.");
        }

        // 交换行
        std::swap(A[i], A[maxRow]);
        std::swap(B[i], B[maxRow]);

        // 消元
        for (int k = i + 1; k < n; ++k)
        {
            double factor = A[k][i] / A[i][i];
            for (int j = i; j < n; ++j)
            {
                A[k][j] -= factor * A[i][j];
            }
            B[k] -= factor * B[i];
        }
    }

    // 回代
    std::vector<double> x(n);
    for (int i = n - 1; i >= 0; --i)
    {
        double sum = 0.0;
        for (int j = i + 1; j < n; ++j)
        {
            sum += A[i][j] * x[j];
        }
        x[i] = (B[i] - sum) / A[i][i];
    }

    return x;
}

double OrdinaryKriging::PredictedKriging(const std::vector<suPoint>& known, suPoint& predict, double nugget, double sill, double range)
{
    int n = known.size();
    int matrixSize = n + 1; // +1 用于拉格朗日乘数

    // 初始化矩阵 A 和向量 B
    // 方程组形式:
    // [ C  1 ] [ λ ]   [ c ]
    // [ 1^T 0 ] [ μ ] = [ 1 ]
    // 其中 C 是协方差/变异函数矩阵, 1 是全1向量, μ 是拉格朗日乘数

    std::vector<std::vector<double> > A(matrixSize, std::vector<double>(matrixSize, 0.0));
    std::vector<double> B(matrixSize, 0.0);

    // 1. 填充变异函数矩阵 (左上角 n x n)
    // 注意：克里金方程组通常直接使用半变异函数 γ(h)
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            double h = known[i].dist(known[j]);
            A[i][j] = sphericalVariogram(h, nugget, sill, range);
        }
    }

    // 2. 填充约束条件 (最后一列和最后一行为 1，右下角为 0)
    for (int i = 0; i < n; ++i)
    {
        A[i][n] = 1.0;
        A[n][i] = 1.0;
    }
    A[n][n] = 0.0;

    // 3. 填充右侧向量 B
    // 前 n 个元素是待估点与已知点的半变异函数
    for (int i = 0; i < n; ++i)
    {
        double h = known[i].dist(predict);
        B[i] = sphericalVariogram(h, nugget, sill, range);
    }
    // 最后一个元素是约束值 1
    B[n] = 1.0;

    // 4. 求解方程组
    std::vector<double> solution = solveLinearSystem(A, B);

    // 5. 提取权重 (前 n 个) 和 拉格朗日乘数 (最后 1 个)
    std::vector<double> weights(n);
    for (int i = 0; i < n; ++i)
    {
        weights[i] = solution[i];
    }
    double mu = solution[n];

    // 6. 计算预测值 Z* = Σ (λ_i * Z_i)
    double zPred = 0.0;
    for (int i = 0; i < n; ++i)
    {
        zPred += weights[i] * known[i].v;
    }

    // 7. 计算克里金方差 σ² = Σ (λ_i * γ(h_i0)) + μ
    // 注意：这里的 γ(h_i0) 就是 B[i] (在求解前计算的)
    double krigingVar = 0.0;
    for (int i = 0; i < n; ++i)
    {
        krigingVar += weights[i] * B[i];
    }
    krigingVar += mu;

    // 方差理论上应非负，但因数值误差可能略小于0，需处理
    if (krigingVar < 0)
    {
        krigingVar = 0.0;
    }
    predict.v = zPred;
    return krigingVar;
}