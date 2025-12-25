/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-25
 * @file: XAJ_HN43
 * @description: 赵 提供的 适用湖南的新安江模型的简化版
 * @version: v1.0.1 2025-12-25 dou li yang
 */
#ifndef XAJ_HN43_H
#define XAJ_HN43_H
/*
流域特征	Area, Imp
蒸散发/土壤	Wum, Wlm, Wdm, Wm, Wu0/Wl0/Wd0, C
产流机制	B
水源划分	Sm, Kg, Ki(隐含 Ks = 1-Ki-Kg)
汇流演算	Ci, Cg, LagCS, LagTM
时间设置	ForePeriod, CalcSteps
初始状态	S0, Q0, QS0, QI0, QG0 等
*/

#include <vector>

class XAJ_HN43
{
  public:
    XAJ_HN43()
    {
        Area = 100;  // 流域面积(km²)
        Imp = 0.01;  // 不透水面积比例(0~1)

        Wm = 70;  // 总张力水容量(mm),Wm = Wum + Wlm + Wdm

        Wu0 = 0;   // 上层初始土壤含水量(mm)
        Wl0 = 40;  // 下层初始土壤含水量(mm)
        Wd0 = 10;  // 深层初始土壤含水量(mm)

        Wum = 20;  // 上层最大蓄水容量(mm)
        Wlm = 40;  // 下层最大蓄水容量(mm)
        Wdm = 10;  // 深层最大蓄水容量(mm)

        K = 0.9;  // 自由水总出流系数(部分实现使用,常被 Kg/Ki/Ks 替代)

        B = 0.14;  // 蓄水容量空间分布不均匀性指数(b 参数,0~1)

        C = 0.15;  // 深层蒸散发折减系数(0~1)

        Kg = 0.3;  // 地下径流出流系数(自由水→地下流)
        Ki = 0.4;  // 壤中流出流系数(自由水→壤中流)

        Ci = 0.9;    // 壤中流线性水库消退系数(汇流用)
        Cg = 0.998;  // 地下径流消退系数(接近1,退水极慢)

        Sm = 15;  // 自由水蓄水库最大容量(mm)

        Ex = 1.1;  // 自由水出流非线性指数(标准模型通常为1.0)

        ForePeriod = 24;  // 预报时长(小时)
        CalcSteps = 60;   // 计算时间步长(分钟)

        Fr0 = 0.3;  // 自由水初始蓄满度(或比例)
        S0 = 0;     // 自由水初始蓄量(mm)

        Q0 = 0;   // 初始总流量(m³/s)
        QS0 = 0;  // 初始地表流(m³/s)
        QI0 = 0;  // 初始壤中流(m³/s)
        QG0 = 0;  // 初始地下流(m³/s)

        LagTM = 0;    // 地表汇流滞后时间(小时,此处未启用)
        LagCS = 0.4;  // 地表汇流调蓄系数

        MskX = 0.4;  // 马斯京根法流量权重系数(0~0.5)
        MskK = 1;    // 马斯京根法蓄量常数(小时)

        rlen = 0;  // 河道长度或单位线长度(未使用)

        // 下一时刻状态变量(用于迭代计算)
        Wu1 = 0;  // 上层含水量(t+1)
        Wl1 = 0;  // 下层含水量(t+1)
        Wd1 = 0;  // 深层含水量(t+1)

        Fr1 = 0;  // 自由水蓄满度(t+1)
        S1 = 0;   // 自由水蓄量(t+1)

        Q1 = 0;   // 总流量(t+1)
        QS1 = 0;  // 地表流(t+1)
        QI1 = 0;  // 壤中流(t+1)
        QG1 = 0;  // 地下流(t+1)
    };
    double Area, Wm, Wu0, Wl0, Wd0, Wum, Wlm, Wdm, Imp, K, B, C, Kg, Ki, Ci, Cg, Sm, Ex, ForePeriod, CalcSteps, Fr0, S0, Q0, QS0, QI0, QG0, LagTM, LagCS, MskX, MskK, rlen;
    double Wu1, Wl1, Wd1, Fr1, S1, Q1, QS1, QI1, QG1;

