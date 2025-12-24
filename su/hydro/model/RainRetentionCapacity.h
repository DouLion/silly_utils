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
    double BeginRZ = 0;  // 起始水位(m)
    double EndRZ = 0;
    // 四个目标水位数组
    double Yhdgc = -9999;   // 溢洪道高程(m)
    double Xxsw = -9999;    // 校核洪水位(m)
    double Sjsw = -9999;    // 设计洪水位(m)
    double Bdgc = -9999;    // 坝顶高程(m)
    double Area = 0;        // 流域面积(km2)
    double Wm = 0;          // 最大蓄水量(mm)
    double Pa = 0;          // 前期影响雨量(mm)
    double Yhdk = 10;       // 溢洪道宽度(m)
    double KCH = 1;         // 流量系数
    double TCH;             // 出流时间 分钟
    double WCH;             // 出流量 百万方
    double pmin;            // 最小降雨(mm)
    double pmax;            // 最大降雨(mm)
    double CalcSteps = 15;  // 计算步长 分钟
    int CalcType = 0;       // 0 反算 1 正算
};

struct ForwardParam
{
    double BeginRZ = 0;  // 起始水位(m)
    double EndRZ = 0;
    // 四个目标水位数组
    double Yhdgc = -9999;   // 溢洪道高程(m)
    double Xxsw = -9999;    // 校核洪水位(m)
    double Sjsw = -9999;    // 设计洪水位(m)
    double Bdgc = -9999;    // 坝顶高程(m)
    double Area = 0;        // 流域面积(km2)
    double Wm = 0;          // 最大蓄水量(mm)
    double Pa = 0;          // 前期影响雨量(mm)
    double Yhdk = 10;       // 溢洪道宽度(m)
    double KCH = 1;         // 流量系数
    double TCH;             // 出流时间 分钟
    double WCH;             // 出流量 百万方
    double pmin;            // 最小降雨(mm)
    double pmax;            // 最大降雨(mm)
    double CalcSteps = 15;  // 计算步长 分钟
    int CalcType = 0;       // 0 反算 1 正算
};
struct InverseParam
{
    double BeginRZ = 0;  // 起始水位(m)
    double EndRZ = 0;
    // 四个目标水位数组
    double Yhdgc = -9999;   // 溢洪道高程(m)
    double Xxsw = -9999;    // 校核洪水位(m)
    double Sjsw = -9999;    // 设计洪水位(m)
    double Bdgc = -9999;    // 坝顶高程(m)
    double Area = 0;        // 流域面积(km2)
    double Wm = 0;          // 最大蓄水量(mm)
    double Pa = 0;          // 前期影响雨量(mm)
    double Yhdk = 10;       // 溢洪道宽度(m)
    double KCH = 1;         // 流量系数
    double TCH;             // 出流时间 分钟
    double WCH;             // 出流量 百万方
    double pmin;            // 最小降雨(mm)
    double pmax;            // 最大降雨(mm)
    double CalcSteps = 15;  // 计算步长 分钟
    int CalcType = 0;       // 0 反算 1 正算
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

    void Print() const
    {
        std::cout << "{\n  BeginRZ:" << BeginRZ;
        std::cout << "\n  BeginW:" << BeginW;
        std::cout << "\n  EndRZ:" << EndRZ;
        std::cout << "\n  EndW:" << EndW;
        std::cout << "\n  PP:" << PP;
        std::cout << "\n  PE:" << PE;
        std::cout << "\n  dW:" << dW;
        std::cout << "\n  OTW:" << OTW;
        std::cout << "\n  OTQ:" << OTQ << "\n}," << std::endl;
    }
};

class RainRetentionCapacity
{
  public:
    RainRetentionCapacity() = default;
    ~RainRetentionCapacity() = default;
    RZ2W_L pRZ2WLine;  // 水位库容关系
    PAPR_L pPaPrLine;  // 雨量径流关系

    std::vector<CalcResult> Forward(ForwardParam& p);
    std::vector<CalcResult> Inverse(InverseParam& p);

    std::vector<CalcResult> CalcNYNL(CalcParam& p);
    CalcResult CalcPPZ(CalcParam& p) const;
    CalcResult CalcPPF(CalcParam& p);

    void Test();
};
#endif  // RAIN_RETENTION_CAPACITY_H
