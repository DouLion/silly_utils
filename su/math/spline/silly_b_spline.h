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
    double b2(int i, double t, const std::vector<double>& knots) {
        if (i < 0 || i >= knots.size() - 2) return 0.0;
        if (knots[i] <= t && t < knots[i + 1]) return (t - knots[i]) / (knots[i + 1] - knots[i]);
        if (knots[i + 1] <= t && t < knots[i + 2]) return (knots[i + 2] - t) / (knots[i + 2] - knots[i + 1]);
        return 0.0;
    }

    // 二次 B 样条插值
    silly_point quadratic_b_Spline(double t, const std::vector<silly_point>& controlPoints, const std::vector<double>& knots) {
        silly_point result = {0.0, 0.0};
        for (size_t i = 0; i < controlPoints.size(); ++i) {
            double b = b2(i, t, knots);
            result.lgtd += b * controlPoints[i].lgtd;
            result.lttd += b * controlPoints[i].lttd;
        }
        return result;
    }
};

#endif  // SILLY_UTILS_SILLY_B_SPLINE_H
