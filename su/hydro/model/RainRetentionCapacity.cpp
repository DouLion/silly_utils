/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: RainRetentionCapacity
 * @description: RainRetentionCapacity实现
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#include "RainRetentionCapacity.h"
#include <log/silly_log.h>

RRCResultSet RainRetentionCapacity::CalcRRC(const RRCParam& p) const
{
    // 定义四个目标水位：溢洪道、校核、设计、坝顶
    std::vector<double> pTargetRZ = {p.DstRZ.YhdE, p.DstRZ.JhRZ, p.DstRZ.SjRZ, p.DstRZ.BadE};
    RRCResultSet ret;
    std::vector<RRCResult*> nynlRet = {
        &ret.YhdE,
        &ret.JhRZ,
        &ret.SjRZ,
        &ret.BadE
    };

    // 1. 查水位库容曲线(Z-V Curve)，获取当前水位对应的库容
    double BW = pRZ2WLine.GetWFromZ(p.BRZ, 1);

    for (int i = 0; i < pTargetRZ.size(); i++)
    {
        nynlRet[i]->BRZ = p.BRZ;
        nynlRet[i]->BW = BW;

        double dstRZ = pTargetRZ[i];

        // 查目标水位对应的库容
        double EW = pRZ2WLine.GetWFromZ(dstRZ, 1);
        nynlRet[i]->ERZ = dstRZ;
        nynlRet[i]->EW = EW;

        // 异常判断：如果当前水位已经超过目标水位，纳雨能力为0
        if (p.BRZ >= dstRZ || dstRZ <= -999)
        {
            nynlRet[i]->PE = 0;
            nynlRet[i]->OTW = 0;
            nynlRet[i]->PP = 0;
        }
        else
        {
            // -------------------------------------------------------
            // 第一步：CalcPPF (粗算) - 基于水量平衡
            // -------------------------------------------------------
            auto rPPF = CalcPPF(p, dstRZ);

            // 先暂存粗算结果
            nynlRet[i]->PE = rPPF.PE;
            nynlRet[i]->dW = rPPF.dW;
            nynlRet[i]->OTW = rPPF.OTW;
            nynlRet[i]->PP = rPPF.PP;

            // 如果不需要精算，或者计算出的出库量异常，则直接跳过精算
            if (p.CalcType == 0 || rPPF.OTW <= 0)
            {
                continue;
            }

            // -------------------------------------------------------
            // 第二步：CalcPPZ (精算) - 基于调洪演算
            // -------------------------------------------------------
            // 使用二分法逼近，考虑了水位上涨过程中的动态泄流变化
            auto rPPZ = CalcPPZ(p, dstRZ);

            // -------------------------------------------------------
            // 第三步：结果融合与约束
            // -------------------------------------------------------
            double finalPP = rPPZ.PP;

            // 安全约束：强制精算结果在粗算结果的 [0.8, 1.2] 倍之间
            // 防止调洪演算因参数敏感导致结果过大或过小
            if (finalPP < rPPF.PP * 0.8)
                finalPP = rPPF.PP * 0.8;
            if (finalPP > rPPF.PP * 1.2)
                finalPP = rPPF.PP * 1.2;

            // 应用用户指定的绝对上下限
            if (p.optional.pmin > 0 && finalPP < p.optional.pmin)
                finalPP = p.optional.pmin;
            if (p.optional.pmax > 0 && finalPP > p.optional.pmax)
                finalPP = p.optional.pmax;

            // 更新最终结果
            nynlRet[i]->PE = rPPZ.PE; // 注意：PE通常保留精算的PE，或者应该根据finalPP反推
            nynlRet[i]->dW = rPPZ.dW;
            nynlRet[i]->OTW = rPPZ.OTW;
            nynlRet[i]->PP = finalPP;
        }
    }

    return ret;
}

