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

struct NynlParam
{
    double BRZ = 0;  // 起始水位(m)
    // double ERZ = 0;  // 结束水位
    // 四个目标水位数组
    struct
    {
        double YhdE = -9999;  // 溢洪道高程(m)
        double JhRZ = -9999;  // 校核洪水位(m)
        double SjRZ = -9999;  // 设计洪水位(m)
        double BadE = -9999;  // 坝顶高程(m)
    } DstRZ;

    double Area = 0;        // 流域面积(km2)
    double Wm = 0;          // 最大蓄水量(mm)
    double Pa = 0;          // 土壤湿度 * Wm
    double YhdW = 10;       // 溢洪道宽度(m)
    double KCH = 1;         // 流量系数
    double CalcSteps = 15;  // 计算步长 分钟
    int CalcType = 1;       // 0 反算 1 正算
    struct
    {
        double TCH = 0;   // 出流时间 分钟
        double WCH = -1;  // 出流量 百万方

        double pmin = -1;  // 最小降雨(mm)
        double pmax = -1;  // 最大降雨(mm)
    } optional;            // 可选参数
};

struct NynlResult
{
    double BRZ = 0;  // 起始水位
    double BW = 0;   // 起始库容
    double ERZ = 0;  // 结束水位
    double EW = 0;   // 结束库容
    double PP = 0;   // 降雨量
    double PE = 0;   // 净雨量
    double dW = 0;   // 库容差
    double OTW = 0;  // 出库水量

    void Print() const;
};

class RainRetentionCapacity
{
  public:
    RainRetentionCapacity() = default;
    ~RainRetentionCapacity() = default;
    RZ2W_L pRZ2WLine;  // 水位库容关系
    PAPR_L pPaPrLine;  // 雨量径流关系

    std::vector<NynlResult> CalcNYNL(const NynlParam& p) const;
    NynlResult CalcPPZ(const NynlParam& p, const double& dstRZ) const;
    NynlResult CalcPPF(const NynlParam& p, const double& dstRZ) const;

    void TestPAPRLine();  // 已确认
    void TestRZWLine();
    void TestModel();  // 已确认
};
#endif  // RAIN_RETENTION_CAPACITY_H
