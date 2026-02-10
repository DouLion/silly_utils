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

/**
 * @brief 水库纳雨能力计算参数结构体
 */
struct RRCParam
{
    double BRZ = 0; // [输入] 起始水位 (Begin Reservoir Z, m)
    // double ERZ = 0;        // [内部计算] 结束水位 (End Reservoir Z, m)

    // 目标水位集合 (Target Water Levels)
    struct
    {
        double YhdE = -9999; // 溢洪道堰顶高程 (Spillway Elevation, m)
        double JhRZ = -9999; // 校核洪水位 (Check Flood Level, m)
        double SjRZ = -9999; // 设计洪水位 (Design Flood Level, m)
        double BadE = -9999; // 坝顶高程 (Dam Crest Elevation, m)
    } DstRZ;

    double Area = 0; // 流域集雨面积 (Catchment Area, km2)
    double Wm = 0;   // 流域平均最大蓄水量 (Max Soil Moisture Storage, mm)
    double Pa = 0;   // 当前流域平均土壤含水量 (Current Soil Moisture, mm)

    // 水力学参数
    double YhdW = 10;  // 溢洪道净宽 (Weir Width, m)
    double KCH = 0.65; // 堰流综合流量修正系数 (epsilon, 无量纲, 0.6-0.95)
    // 公式: Q = m * KCH * B * sqrt(2g) * H^1.5
    // 计算控制参数
    double CalcSteps = 15; // 演算时间步长 (minutes)
    int CalcType = 1;      // 计算模式: 0=仅做水量平衡粗算, 1=进行调洪演算精算

    struct
    {
        double TCH = 0;  // 预见期/出流时长 (minutes)
        double WCH = -1; // 指定的总出库水量 (10^6 m3, 若设置则优先使用)

        double pmin = -1; // 降雨量下限约束 (mm)
        double pmax = -1; // 降雨量上限约束 (mm)
    } optional;
};

/**
 * @brief 计算结果结构体
 */
struct RRCResult
{
    double BRZ = 0; // 起始水位 (m)
    double BW = 0;  // 起始库容 (m3 或 万m3, 取决于 Unit)
    double ERZ = 0; // 目标结束水位 (m)
    double EW = 0;  // 目标结束库容
    double PP = 0;  // 【核心结果】允许最大降雨量 (Precipitation, mm)
    double PE = 0;  // 对应的净雨量 (Net Rain / Runoff, mm)
    double dW = 0;  // 期间库容变化量 (Delta W)
    double OTW = 0; // 期间总出库水量 (Outflow Total Volume)
    void Print() const;
};

struct RRCResultSet
{
    RRCResult YhdE; // 溢洪道高程对应的纳雨能力
    RRCResult JhRZ; // 校核洪水位对应的纳雨能力
    RRCResult SjRZ; // 设计洪水位对应的纳雨能力
    RRCResult BadE; // 坝顶高程对应的纳雨能力
};

class RainRetentionCapacity
{
public:
    RainRetentionCapacity() = default;

    ~RainRetentionCapacity() = default;

    RZ2W_L pRZ2WLine; // 水位库容关系
    PaPR_L pPaPRLine; // 雨量径流关系

    RRCResultSet CalcRRC(const RRCParam& p) const;

    /**
     * @brief 调洪演算精算法 (Routing Simulation)
     * 通过二分法假设降雨量，生成洪水过程线，推演水库水位变化
     */
    RRCResult CalcPPZ(const RRCParam& p, const double& dstRZ) const;

    /**
     * @brief 水量平衡粗算法 (Volume Balance)
     * 公式: 允许入库总量 = (目标库容 - 当前库容) + (预估平均出流 * 时间)
     */
    RRCResult CalcPPF(const RRCParam& p, const double& dstRZ) const;

    void TestPaPRLine(); // 已确认
    void TestRZWLine();

    void TestModel(); // 已确认
};
#endif  // RAIN_RETENTION_CAPACITY_H