    // 无雨延续洪水预报
    void Calc(std::vector<double>& vRain, std::vector<double>& vResultQ)
    {
        if (Ki + Kg > 0.9)
        {
            double sum = Ki + Kg;
            Ki = Ki / sum;
            Kg = Kg / sum;
        }

        int RainNum = vRain.size();
        double weight = 1;
        std::vector<double> vEM;
        std::vector<double> vE;
        std::vector<double> vR;
        std::vector<double> vRS;
        std::vector<double> vRI;
        std::vector<double> vRG;
        std::vector<double> vQS;
        std::vector<double> vQI;
        std::vector<double> vQG;
        std::vector<double> vQ;

        int PointNum = RainNum + (ForePeriod) * 60 / CalcSteps;
        double EM = 4.8 * CalcSteps / 60.0 / 24;  // 常数日蒸散发 4.8 mm/day(典型湿润地区经验值
        vEM.resize(PointNum, EM);
        vE.resize(PointNum, 0);
        vR.resize(PointNum, 0);
        vRS.resize(PointNum, 0);
        vRI.resize(PointNum, 0);
        vRG.resize(PointNum, 0);
        vQS.resize(PointNum, 0);
        vQI.resize(PointNum, 0);
        vQG.resize(PointNum, 0);
        vQ.resize(PointNum, 0);
        vResultQ.clear();
        vResultQ.resize(PointNum, 0);

        if (PointNum <= 0)
        {
            return;
        }

        CalcCore(vRain.data(), RainNum, 1, vEM.data(), vE.data(), vR.data(), vRS.data(), vRI.data(), vRG.data(), vQS.data(), vQI.data(), vQG.data(), vQ.data(), PointNum);

        if (MskK > 0)
        {
            Xaj3_LL(PointNum, vQ.data(), vResultQ.data(), LagTM, LagCS);
        }
        else
        {
            vResultQ.swap(vQ);
        }
    }

