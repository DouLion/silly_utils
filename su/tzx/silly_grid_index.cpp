/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-11
 * @file: suGridIndx.cpp
 * @description: suGridIndx实现
 * @version: v1.0.1 2024-10-11 dou li yang
 */
#include "silly_grid_index.h"

bool divisible(double a, double b)
{
    double quotient = a / b;
    double roundedQuotient = std::round(quotient);
    return std::fabs(b * roundedQuotient - a) < SU_EPSILON;
}
int64_t suGridIndx::index(const double& lgtd, const double& lttd, const double& step)
{
    int64_t col = (int64_t)(CN_RECT_RIGHT - CN_RECT_LEFT) / step + 1;
    return (int64_t)((lttd - CN_RECT_BOTTOM) / step) * col + (int64_t)((lgtd - CN_RECT_LEFT) / step) + 1;
}

std::pair<double, double> suGridIndx::point(const int64_t& index, const double& step)
{
    int64_t row = (int64_t)(CN_RECT_RIGHT - CN_RECT_LEFT) / step + 1;
    double x = (index - 1) % row * step + CN_RECT_LEFT;
    double y = (index - 1) / row * step + CN_RECT_BOTTOM;

    return std::pair<double, double>(x, y);
}

int64_t suGridIndx::checked_index(const double& lgtd, const double& lttd, const double& step)
{
    if (step < 1e-6)
    {
        throw std::runtime_error("step 不应该小于1e-6 .");
    }
    if (!divisible(1.0, step))
    {
        throw std::runtime_error("step 需要能整除1.0 .");
    }
    if (lgtd < CN_RECT_LEFT || lgtd > CN_RECT_RIGHT || lttd < CN_RECT_BOTTOM || lttd > CN_RECT_TOP)
    {
        throw std::runtime_error("坐标超出中国范围");
    }

    int64_t cols = std::round((CN_RECT_RIGHT - CN_RECT_LEFT) / step + 1);
    int64_t col = std::round((lgtd - CN_RECT_LEFT) / step);
    int64_t row = std::round((lttd - CN_RECT_BOTTOM) / step);
    return row * cols + col + 1;
}

std::pair<double, double> suGridIndx::checked_point(const int64_t& index, const double& step)
{
    if (step < 1e-6)
    {
        throw std::runtime_error("step 不应该小于1e-6 .");
    }
    if (!divisible(1.0, step))
    {
        throw std::runtime_error("step 需要能整除1.0 .");
    }
    if (index == 0)
    {
        throw std::runtime_error("序号从1开始");
    }
    int64_t row = std::round((CN_RECT_RIGHT - CN_RECT_LEFT) / step + 1);
    double x = (index - 1) % row * step + CN_RECT_LEFT;
    double y = (index - 1) / row * step + CN_RECT_BOTTOM;

    return std::pair<double, double>(x, y);
}
int64_t suGridIndx::customize_index(const double& lgtd, const double& lttd, const double& step, std::pair<double, double> minp, std::pair<double, double> maxp, const int64_t& bidx)
{
    double left = minp.first;
    double right = maxp.first;
    double bottom = minp.second;
    double top = maxp.second;
    if (lgtd >= left && lgtd <= right && lttd >= bottom && lttd <= top)
    {
        int64_t col = (int64_t)(right - left) / step + 1;
        return (int64_t)((lttd - bottom) / step) * col + (int64_t)((lgtd - left) / step) + bidx;
    }
    return 0;
}
std::pair<double, double> suGridIndx::customize_point(const int64_t& index, const double& step, std::pair<double, double> minp, std::pair<double, double> maxp, const int64_t& bidx)
{
    double left = minp.first;
    double right = maxp.first;
    double bottom = minp.second;
    double top = maxp.second;
    if (index == 0)
    {
        throw std::runtime_error("序号从1开始");
    }
    int64_t row = (int64_t)(right - left) / step + 1;
    double x = (index - bidx) % row * step + left;
    double y = (index - bidx) / row * step + bottom;

    return std::pair<double, double>(x, y);
}
int64_t suCNGrid::index(const suCNGrid::LLPoint& point, const double& step)
{
    if (step < 0.01)
    {
        return 0;
    }
    int64_t cols = (int64_t)(CN_RECT_RIGHT - CN_RECT_LEFT) / step + 1;
    int64_t rows = (int64_t)(CN_RECT_TOP - CN_RECT_BOTTOM) / step + 1;
    int64_t col = static_cast<int64_t>(std::round((point.lon - CN_RECT_LEFT) / step));
    int64_t row = static_cast<int64_t>(std::round((point.lat - CN_RECT_BOTTOM) / step));
    if (col < 0 || col >= cols || row < 0 || row >= rows)
    {
        return 0;
    }
    return (row * cols + col) + 1;
}
suCNGrid::LLPoint suCNGrid::point(const int64_t& index, const double& step)
{
    int64_t cols = (int64_t)(CN_RECT_RIGHT - CN_RECT_LEFT) / step + 1;
    int64_t row = (index - 1) / cols;
    int64_t col = (index - 1) % cols;
    return suCNGrid::LLPoint{CN_RECT_LEFT + col * step, CN_RECT_BOTTOM + row * step};
}
double suCNGrid::step(const suCNGrid::LLPoint& point, const int64_t& index)
{
    double ret = 0.0;
    // 根据经纬度和索引 推算步长
    ret = std::abs(point.lon - suCNGrid::point(index, ret).lon) + std::abs(point.lat - suCNGrid::point(index, ret).lat);

    return ret;
}
