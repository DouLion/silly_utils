/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-03
 * @file: silly_delaunay_sweep_line.cpp
 * @description: silly_delaunay_sweep_line实现
 * @version: v1.0.1 2024-09-03 dou li yang
 */
#include "silly_delaunay_sweep_line.h"
void silly_delaunay_sweep_line::points(std::vector<silly_dt_point>& pts)
{
    m_points = pts;
    std::sort(m_points.begin(), m_points.end(), [](silly_dt_point& lhs, silly_dt_point& rhs) -> bool {  return lhs.lttd < rhs.lttd || (lhs.lttd == rhs.lttd && lhs.lgtd < rhs.lgtd);});
}
