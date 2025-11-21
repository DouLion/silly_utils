/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-30
 * @file: silly_bilinear.h
 * @description: 双线性插值 类声明
 * @version: v1.0.1 2024-12-30 dou li yang
 */
#ifndef SILLY_BILINEAR_INTERP_H
#define SILLY_BILINEAR_INTERP_H

class suBilinearInterp
{
  public:
    /// <summary>
    ///
    /// q10  q11
    /// q00  q01 上下可以逆序,但是dx 和 dy要与q0 - q3的值要一致
    /// </summary>
    /// <param name="q00">左下角的值 (0, 0)</param>
    /// <param name="q01">右下角的值 (dx, 0)</param>
    /// <param name="q10">左上角的值 (0, dy)</param>
    /// <param name="q11">右上角的值 (dx, dy)</param>
    /// <param name="dx">距离 q00q10 的距离权重</param>
    /// <param name="dy">距离 q00q01 的距离权重</param>
    /// <returns></returns>
    static double calc(const double& q00, const double& q01, const double& q10, const double& q11, const double& dx, const double& dy);

    /*
     Q3(x0,y1) ------ Q4(x1,y1)
        |               |
        |               |
        |     (x,y)     |
        |               |
     Q1(x0,y0) ------ Q2(x1,y0)

    Q1 = (x0, y0) 高程为 z11
    Q2 = (x1, y0) 高程为 z21
    Q3 = (x0, y1) 高程为 z12
    Q4 = (x1, y1) 高程为 z22

    */
    double interpolate(double x, double y) const
    {
        double z_bottom = Q1 * (x1 - x) / (x1 - x0) + Q2 * (x - x0) / (x1 - x0);
        double z_top = Q3 * (x1 - x) / (x1 - x0) + Q4 * (x - x0) / (x1 - x0);
        double z = z_bottom * (y1 - y) / (y1 - y0) + z_top * (y - y0) / (y1 - y0);
        return z;
    }

public:
    double x0;  // left
    double x1;  // right
    double y0;  // bottom
    double y1;  // top

    double Q1;  // left bottom
    double Q2;  // top bottom
    double Q3;  // left top
    double Q4;  // right top

  private:
};

#endif  // SILLY_BILINEAR_INTERP_H
