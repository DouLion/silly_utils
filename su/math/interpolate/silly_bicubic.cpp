/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-30
 * @file: silly_bicubic.cpp
 * @description: silly_bicubic实现
 * @version: v1.0.1 2024-12-30 dou li yang
 */
#include "silly_bicubic.h"
using namespace silly::interpolation;

// 猫姆鲁普滤波器权重函数
static double catmull_rom(double t)
{
    double abs_t = std::abs(t);
    if (abs_t < 1.0)
    {
        return 0.5 * (1.0 - 2.0 * t * t + t * t * t);
    }
    else if (abs_t < 2.0)
    {
        return 0.5 * (2.0 - abs_t) * (2.0 - abs_t) * (2.0 - abs_t) / 2.0;
    }
    else
    {
        return 0.0;
    }
}

// 边界处理：使用最近邻法
static double get_value(const std::vector<std::vector<double>>& grid, int x, int y)
{
    int width = grid.size();
    int height = grid[0].size();

    // 处理边界情况
    if (x < 0)
        x = 0;
    if (x >= width)
        x = width - 1;
    if (y < 0)
        y = 0;
    if (y >= height)
        y = height - 1;

    return grid[x][y];
}


double bicubic::calc(const std::vector<double>& grid, const double& dx, const double& dy)
{
    //int width = grid.size();
    //int height = grid[0].size();

    //// 找到最近的整数坐标
    //int x_floor = static_cast<int>(std::floor(dx));
    //int y_floor = static_cast<int>(std::floor(dy));

    //// 计算相对距离
    //double dx = x - x_floor;
    //double dy = y - y_floor;

    //// 初始化结果
    //double result = 0.0;

    //// 遍历 4x4 的邻域
    //for (int i = -1; i <= 2; ++i)
    //{
    //    for (int j = -1; j <= 2; ++j)
    //    {
    //        // 计算权重
    //        double weight_x = catmull_rom(dx - i);
    //        double weight_y = catmull_rom(dy - j);

    //        // 获取邻点的值
    //        double value = get_value(grid, x_floor + i, y_floor + j);

    //        // 累加加权和
    //        result += weight_x * weight_y * value;
    //    }
    //}

    //return result;
    return 0;
}