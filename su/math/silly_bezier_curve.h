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

#include <geo/silly_geo.h>

/// <summary>
/// 参照 https://blog.csdn.net/sangxiaonian/article/details/51984013
/// </summary>

class silly_bezier_curve
{
  public:

    static silly_point bezier_interpolation_base(const silly_point& p1, const silly_point& p2, double sc)
    {
        silly_point result;
        result.lgtd = p1.lgtd + sc * (p2.lgtd - p1.lgtd);
        result.lttd = p1.lttd + sc * (p2.lttd - p1.lttd);
        return result;
    }
    /// <summary>
    /// K阶贝塞尔曲线计算公式
    /// </summary>
    /// <param name="k">k次</param>
    /// <param name="ctrls">控制点,控制点个数为k+1</param>
    /// <param name="n"> 1</param>
    /// <returns></returns>
    static std::vector<silly_point> bezier_interpolation(const int& k, const std::vector<silly_point>& ctrls, const int& n)
    {
        std::vector<silly_point> out;
        if(1 == k) return ctrls; //
        if(k+1 != ctrls.size()) return ctrls;// 无效控制点
        out.push_back(ctrls[0]);
        double c = 1;
        while(c <= n)
        {
            double sc = c/n;
            std::vector<silly_point> old_ctrls = ctrls;
            std::vector<silly_point> new_ctrls;
            int nk = k;
            while(nk> 2)
            {
                for(int i = 0; i < k; i++)
                {
                    new_ctrls.push_back(bezier_interpolation_base(old_ctrls[i], old_ctrls[i+1], sc));
                }
                old_ctrls = new_ctrls;
                new_ctrls.clear();
                nk--;
            }
            out.push_back(bezier_interpolation_base(old_ctrls[0], old_ctrls[1], sc));
            c++;
        }
        out.push_back(ctrls[0]);
    }

    static std::vector<silly_point>  bezier_curve(const int& k, const std::vector<silly_point>& ctrls, const int& n)
    {

    }

    /// <summary>
    /// 二阶贝塞尔曲线计算公式
    /// </summary>
    /// <param name="step"></param>
    /// <param name="start"></param>
    /// <param name="control "></param>
    /// <param name="end"></param>
    /// <returns></returns>
    static double quadratic_bezier(double step, double start, double control, double end)
    {
        double c1 = (control - start) * step + start;
        double c2 = (end - control) * step + control;
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

#endif  // SILLY_UTILS_SILLY_BEZIER_CURVE_H
