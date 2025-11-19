/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/7/15 16:38
 * @version: 1.0.1
 * @description: 插值函数
 */
#ifndef SILLY_UTILS_SILLY_INTERPOLATE_H
#define SILLY_UTILS_SILLY_INTERPOLATE_H

// template <typename double>
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
class silly_bilinear_interpolate
{
  public:
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
};

#endif  // SILLY_UTILS_SILLY_INTERPOLATE_H
