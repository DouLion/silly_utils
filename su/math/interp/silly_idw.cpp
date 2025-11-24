/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-11-24
 * @file: silly_idw.c
 * @description: silly_idw实现
 * @version: v1.0.1 2025-11-24 dou li yang
 */
#include "silly_idw.h"
double suIDW::get(const double& x, const double& y, const double& ign) const
{
    if (m_points.empty()) {
        throw std::runtime_error("No points available for interpolation.");
    }

    double sum_weighted_value = 0.0;
    double sum_weights = 0.0;
    bool has_valid_point = false;

    for (const auto& p : m_points) {
        double dx = x - p.x;
        double dy = y - p.y;
        double dist_sq = dx * dx + dy * dy;

        // 如果目标点与已知点重合，直接返回该点的值
        if (dist_sq == 0.0) {
            return p.z;
        }

        double dist = std::sqrt(dist_sq);

        // 如果设置了忽略距离且当前点超出范围，跳过
        if (ign > 0 && dist > ign) {
            continue;
        }

        // IDW 权重：1 / dist^2
        double weight = 1.0 / dist_sq;  // 等价于 1 / (dist^2)

        sum_weighted_value += weight * p.z;
        sum_weights += weight;
        has_valid_point = true;
    }

    if (!has_valid_point) {
        // 可选：返回 NaN 或抛出异常
        // return std::numeric_limits<double>::quiet_NaN();
        throw std::runtime_error("No points within the specified influence radius.");
    }

    return sum_weighted_value / sum_weights;
}