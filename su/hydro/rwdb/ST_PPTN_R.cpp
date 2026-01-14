/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: ST_PPTN_R.c
 * @description: ST_PPTN_R实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "ST_PPTN_R.h"
double RWDB::ST_PPTN_R::INTV2S(const double& intv)
{
    if (intv <= 0.0)
    {
        return 1.0;
    }
    double hour = std::floor(intv);
    double minute = intv - hour;
    return hour * 3600 + minute * 60;
}
double RWDB::ST_PPTN_R::INTV2MS(const double& intv)
{
    return INTV2S(intv) * 1000.0;
}