RRCResult RainRetentionCapacity::CalcPPZ(const RRCParam& p, const double& dstRZ) const
{
    RRCResult ret;
    double BW = pRZ2WLine.GetWFromZ(p.BRZ, 1); // 起始库容
    double EW = pRZ2WLine.GetWFromZ(dstRZ, 1); // 目标库容

    // ==========================================
    // 1. 确定二分查找的边界 (MinPE, MaxPE)
    // ==========================================
    double MinOTQ = 0;
    double MaxOTQ = 0;

    // 估算最大可能的泄流量（假设水位一直维持在目标水位 dstRZ）
    // 宽顶堰流公式: Q = m * ε * B * sqrt(2g) * H^1.5
    // m=0.385, ε=p.KCH, B=p.YhdW
    if (dstRZ > p.DstRZ.YhdE)
    {
        MaxOTQ = 0.385 * p.KCH * std::sqrt(2 * 9.8) * p.YhdW * std::pow(dstRZ - p.DstRZ.YhdE, 1.5);
        // 注意：这里原文乘以 3600*24，意味着估算最大排泄量是按"一天"计算的
        // 这决定了MaxPE的搜索上限量级
        MaxOTQ = MaxOTQ * 3600 * 24;
    }

    // 最小净雨量：假设不泄洪，仅填满剩余库容
    // (EW - BW) * Unit 是将库容转为 m3
    double MinPE = ((EW - BW) * pRZ2WLine.Unit() + MinOTQ) / (p.Area * 1000.0);
    // 最大净雨量：填满库容 + 全天最大泄洪
    double MaxPE = ((EW - BW) * pRZ2WLine.Unit() + MaxOTQ) / (p.Area * 1000.0);

    // 应用用户约束
    if (p.optional.pmin > 0)
        MinPE = p.optional.pmin;
    if (p.optional.pmax > 0)
        MaxPE = p.optional.pmax;
    if (MinPE < 0)
        MinPE = 0;
    if (MaxPE > 999)
        MaxPE = 999;

    // ==========================================
    // 2. 构建概化单位线 (Unit Hydrograph)
    // ==========================================
    // 假设降雨历时为 1小时 (60分钟)
    int Steps = std::ceil(60.0 / p.CalcSteps);
    if (Steps <= 0)
        Steps = 1;

    // 估算汇流总时长 (Steps数)
    // 经验公式：Area / 30.0 是一种概化的汇流时间估算
    int step_factor = std::max(std::floor(60.0 / p.CalcSteps), 1.);
    int sizeT = std::ceil(p.Area / 30.0) * step_factor + 2;
    int peakT = std::max(std::floor(sizeT / 3), 1.); // 假设洪峰出现在 1/3 处

    // 构建三角形单位线
    std::vector<double> vUnit(sizeT, 0.0);
    vUnit[peakT] = (p.Area * 1000) / (p.CalcSteps * 60.0 * 2); // 峰值流量

    // 计算单位线形状（简单的三角形分布）
    double Sum = 0;
    for (int i = peakT + 1; i < sizeT; i++)
    {
        vUnit[i] = vUnit[i - 1] / 2;
        Sum += vUnit[i];
    }
    for (int i = peakT - 1; i > 0; i--)
    {
        vUnit[i] = vUnit[i + 1] / 4;
        Sum += vUnit[i];
    }

    // 归一化单位线：保证单位线对应的总水量等于单位净雨
    if (Sum > 0)
    {
        double scaleFactor = (p.Area * 1000) / (Sum * p.CalcSteps * 60.0);
        for (auto& vu : vUnit)
            vu *= scaleFactor;
    }

    // ==========================================
    // 3. 二分法迭代计算 (Binary Search)
    // ==========================================
    ret.PE = MinPE;
    ret.PP = 0;
    ret.OTW = 0;
    double Loop = 0;

    std::vector<double> vQVal; // 存储入库流量过程

    while (MaxPE - MinPE > 0.1 && Loop <= 100)
    {
        ret.PE = (MaxPE + MinPE) / 2;

        // 3.1 净雨 -> 入库流量过程 (卷积计算)
        vQVal.assign(sizeT + Steps + 1, 0.0);
        double StepRainVal = ret.PE / Steps; // 假设净雨在60分钟内均匀分布

        for (int i = 0; i < Steps; i++)
        {
            for (int j = 0; j < sizeT; j++)
            {
                if (i + j < vQVal.size())
                {
                    vQVal[i + j] += vUnit[j] * StepRainVal;
                }
            }
        }

        // 3.2 调洪演算 (Level Pool Routing)
        double RZ = p.BRZ;
        double W = pRZ2WLine.GetWFromZ(RZ, 1);
        bool bExceed = false;
        ret.OTW = 0;

        for (auto& q_in : vQVal) // q_in 单位: m3/s (假设)
        {
            // 计算当前水位的出库流量
            double q_out = 0;
            if (RZ > p.DstRZ.YhdE)
            {
                // 宽顶堰公式
                q_out = 0.385 * p.KCH * std::sqrt(2 * 9.8) * p.YhdW * std::pow(RZ - p.DstRZ.YhdE, 1.5);
            }

            // 累加出库水量
            ret.OTW += q_out * p.CalcSteps * 60.0 / pRZ2WLine.Unit();

            // 水量平衡方程: W(t+1) = W(t) + (Qin - Qout) * dt
            W += (q_in - q_out) * p.CalcSteps * 60.0 / pRZ2WLine.Unit();

            // 更新水位
            RZ = pRZ2WLine.GetZFromW(W, 1);

            // 判断是否超限
            if (RZ >= dstRZ)
            {
                bExceed = true;
                // 只要瞬间水位超限，即认为该降雨量不可行（保守策略）
                // 若允许短时超限，可在此调整逻辑
            }
        }
        ret.dW = EW - BW + ret.OTW; // 实际上 dW = FinalW - InitialW

        // 3.3 调整二分范围
        if (bExceed)
            MaxPE = ret.PE; // 雨太大了，减小上限
        else
            MinPE = ret.PE; // 还能多下点，提高下限

        Loop++;
    }

    // ==========================================
    // 4. 反推降雨量 (Hydrology Back-Calculation)
    // ==========================================
    // 根据最终确定的净雨 PE，结合土壤含水量 Pa，反推降雨量 PP
    ret.PP = pPaPRLine.GetP(p.Pa, ret.PE, p.Wm);

    // 结果保留小数位
    ret.PE = std::round(ret.PE * 100) / 100;
    ret.PP = std::round(ret.PP * 100) / 100;
    ret.dW = std::round(ret.dW * 10000) / 10000;
    ret.OTW = std::round(ret.OTW * 10000) / 10000;
    return ret;
}


