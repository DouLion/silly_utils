/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-03-13
 * @file: silly_kriging.cpp
 * @description: silly_kriging实现
 * @version: v1.0.1 2025-03-13 dou li yang
 */
#include "silly_kriging.h"
double suKrigingInterp::GetZ(const double& x, const double& y) const
{
    throw std::runtime_error("suKrigingInterp::GetZ");
    return 0;
}
void suKrigingInterp::SetPoints(const std::vector<suPoint>& points)
{
    m_points = points;
}