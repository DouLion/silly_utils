/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2026-01-08
 * @file: silly_rwdb.h
 * @description: silly_rwdb 头文件
 * @version: v1.0.1 2026-01-08 dou li yang
 */
#ifndef SILLY_RWDB_H
#define SILLY_RWDB_H
#include <hydro/rwdb/ST_STBPRP_B.h>
#include <hydro/rwdb/ST_PPTN_R.h>
#include <hydro/rwdb/ST_RIVER_R.h>
#include <hydro/rwdb/ST_RSVR_R.h>

namespace RWDB
{

/**
 * 将水文中习惯表示的时段长转为分钟
 * 1.16 表示 1小时16分钟
 * @param duration 时段长
 * @return
 */
inline std::time_t Duration2Seconds(const double&  duration)
{
    const std::time_t hh = std::floor(duration);
    return (duration - hh) * 100 + hh * 60;
}
inline std::time_t Intv2Sec(const double&  duration)
{
    return Duration2Seconds(duration);
}

inline double Duration2Minutes(const double&  duration)
{
    const std::time_t hh = std::floor(duration);
    return (duration - hh) * 100 + hh * 60;
}

}



#endif  // SILLY_RWDB_H
