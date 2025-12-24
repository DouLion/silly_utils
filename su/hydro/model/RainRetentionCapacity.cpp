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

std::vector<CalcResult> RainRetentionCapacity::Forward(ForwardParam& p)
{
    return {};
}
std::vector<CalcResult> RainRetentionCapacity::Inverse(InverseParam& p)
{
    return {};
}
std::vector<CalcResult> RainRetentionCapacity::CalcNYNL(CalcParam& p)
{
    // 目标水位数组
    std::vector pTargetRZ = {p.Yhdgc, p.Xxsw, p.Sjsw, p.Bdgc};
    std::vector<CalcResult> nynlRet(pTargetRZ.size());

    // 计算起始库容
    double BeginW = pRZ2WLine.GetWFromZ(p.BeginRZ, 1);

    // 循环计算4个目标水�?
    for (int i = 0; i < pTargetRZ.size(); i++)
    {
        nynlRet[i] = {};
        nynlRet[i].BeginRZ = p.BeginRZ;
        nynlRet[i].BeginW = BeginW;

        double EndRZ = pTargetRZ[i];
        double EndW = pRZ2WLine.GetWFromZ(EndRZ, 1);
        nynlRet[i].EndRZ = EndRZ;
        nynlRet[i].EndW = EndW;

        if (p.BeginRZ >= EndRZ || EndRZ <= 0)
        {
            // 无效情况，PP设为0
            nynlRet[i].PE = 0;
            nynlRet[i].OTW = 0;
            nynlRet[i].PP = 0;
        }
        else
        {
            // 反算
            // const {PP : rPPF.PP, PE : PE1, dW : dW1, OTQ : OTQ1, OTW : OTW1} = CalcPPF(BeginRZ, KCH, TCH, WCH, pRZ2WLine, pPaPrLine, Pa, Wm, Area, Bdgc, Yhdk, Yhdgc, EndRZ);
            auto rPPF = CalcPPF(p);
            nynlRet[i].PE = rPPF.PE;
            nynlRet[i].dW = rPPF.dW;
            nynlRet[i].OTW = rPPF.OTW;
            nynlRet[i].PP = rPPF.PP;

            if (p.CalcType == 0 || rPPF.OTW <= 0)
            {
                continue;
            }

            // 第二次计算PP（使用相同的参数�?
            // const {PP : rPPZ.PP, PE : rPPZ.PE, dW : dW2, OTW : OTW2} = CalcPPZ(BeginRZ, KCH, pRZ2WLine, pPaPrLine, Pa, Wm, pmin, pmax, CalcSteps, Area, Bdgc, Yhdk, Yhdgc, EndRZ);
            auto rPPZ = CalcPPZ(p);

            // 应用范围限制
            double finalPP = rPPZ.PP;
            if (rPPZ.PP < rPPF.PP * 0.8)
                finalPP = rPPF.PP * 0.8;
            if (rPPZ.PP > rPPF.PP * 1.2)
                finalPP = rPPF.PP * 1.2;

            if (rPPZ.PP < p.pmin && p.pmin > 0)
                finalPP = p.pmin;
            if (rPPZ.PP > p.pmax && p.pmax > 0)
                finalPP = p.pmax;
            nynlRet[i].PE = rPPZ.PE;
            nynlRet[i].dW = rPPZ.dW;
            nynlRet[i].OTW = rPPZ.OTW;
            nynlRet[i].PP = finalPP;
        }
    }

    return nynlRet;
}
CalcResult RainRetentionCapacity::CalcPPZ(CalcParam& p) const
{
    // 获取起始和结束库�?
    double BeginW = pRZ2WLine.GetWFromZ(p.BeginRZ, 1);
    double EndW = pRZ2WLine.GetWFromZ(p.EndRZ, 1);

    // 初始化溢洪道流量和净雨量范围
    double MinOTQ = 0;
    double MaxOTQ = 0;

    // 计算最大溢洪道流量（如果目标水位高于溢洪道高程�?
    if (p.EndRZ > p.Yhdgc)
    {
        MaxOTQ = 0.385 * p.KCH * std::sqrt(2 * 9.8) * p.Yhdk * std::pow(p.EndRZ - p.Yhdgc, 1.5);
        MaxOTQ = MaxOTQ * 3600 * 24;  // 转换为立方米/小时
    }

    // 计算净雨量(PE)的最小值和最大�?
    double MinPE = ((EndW - BeginW) * pRZ2WLine.Unit() + MinOTQ) / (p.Area * 1000.0);
    double MaxPE = ((EndW - BeginW) * pRZ2WLine.Unit() + MaxOTQ) / (p.Area * 1000.0);

    if (p.pmin > 0)
    {
        MinPE = p.pmin;
    }

    if (p.pmax > 0)
    {
        MaxPE = p.pmax;
    }

    // 确保净雨量在合理范围内
    if (MinPE < 0)
        MinPE = 0;
    if (MaxPE > 999)
        MaxPE = 999;

    double PE = MinPE;
    double PP = 0;
    double OTW = 0;
    double OutQ = 0;
    double dW = 0;

    // 计算时间步数
    double Steps = std::ceil(60.0 / p.CalcSteps);
    if (Steps <= 0)
        Steps = 1;

    // 初始化流量数�?
    std::vector<double> vQVal;
    std::vector<int> vUnit;
    double StepRainVal = 0;
    double Loop = 0;

    // 概化单位线处�?
    int step = std::max(std::floor(60 / p.CalcSteps), 1.);
    int sizeT = std::ceil(p.Area / 30.0) * step + 2;
    int maxT = std::max(std::floor(sizeT / 3), 1.);

    // 初始化单位线
    vUnit.resize(sizeT);
    vUnit[maxT] = (p.Area * 1000) / (p.CalcSteps * 60.0 * 2);

    // 计算单位线总和
    double Sum = 0;
    for (double i = maxT + 1; i < sizeT; i++)
    {
        vUnit[i] = vUnit[i - 1] / 2;
        Sum += vUnit[i];
    }
    for (double i = maxT - 1; i > 0; i--)
    {
        vUnit[i] = vUnit[i + 1] / 4;
        Sum += vUnit[i];
    }

    // 调整单位
    if (Sum > 0)
    {
        double scaleFactor = (p.Area * 1000) / (Sum * p.CalcSteps * 60.0);
        for (auto vu : vUnit)
        {
            vu = vu * scaleFactor;
        }
    }

    // 使用二分法迭代计算净雨量
    while (MaxPE - MinPE > 0.1 && Loop <= 100)
    {
        PE = (MaxPE + MinPE) / 2;
        // 计算流量过程
        vQVal.resize(sizeT + Steps + 1);
        StepRainVal = PE / Steps;

        for (double i = 0; i < Steps; i++)
        {
            for (double j = 0; j < sizeT; j++)
            {
                if (i + j < vQVal.size())
                {
                    vQVal[i + j] += vUnit[j] * StepRainVal;
                }
            }
        }

        // 模拟洪水演进过程
        double RZ = p.BeginRZ;
        double W = pRZ2WLine.GetWFromZ(RZ, 1);
        double bExceed = false;

        OTW = 0;
        for (auto& q : vQVal)
        {
            // 计算溢洪道流量
            OutQ = 0;
            if (RZ > p.Yhdgc)
            {
                OutQ = 0.385 * p.KCH * std::sqrt(2 * 9.8) * p.Yhdk * std::pow(RZ - p.Yhdgc, 1.5);
            }
            OTW += OutQ * p.CalcSteps * 60.0 / pRZ2WLine.Unit();

            // 更新库容
            W += (q - OutQ) * p.CalcSteps * 60.0 / pRZ2WLine.Unit();

            // 更新水位
            RZ = pRZ2WLine.GetZFromW(W, 1);

            // 检查是否超过目标水位
            if (RZ >= p.EndRZ)
            {
                bExceed = true;
                // break;
            }
        }
        dW = EndW - BeginW + OTW;

        // 调整净雨量范围
        if (bExceed)
        {
            MaxPE = PE;
        }
        else
        {
            MinPE = PE;
        }

        Loop++;
    }

    CalcResult ret;
    // 计算最终降雨量PP
    ret.PP = pPaPrLine.GetP(p.Pa, PE, p.Wm);
    ret.PE = std::round(PE * 100) / 100;
    ret.PP = std::round(PP * 100) / 100;
    ret.dW = std::round(dW * 10000) / 10000;
    ret.OTW = std::round(OTW * 10000) / 10000;
    return ret;
}
CalcResult RainRetentionCapacity::CalcPPF(CalcParam& p)
{
    double BeginW = pRZ2WLine.GetWFromZ(p.BeginRZ, 1);
    double EndW = pRZ2WLine.GetWFromZ(p.EndRZ, 1);

    double OTQ = 0;
    double OTW = 0;
    double dW = 0;

    // 计算溢洪道流量
    if (p.WCH > 0)
    {
        OTW = p.WCH;
    }
    else if (p.EndRZ > p.Yhdgc && p.TCH && p.KCH)
    {
        OTQ = 0.385 * p.KCH * std::sqrt(2 * 9.8) * p.Yhdk * std::pow((p.EndRZ - p.Yhdgc), 1.5);
        OTW = OTQ * p.TCH * 60 / pRZ2WLine.Unit();  // 转换为立方米/小时
    }

    // 计算净雨量PE
    dW = EndW - BeginW + OTW;
    double PE = dW * pRZ2WLine.Unit() / (p.Area * 1000.0);

    // 计算降雨量PP
    double PP = pPaPrLine.GetP(p.Pa, PE, p.Wm);
    CalcResult ret;
    ret.PE = std::round(PE * 100) / 100;
    ret.PP = std::round(PP * 100) / 100;
    ret.OTQ = std::round(OTQ * 100) / 100;
    ret.OTW = std::round(OTW * 10000) / 10000;
    ret.dW = dW;
    return ret;
}