RRCResult RainRetentionCapacity::CalcPPF(const RRCParam& p, const double& dstRZ) const
{
    RRCResult ret;
    double BW = pRZ2WLine.GetWFromZ(p.BRZ, 1);
    double EW = pRZ2WLine.GetWFromZ(dstRZ, 1);

    // 1. 计算预计出库水量 (OTW)
    double OTQ = 0; // 平均出库流量 m3/s

    // 优先级1: 用户直接指定了总泄量
    if (p.optional.WCH > 0)
    {
        ret.OTW = p.optional.WCH;
    }
    // 优先级2: 指定了泄流时长，根据水位差估算平均泄量
    else if (dstRZ > p.DstRZ.YhdE && p.optional.TCH > 0 && p.KCH > 0)
    {
        // 简化假设：以 (目标水位 - 堰顶) 的水头恒定泄流
        // 这是一个比较激进的估计（高估泄量），因为水位是逐渐上涨的
        // 如果为了安全，这里应该用 (StartHead + EndHead)/2 或者更保守的值
        double head = dstRZ - p.DstRZ.YhdE;
        OTQ = 0.385 * p.KCH * std::sqrt(2 * 9.8) * p.YhdW * std::pow(head, 1.5);

        // 转换体积: m3/s * 分钟 * 60 / 单位换算
        ret.OTW = OTQ * p.optional.TCH * 60 / pRZ2WLine.Unit();
    }
    else
    {
        ret.OTW = 0;
    }

    // 2. 计算允许的总净雨体积 (Delta W + Outflow)
    // 逻辑: 允许进来的水 = 坑里剩下的空间 + 期间能流走的水
    ret.dW = EW - BW + ret.OTW; // 注意：这里的dW变量名可能略有歧义，实际代表 Total Allowed Inflow Volume

    // 3. 换算为净雨深 PE (mm)
    double PE = ret.dW * pRZ2WLine.Unit() / (p.Area * 1000.0);

    // 4. 反推降雨量 PP (mm)
    double PP = pPaPRLine.GetP(p.Pa, PE, p.Wm);

    // 格式化输出
    ret.PE = std::round(PE * 100) / 100;
    ret.PP = std::round(PP * 100) / 100;
    ret.OTW = std::round(ret.OTW * 10000) / 10000;
    return ret;
}

