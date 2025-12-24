/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: RainRetentionCapacity.h
 * @description: 水库纳雨能力
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef RAIN_RETENTION_CAPACITY_H
#define RAIN_RETENTION_CAPACITY_H
#include <hydro/line/RZ2F_L.h>
#include <hydro/line/PAPR_L.h>
#include <hydro/line/RZ2W_L.h>
#include <hydro/rwdb/ST_PPTN_R.h>
#include <hydro/rwdb/ST_STBPRP_B.h>
#include <hydro/rwdb/ST_RSVR_R.h>
#include <hydro/rwdb/ST_RIVER_R.h>

struct CalcParam
{
    double BeginRZ = 0;
    double EndRZ = 0;
    // 四个目标水位数组
    double Yhdgc = -9999;
    double Xxsw = -9999;
    double Sjsw = -9999;
    double Bdgc = -9999;
    double Area = 0;
    double Wm = 0;
    double Pa = 0;
    double CalcSteps = 15;
    double Yhdk = 10;
    double KCH = 1;
    double TCH;
    double WCH;
    double pmin;
    double pmax;
    int CalcType = 0;
};
struct CalcResult
{
    double BeginRZ = 0;
    double BeginW = 0;
    double EndRZ = 0;
    double EndW = 0;
    double PP = 0;
    double PE = 0;
    double dW = 0;
    double OTW = 0;
    double OTQ = 0;
};

class RainRetentionCapacity
{
  public:
    RainRetentionCapacity() = default;
    ~RainRetentionCapacity() = default;
    RZ2W_L pRZ2WLine;
    PAPR_L pLine;

    std::vector<CalcResult> CalcNYNL(CalcParam& p);
    CalcResult CalcPPZ(CalcParam& p) const;
    CalcResult CalcPPF(CalcParam& p);

};
#endif  // RAIN_RETENTION_CAPACITY_H
