/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-03
 * @file: silly_delaunay_sweep_line.h
 * @description: silly_delaunay_sweep_line  德劳内三角扫线算法
 * @version: v1.0.1 2024-09-03 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_DELAUNAY_SWEEP_LINE_H
#define SILLY_UTILS_SILLY_DELAUNAY_SWEEP_LINE_H
#include <geo/triangular/silly_delaunay.h>

class silly_delaunay_sweep_line
{
  public:
    silly_delaunay_sweep_line() = default;
    ~silly_delaunay_sweep_line() = default;
    void points(std::vector<silly_dt_point>& pts);

  private:
    std::vector<silly_dt_point> m_points{};
};

#endif  // SILLY_UTILS_SILLY_DELAUNAY_SWEEP_LINE_H