void RRCResult::Print() const
{
    std::cout << "{\n  BRZ:" << BRZ;
    std::cout << "\n  BW:" << BW;
    std::cout << "\n  ERZ:" << ERZ;
    std::cout << "\n  EW:" << EW;
    std::cout << "\n  PP:" << PP;
    std::cout << "\n  PE:" << PE;
    std::cout << "\n  dW:" << dW;
    std::cout << "\n  OTW:" << OTW << "\n}," << std::endl;
}

#ifndef NDEBUG
struct PaPR
{
    double extVal;
    std::vector<double> PP; // 降雨量
    std::vector<double> R;  // 径流
};

std::vector<PaPR> PAPRs = {{

                               0,
                               {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200},
                               {0, 0.88, 2.52, 5.013333, 8.486667, 13.093333, 19.1, 26.946667, 37.973333, 56.993333, 76.446667}},
                           {24, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 1.513333, 3.886667, 7.22, 11.68, 17.493333, 25.086667, 35.553333, 53.886667, 73.326667, 92.806667}},
                           {48, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 2.58, 6.206667, 11.026667, 17.326667, 25.626667, 38.246667, 57.64, 77.08, 96.56, 116.06}},
                           {72, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 3.946667, 9.26, 16.24, 25.66, 41.94, 61.333333, 80.786667, 100.273333, 119.786667, 139.32}},
                           {96, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 5.946667, 14.02, 26.28, 45.54, 64.946667, 84.42, 103.933333, 123.466667, 143.02, 162.58}},
                           {120, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 10.913333, 29.793333, 49.006667, 68.426667, 87.946667, 107.506667, 127.08, 146.66, 166.246667, 185.84}}};

std::vector<double> RZs = {444.5, 445, 445.5, 446, 446.5, 447, 447.17, 447.5, 448, 448.5, 449, 449.5, 450, 450.5, 451, 451.5, 452, 452.5, 453, 453.5, 454, 454.5, 455, 455.5, 456, 456.5, 457, 457.5, 458, 458.44, 458.5, 459, 459.05, 459.31, 459.5, 460};
std::vector<double> Ws = {0, 0.000227, 0.001398, 0.003565, 0.006848, 0.01135, 0.0133, 0.01705, 0.024231, 0.032963, 0.04316, 0.054616, 0.067059, 0.080542, 0.095294, 0.111263, 0.128361, 0.146621,
                          0.166056, 0.18657, 0.208026, 0.230437, 0.253783, 0.277973, 0.30299, 0.328989, 0.356011, 0.384258, 0.413783, 0.4407, 0.444343, 0.476025, 0.4793, 0.4964, 0.508918, 0.543015};
#endif

