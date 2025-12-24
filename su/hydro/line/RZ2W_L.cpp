/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: RZ2W_L.c
 * @description: RZ2W_L实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "RZ2W_L.h"

RZ2W_L::RZ2W_L(const double& unit)
{
    m_unit = unit;
}

double RZ2W_L::GetW(const double& z, const int& type) const
{
    return GetYFromX(z, type);
}
double RZ2W_L::GetZ(const double& w, const int& type) const
{
    return GetXFromY(w, type);
}