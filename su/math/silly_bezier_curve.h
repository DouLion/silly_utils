/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/15 14:12
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_BEZIER_CURVE_H
#define SILLY_UTILS_SILLY_BEZIER_CURVE_H

#include <vector>

/// <summary>
/// 参照 https://blog.csdn.net/sangxiaonian/article/details/51984013
/// </summary>


class silly_bezier_curve
{
public:

    /// <summary>
    /// 二阶贝塞尔曲线计算公式
    /// </summary>
    /// <param name="step"></param>
    /// <param name="start"></param>
    /// <param name="control "></param>
    /// <param name="end"></param>
    /// <returns></returns>
    static double quadratic_bezier(double step, double start, double control , double end)
    {
        double c1 = (control  - start) * step + start;
        double c2 = (end - control ) * step + control ;
        // double c3 = (c2 - c1) * step + c1;
        // return c3;
        return (c2 - c1) * step + c1;
    }

    
    /// <summary>
    /// 三阶贝塞尔曲线计算公式
    /// </summary>
    /// <param name="step"></param>
    /// <param name="start"></param>
    /// <param name="c1">控制点1</param>
    /// <param name="c2">控制点1</param>
    /// <param name="end"></param>
    /// <returns></returns>
    static double cubic_bezier(double step, double start, double c1, double c2, double end)
    {
        return 0;
    }

    /// <summary>
    /// 高阶贝塞尔曲线计算, 3阶由2阶多递归一次, 以此类推高阶计算方式
    /// </summary>
    /// <param name="step"></param>
    /// <param name="start">开始点</param>
    /// <param name="points">控制点</param>
    /// <param name="end">终点</param>
    /// <returns></returns>
    static double high_order_bezier(double step, double start, std::vector<double> points, double end)
    {
        return 0;
    }
};

#endif //SILLY_UTILS_SILLY_BEZIER_CURVE_H
