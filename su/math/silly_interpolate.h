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

template <typename T>
class silly_bilinear_interpolate
{
  public:
    T interpolate()
    {
        T x_delta = (x - xl) / (xr - xl);
        T y_delta = (y - yb) / (yt - yb);

        return (1 - x_delta) * (1 - y_delta) * vlb + x_delta * (1 - y_delta) * vlr + (1 - x_delta) * y_delta * vtb + x_delta * y_delta * vtr;
    }

  public:
    T xl;  // left
    T xr;  // right
    T yb;  // bottom
    T yt;  // top
    T x;
    T y;

    T vlb;  // left bottom
    T vtb;  // top bottom
    T vlr;  // left top
    T vtr;  // right top
};

#endif  // SILLY_UTILS_SILLY_INTERPOLATE_H