void RainRetentionCapacity::TestPaPRLine()
{
#ifndef NDEBUG
    for (auto& paPR : PAPRs)
    {
        PairsL tmp;
        tmp.SetData(paPR.PP, paPR.R);
        pPaPRLine.AddLine(paPR.extVal, tmp);
    }

    SLOG_DEBUG("\n最大Pa值:{}", pPaPRLine.GetMaxPa());

    // 测试1: 验证离散点计算
    {
        const double testPa = 24;
        const double testP = 40;
        const double expectedR = 3.886667;
        const double calculatedR = pPaPRLine.GetR(testPa, testP);
        SLOG_DEBUG(R"(
测试1 : 离散点计算(Pa = {:.6f}, P = {:.6f})
预期径流量: {:.6f}
计算径流量: {:.6f}
绝对误差: {:.6f})",
                   testPa,
                   testP,
                   expectedR,
                   calculatedR,
                   std::abs(calculatedR - expectedR));
    }

    // 测试2: 反向查询Pa值
    {
        const double testPa = 24;
        const double testP = 40;
        const double testR = 3.886667;
        const double calculatedPa = pPaPRLine.GetPA(testP, testR);
        SLOG_DEBUG(R"(
测试2 : 反向查询(P = {:.6f}, R = {:.6f})
预期Pa值:{:.6f}
计算Pa值:{:.6f})",
                   testP,
                   testR,
                   testPa,
                   calculatedPa);
    }

    // 测试3: 非离散点插值计算
    {
        const double testPa2 = 36;
        const double testP2 = 75;
        SLOG_DEBUG(R"(
测试3 : 插值计算(Pa = {:.6f}, P = {:.6f})
计算径流量:{:.6f})",
                   testPa2,
                   testP2,
                   pPaPRLine.GetR(testPa2, testP2));
    }

    // 测试4: 边界情况测试 (Pa=0)
    {
        const double testPa3 = 0;
        const double testP3 = 100;
        SLOG_DEBUG(R"(
测试4 : 边界情况(Pa = {:.6f}, P = {:.6f})
计算径流量:{:.6f}
)",
                   testPa3,
                   testP3,
                   pPaPRLine.GetR(testPa3, testP3));
    }

    // 测试5: 极端情况测试 (Pa=120, P=200)
    {
        const double testPa4 = 120;
        const double testP4 = 200;
        SLOG_DEBUG(R"(
测试5 : 极端情况(Pa = {:.6f}, P = {:.6f})
计算径流量:{:.6f})",
                   testPa4,
                   testP4,
                   pPaPRLine.GetR(testPa4, testP4));
    }

    // 测试6: 获取总雨量P
    {
        const double testPa5 = 48;
        const double testR2 = 38.246667;
        SLOG_DEBUG(R"(
测试6 : 获取总雨量P(Pa = {:.6f}, R = {:.6f})
计算总雨量:{:.6f})",
                   testPa5,
                   testR2,
                   pPaPRLine.GetP(testPa5, testR2));
    }

    // 测试7: 单条曲线行为模拟
    {
        PaPR_L singlePaPrLine;
        PairsL singleLine;
        singleLine.SetData({0, 0, 50, 30, 100, 80, 150, 130});
        singlePaPrLine.AddLine(50, singleLine);
        SLOG_DEBUG(R"(
测试7: 单条曲线行为
单条曲线 (Pa=50, P=120) 径流量:{:.6f}
单条曲线 (P=120, R=75) Pa值:{:.6f})",
                   singlePaPrLine.GetR(50, 120),
                   singlePaPrLine.GetPA(120, 75));
    }
#endif
}

void RainRetentionCapacity::TestRZWLine()
{
#ifndef NDEBUG

#endif
}

void RainRetentionCapacity::TestModel()
{
#ifndef NDEBUG

    pRZ2WLine.SetData(RZs, Ws);
    for (auto& paPR : PAPRs)
    {
        PairsL tmp;
        tmp.SetData(paPR.PP, paPR.R);
        pPaPRLine.AddLine(paPR.extVal, tmp);
    }
    RRCParam p;
    p.BRZ = 447;          // 起始水位(m)
    p.Area = 5;           // 流域面积(km2)
    p.Wm = 120;           // 最大蓄水量(mm)
    p.Pa = 20;            // 前期影响雨量(mm)
    p.DstRZ.BadE = 460;   // 坝顶高程(m)
    p.YhdW = 2;           // 溢洪道宽度(m)
    p.DstRZ.YhdE = 454;   // 溢洪道高程(m)
    p.DstRZ.SjRZ = 456;   // 设计洪水位(m)
    p.DstRZ.JhRZ = 452;   // 校核洪水位(m)
    p.KCH = 0.6;          // 流量系数
    p.optional.TCH = 60;  // 出流时间 分钟
    p.optional.WCH = -1;  // 出流量 百万方
    p.optional.pmin = -1; // 最小降雨(mm)
    p.optional.pmax = -1; // 最大降雨(mm)
    p.CalcSteps = 15;     // 计算步长 分钟
    p.CalcType = 0;       // 0 反算 1 正算

    RRCResultSet ret = CalcRRC(p);
    if (p.CalcType)
    {
        std::cout << "========正算========" << std::endl;
    }
    else
    {
        std::cout << "========反算========" << std::endl;
    }

    std::vector<RRCResult> rrcSet = {
        ret.YhdE,
        ret.JhRZ,
        ret.SjRZ,
        ret.BadE
    };

    for (auto& r : rrcSet)
    {
        r.Print();
    }
#endif
}