void RainRetentionCapacity::Test()
{
    std::vector<double> RZs = {444.5, 445, 445.5, 446, 446.5, 447, 447.17, 447.5, 448, 448.5, 449, 449.5, 450, 450.5, 451, 451.5, 452, 452.5, 453, 453.5, 454, 454.5, 455, 455.5, 456, 456.5, 457, 457.5, 458, 458.44, 458.5, 459, 459.05, 459.31, 459.5, 460};
    std::vector<double> Ws = {0,        0.000227, 0.001398, 0.003565, 0.006848, 0.01135,  0.0133,  0.01705,  0.024231, 0.032963, 0.04316,  0.054616, 0.067059, 0.080542, 0.095294, 0.111263, 0.128361, 0.146621,
                              0.166056, 0.18657,  0.208026, 0.230437, 0.253783, 0.277973, 0.30299, 0.328989, 0.356011, 0.384258, 0.413783, 0.4407,   0.444343, 0.476025, 0.4793,   0.4964,   0.508918, 0.543015};
    struct PaPR
    {
        double extVal;
        std::vector<double> PP;  // 降雨量
        std::vector<double> R;   // 径流
    };
    std::vector<PaPR> PAPRs = {
        {

            0, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 0.88, 2.52, 5.013333, 8.486667, 13.093333, 19.1, 26.946667, 37.973333, 56.993333, 76.446667}},
        {
            24, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 1.513333, 3.886667, 7.22, 11.68, 17.493333, 25.086667, 35.553333, 53.886667, 73.326667, 92.806667}},
        {
            48, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 2.58, 6.206667, 11.026667, 17.326667, 25.626667, 38.246667, 57.64, 77.08, 96.56, 116.06}},
        {
            72, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 3.946667, 9.26, 16.24, 25.66, 41.94, 61.333333, 80.786667, 100.273333, 119.786667, 139.32}},
        {
            96, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 5.946667, 14.02, 26.28, 45.54, 64.946667, 84.42, 103.933333, 123.466667, 143.02, 162.58}},
        {
            120, {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200}, {0, 10.913333, 29.793333, 49.006667, 68.426667, 87.946667, 107.506667, 127.08, 146.66, 166.246667, 185.84}}
    };

    pRZ2WLine.SetData(RZs, Ws);
    for (auto& paPR : PAPRs)
    {
        PairsL tmp;
        tmp.SetData( paPR.PP, paPR.R);
        pPaPrLine.AddLine(paPR.extVal, tmp);
    }
    CalcParam p;
    p.BeginRZ= 447;       // 起始水位(m)
    p.Area= 5;          // 流域面积(km2)
    p.Wm= 120;           // 最大蓄水量(mm)
    p.Pa= 20;             // 前期影响雨量(mm)
    p.Bdgc= 460;          // 坝顶高程(m)
    p.Yhdk= 2;           // 溢洪道宽度(m)
    p.Yhdgc= 454;         // 溢洪道高程(m)
    p.Sjsw= 456;          // 设计洪水位(m)
    p.Xxsw= 452;          // 校核洪水位(m)
    p.KCH= 0.6;           // 流量系数
    p.TCH= 60;             //出流时间 分钟
    p.WCH= -1;             //出流量 百万方
    p.pmin= -1;             //最小降雨(mm)
    p.pmax= -1;             //最大降雨(mm)
    p.CalcSteps= 15;             //计算步长 分钟
    p.CalcType= 1;             //0 反算 1 正算

    std::vector<CalcResult> ret = CalcNYNL(p);

    for (auto& r: ret)
    {
        r.Print();
    }

}
