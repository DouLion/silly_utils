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

class silly_bezier_curve
{
public:

    /// <summary>
    /// 二阶贝塞尔曲线计算公式
    /// </summary>
    /// <param name="step"></param>
    /// <param name="x1"></param>
    /// <param name="x2"></param>
    /// <param name="x3"></param>
    /// <returns></returns>
    static double quadratic_bezier(const double& step, double x1, double x2, double x3)
    {
        double c1 = (x2 - x1) * step + x1;
        double c2 = (x3 - x2) * step + x2;
        double c3 = (c2 - c1) * step + c1;
        return c3;
    }

    
    /// <summary>
    /// 三阶贝塞尔曲线计算公式
    /// </summary>
    /// <returns></returns>
    static double cubic_bezier()
    {
        return 0;
    }
};

#endif //SILLY_UTILS_SILLY_BEZIER_CURVE_H