    void CalcCore(double* pRain,  // 降雨序列
               int RainNum,    // 降雨时段数
               double weight,  // 雨量站权重
               double* pEM,    // 潜在蒸散发序列
               double* pE,     // 输出：实际蒸散发(累计)
               double* pR,     // 输出：总产流量(mm)
               double* pRS,    // 输出：地表产流(mm)
               double* pRI,    // 输出：壤中流产流(mm)
               double* pRG,    // 输出：地下产流(mm)
               double* pQS,    // 输出：地表流量(m³/s)
               double* pQI,    // 输出：壤中流流量(m³/s)
               double* pQG,    // 输出：地下流流量(m³/s)
               double* pQ,     // 输出：总流量(m³/s)
               int QNum        // 要计算的总时段数(预报长度)
    )
    {
        if (RainNum <= 0 || QNum <= 0)
        {
            return;
        }
        int i, j, jj;
        double mm, ms, dd, cs, ci, cg;
        double c0, c1, c2, c7, c8, c9, ki, kg, q1, ddt;
        double t, X, ss, q, id;
        double gd;
        double au;
        double d, e1;
        double e2 = 0, e, tq2;
        double tq1;
        double w;
        double qg, qi, qs;
        double s;
        double q11;
        double U, a, fr;
        double em, pp, pe;
        double rb;
        double r = 0, r1 = 0, r2, r3, rs, rg, ri;
        double ln, g;
        // VBto upgrade warning: WU As CComVariant	OnWrite(float, short)	OnRead(float)
        double WD, WU, WL;

        ddt = CalcSteps / 60.0;

        std::vector<double> vtmpQ;
        vtmpQ.resize(rlen + 1);

        mm = (1 + B) * Wm / (1 - Imp);
        ms = (1 + Ex) * Sm;
        dd = ddt / 24;
        cs = (double)(pow(LagCS, dd));
        ci = (double)(pow(Ci, dd));
        cg = (double)(pow(Cg, dd));
        c9 = MskK * MskX;
        c8 = (double)(0.5 * ddt);
        c7 = MskK - c9 + c8;
        if (c7 == 0)
            return;

        c0 = (c8 - c9) / c7;
        c1 = (c8 + c9) / c7;
        c2 = (MskK - c9 - c8) / c7;

        ki = (double)((1 - pow((1 - (Kg + Ki)), dd)) / (1 + Kg / Ki));
        kg = ki * Kg / Ki;

        q1 = QS0 + QI0 + QG0;
        if (q1 <= 0.)
        {
            q1 = 1;
            QS0 = 0.1;
            QI0 = 0.5;
            QG0 = 0.4;
        }
        w = Wu0 + Wl0 + Wd0;
        WU = Wu0;
        WL = Wl0;
        WD = Wd0;
        qg = QG0;
        qi = QI0;
        qs = QS0;
        s = S0;

        // 预报单元雨量站权重
        q11 = q1 * weight;
        pQ[0] = q11;

        // 对起始流量的实时校正
        qs = weight * QS0;
        qi = weight * QI0;
        qg = weight * QG0;
        WD = Wd0;
        U = weight * Area / (3.6 * ddt);

        if (w >= Wm)
        {
            w = Wm - 0.01;
        }

        a = mm * (1 - pow((1 - w / Wm), (1 / (1 + B))));
        fr = 1 - (1 - Imp) * pow((1 - a / mm), B) - Imp;

        ln = rlen / ddt + 1;
        if (ln > 0)
        {
            for (i = 0; i < ln; i++)
            {
                vtmpQ[i] = q11;
            }
        }

        for (i = 0; i < QNum - 1; i++)
        {
            // Step 1: 计算净雨
            if (i < RainNum)
            {
                em = K * pEM[i];  // 调整后的潜在蒸散发
                pp = pRain[i];
            }
            else
            {
                pp = 0;
            }
            pe = pp - em;  // p arrary // 净雨(可能为负)
            // Step 2: 蒸散发计算(三层)
            if (pe > 0)
            {
                pE[i] += weight * pe;
                // Step 3: 产流计算(蓄满产流 + 不透水面积)
                rb = Imp * pe;  // 不透水面积直接产流(地表)
                // 可透水部分：判断是否蓄满,计算 r(可透水产流量)
                if (w >= Wm)
                {
                    r = pe + w - Wm;
                }
                else
                {
                    // 使用 B 参数计算蓄水容量曲线,求 r
                    a = mm * (1 - pow((1 - w / Wm), (1 / (1 + B))));
                    if (a + pe >= mm)
                    {
                        r = pe + w - Wm;
                    }
                    else
                    {
                        r = pe - Wm + w + Wm * (pow((1 - (pe + a) / mm), (1 + B)));
                    }
                }
                // Step 4: 水源划分(自由水水库 + 非线性出流)
                t = r - rb;
                X = fr;
                fr = t / pe;     // 产流面积比例
                s = X * s / fr;  // 自由水蓄量
                ss = s;
                q = t / fr;
                g = floor(q / 5.) + 1;
                q /= g;
                id = (1. - pow((1. - (kg + ki)), (1. / g))) / (1 + kg / ki);
                gd = (double)(id * kg / ki);
                rs = 0.;  // 计算地表径流
                rg = 0.;  // 计算地下流
                ri = 0.;  // 计算壤中流
                for (jj = 0; jj < g; jj++)
                {
                    if (s >= Sm)
                    {
                        r1 = q + s - Sm;
                        rs = r1 * fr + rs;
                    }
                    else
                    {
                        au = ms * (1 - pow((1 - s / Sm), (1 / (1 + Ex))));
                        if (au + q < ms)
                        {
                            r1 = q - Sm + s + Sm * pow((1 - (q + au) / ms), (1 + Ex));
                            rs = r1 * fr + rs;
                        }
                        else
                        {
                            r1 = q + s - Sm;
                            rs = r1 * fr + rs;
                        }
                    }
                    s = q - r1 + s;
                    r2 = s * id;
                    ri = r2 * fr + ri;
                    r3 = s * gd;
                    rg = r3 * fr + rg;
                    s = (jj + 1) * q + ss - (rs + ri + rg) / fr;
                }
                rs += rb;
            }
            else
            {
                r = 0;
                rs = 0;
                rg = s * kg * fr;
                ri = rg * ki / kg;
                if (fr == 0)
                {
                    return;
                }
                else
                {
                    s -= (rg + ri) / fr;
                }
            }
            d = (double)(WU + pe - r);
            // Step 2: 蒸散发计算(三层)
            if (d >= 0)
            {
                e1 = em;
                e2 = 0.;
                if (d <= Wum)
                {
                    WU = d;
                }
                else
                {
                    WU = Wum;
                    WL += d - Wum;
                    if (WL > Wlm)
                    {
                        WD += WL - Wlm;
                        WL = Wlm;
                        if (WD > Wdm)
                        {
                            WD = Wdm;
                        }
                    }
                }
            }
            else
            {
                e1 = pp + WU;
                WU = 0;
                if (WL / Wlm <= C)
                {
                    e2 = abs(d) * C;
                }
                else
                {
                    e2 = abs(d) * WL / Wlm;
                }
                WL -= e2;
                if (WL < 0)
                {
                    WD += WL;
                    WL = 0;
                }
            }
            w = WU + WL + WD;
            e = e1 + e2;

            if (rg < 0)
            {
                rg = 0;
            }
            if (ri < 0)
            {
                ri = 0;
            }
            if (rs < 0)
            {
                rs = 0;
            }
            // Step 5: 汇流演算(三种水源分别处理)
            qg = (double)(qg * cg + rg * (1 - cg) * U);  // 地下流：线性水库
            qi = (double)(qi * ci + ri * (1 - ci) * U);  // 壤中流：线性水库
            qs = (double)(qs * cs + rs * (1 - cs) * U);  // 地表流：用 LagCS 消退
            if (qg < 0)
            {
                qg = 0;
            }
            if (qi < 0)
            {
                qi = 0;
            }
            if (qs < 0)
            {
                qs = 0;
            }
            tq2 = qs + qi + qg;

            pRS[i] += weight * rs;
            pRI[i] += weight * ri;
            pRG[i] += weight * rg;
            pR[i] += pRS[i] + pRI[i] + pRG[i];
            pQS[i] += qs;
            pQI[i] += qi;
            pQG[i] += qg;
            // Step 6: 河道汇流(Muskingum 多段演进)
            if (rlen > 0)
            {
                for (j = 1; j < ln; j++)
                {
                    tq1 = vtmpQ[j - 1];
                    vtmpQ[j - 1] = (double)tq2;
                    // 使用 vtmpQ 数组模拟河道延迟
                    tq2 = c0 * tq2 + c1 * tq1 + c2 * vtmpQ[j];
                    if (tq2 < 0)
                    {
                        tq2 = 0;
                    }
                }  // j
                tq1 = vtmpQ[ln - 1];
                vtmpQ[ln - 1] = (double)tq2;
            }

            if (pQ[i + 1] < 0.)
            {
                pQ[i + 1] = 0.;
            }
            pQ[i + 1] += tq2;

            if (i == RainNum - 2)
            {
                Wu1 += weight * WU;
                Wl1 += weight * WL;
                Wd1 += weight * WD;
                Fr1 += weight * fr;
                S1 += weight * s;
                Q1 += tq2;
                QS1 += qs;
                QI1 += qi;
                QG1 += qg;
            }
        }  // i
    }
    /// 新安江模型中用于地表径流(或总流量)汇流演算的一个简化模块,其作用是对输入流量过程 Inq 进行滞后与调蓄处理,以模拟水流在坡面或河道中的传播延迟和坦化效应
    void Xaj3_LL(int qnum,     // 流量序列长度(时段数)
                 double* Inq,  // 输入流量过程(m³/s),通常是 CalcCore 输出的 vQ(三水源叠加后)
                 double* Otq,  // 输出流量过程(经汇流演算后)
                 double nLL,   // 滞后时间(Lag Time),单位：时段数(非小时！)
                 double nCS    // 调蓄系数(0~1),控制当前流量与滞后流量的权重 典型值 0.3~0.7。
    )
    {
        int index, i;
        // float* nQQ;
        // 无滞后情况(直接透传)
        if (nLL <= 0)
        {
            for (i = 0; i < qnum; i++)
            {
                Otq[i] = Inq[i];
            }
            return;
        }
        // nCS  越大 → 越依赖近期流量(响应快,洪峰高)
        // nCS 越小 → 越依赖滞后流量(响应慢,洪峰平缓)
        for (i = 1; i < qnum; i++)
        {
            if (i >= nLL)
            {
                index = floor(i - nLL + 0.5);
                if (index > 0 && index < qnum)
                {
                    Otq[i] = Inq[i - 1] * nCS + Inq[index] * (1 - nCS);
                }
            }
        }
    }

    double InterpolateInflow(double currentTime, std::vector<double>& vFlows, double step)
    {
        if (vFlows.empty())
        {
            return 0;
        }
        double index = currentTime / step;
        int lastIndex = floor(index);
        int NextIndex = lastIndex + 1;

        if (NextIndex <= 0)
        {
            return vFlows.front();
        }
        if (NextIndex >= vFlows.size())
        {
            return 0;
        }

        return (vFlows[lastIndex] * (NextIndex - index)) + vFlows[NextIndex] * (index - lastIndex);
    }
};
#endif