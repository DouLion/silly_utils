/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: zhao yan wei
 * @date: 2025-12-25
 * @file: XAJ_HN43
 * @description: 赵 提供的 适用湖南的新安江模型的简化版
 * @version: v1.0.1 2025-12-25 zhao yan wei
 */

#ifndef XAJ_HN43_HPP
#define XAJ_HN43_HPP

#include <vector>
#include <cmath>
#include <algorithm>

class XAJ_HN43
{
  public:
    // 1. 时间与基础配置 (Time & Base Config)
    struct ConfigParam
    {
        double Area = 100.0;       // 流域面积(km²)
        double CalcSteps = 60.0;   // 计算时间步长(分钟)
        double ForePeriod = 24.0;  // 预报时长(小时)
        double Imp = 0.01;         // 不透水面积比例(0~1)
    } Config;

    // 2. 土壤与蒸散发参数 (Soil & Evapotranspiration)
    struct SoilParam
    {
        double Wm = 70.0;   // 总张力水容量(mm)
        double Wum = 20.0;  // 上层最大蓄水容量(mm)
        double Wlm = 40.0;  // 下层最大蓄水容量(mm)
        double Wdm = 10.0;  // 深层最大蓄水容量(mm)
        double B = 0.14;    // 蓄水容量空间分布指数
        double C = 0.15;    // 深层蒸散发折减系数
        double K = 0.9;     // 潜在蒸发折算系数
    } Soil;

    // 3. 分水源与产流参数 (Runoff & Source)
    struct SourceParam
    {
        double Sm = 15.0;  // 自由水最大容量(mm)
        double Ex = 1.1;   // 自由水出流非线性指数
        double Kg = 0.3;   // 地下径流出流系数
        double Ki = 0.4;   // 壤中流出流系数
    } Source;

    // 4. 汇流演进参数 (Routing & Muskingum)
    struct RouteParam
    {
        double Ci = 0.9;     // 壤中流消退系数
        double Cg = 0.998;   // 地下径流消退系数
        double LagCS = 0.4;  // 地表汇流调蓄系数 (同时兼作平滑系数)
        double MskX = 0.4;   // 马斯京根权重系数
        double MskK = 1.0;   // 马斯京根蓄量常数
        double rlen = 0.0;   // 特征河长 (决定Muskingum串联段数)
        double LagTM = 0.0;  // Xaj3_LL滞后时间 (小时)
    } Route;

    // 5. 初始状态 (Initial States)
    struct InitStateParam
    {
        double Wu0 = 0.0;   // 上层初始土壤含水量
        double Wl0 = 40.0;  // 下层初始土壤含水量
        double Wd0 = 10.0;  // 深层初始土壤含水量
        double Fr0 = 0.3;   // 自由水初始蓄满度
        double S0 = 0.0;    // 自由水初始蓄量
        double Q0 = 0.0;    // 初始总流量
        double QS0 = 0.0;   // 初始地表流
        double QI0 = 0.0;   // 初始壤中流
        double QG0 = 0.0;   // 初始地下流
    } Init;

    // 6. 结果与输出 (Output Data)
    struct OutputData
    {
        std::vector<double> pE;   // 实际蒸散发
        std::vector<double> pR;   // 总产流量
        std::vector<double> pRS;  // 地表产流量
        std::vector<double> pRI;  // 壤中产流量
        std::vector<double> pRG;  // 地下产流量
        std::vector<double> pQ;   // 最终总流量
    } Out;

  public:
    XAJ_HN43()
    {
    }

    // ==========================================
    // 兼容原版的单雨量站接口
    // ==========================================
    std::vector<double> Calc(const std::vector<double>& vRain)
    {
        std::vector<std::vector<double>> vRains = {vRain};
        std::vector<double> weights = {1.0};
        return CalcMulti(vRains, weights);
    }

    // ==========================================
    // 多雨量站核心接口
    // ==========================================
    std::vector<double> CalcMulti(const std::vector<std::vector<double>>& vRains, const std::vector<double>& weights)
    {
        std::vector<double> emptyRes;
        if (vRains.empty() || vRains[0].empty())
            return emptyRes;

        // 动态计算总步长 (历史降雨 + 预报期)
        int rainNum = static_cast<int>(vRains[0].size());
        int foreNum = static_cast<int>(Config.ForePeriod * 60.0 / Config.CalcSteps);
        int totalSteps = rainNum + foreNum;
        if (totalSteps <= 0)
            return emptyRes;

        PrepareOutput(totalSteps);

        // 临时累加变量 (用于收集加权后的各水源)
        std::vector<double> sumRS(totalSteps, 0.0);
        std::vector<double> sumRI(totalSteps, 0.0);
        std::vector<double> sumRG(totalSteps, 0.0);

        // 处理所有雨量站的产流
        for (size_t i = 0; i < vRains.size(); ++i)
        {
            AddStationRunoff(vRains[i], weights[i], totalSteps, rainNum, sumRS, sumRI, sumRG);
        }

        // 汇流演算
        return ExecuteRouting(totalSteps, weights[0], sumRS, sumRI, sumRG);
    }

  private:
    void PrepareOutput(int totalSteps)
    {
        Out.pE.assign(totalSteps, 0.0);
        Out.pR.assign(totalSteps, 0.0);
        Out.pRS.assign(totalSteps, 0.0);
        Out.pRI.assign(totalSteps, 0.0);
        Out.pRG.assign(totalSteps, 0.0);
        Out.pQ.assign(totalSteps, 0.0);
    }

    // ==========================================
    // 原汁原味：产流与分水源计算 (含三层蒸发与非线性出流)
    // ==========================================
    void AddStationRunoff(const std::vector<double>& vRain, double weight, int totalSteps, int rainNum, std::vector<double>& sumRS, std::vector<double>& sumRI, std::vector<double>& sumRG)
    {
        double WU = Init.Wu0;
        double WL = Init.Wl0;
        double WD = Init.Wd0;
        double w = WU + WL + WD;
        if (w >= Soil.Wm)
            w = Soil.Wm - 0.01;

        double s = Init.S0;
        double fr = Init.Fr0;

        double ddt = Config.CalcSteps / 60.0;
        double dd = ddt / 24.0;
        double em_constant = 4.8 * ddt / 24.0;

        // 约束 Ki + Kg 不大于 0.9 (原版逻辑)
        double localKi = Source.Ki;
        double localKg = Source.Kg;
        if (localKi + localKg > 0.9)
        {
            double sumK = localKi + localKg;
            localKi /= sumK;
            localKg /= sumK;
        }

        double ki = (1.0 - std::pow(1.0 - (localKg + localKi), dd)) / (1.0 + localKg / localKi);
        double kg = ki * localKg / localKi;

        double mm = (1.0 + Soil.B) * Soil.Wm / (1.0 - Config.Imp);
        double ms = (1.0 + Source.Ex) * Source.Sm;

        for (int i = 0; i < totalSteps - 1; i++)
        {
            double pp = (i < rainNum && i < vRain.size()) ? vRain[i] : 0.0;
            double em = Soil.K * em_constant;
            double pe = pp - em;

            double r = 0, rb = 0, rs = 0, ri = 0, rg = 0;

            if (pe > 0)
            {
                Out.pE[i] += weight * pe;
                rb = Config.Imp * pe;

                if (w >= Soil.Wm)
                {
                    r = pe + w - Soil.Wm;
                }
                else
                {
                    double a = mm * (1.0 - std::pow(1.0 - w / Soil.Wm, 1.0 / (1.0 + Soil.B)));
                    if (a + pe >= mm)
                    {
                        r = pe + w - Soil.Wm;
                    }
                    else
                    {
                        r = pe - Soil.Wm + w + Soil.Wm * std::pow(1.0 - (pe + a) / mm, 1.0 + Soil.B);
                    }
                }

                double t = r - rb;
                double X = fr;
                fr = t / pe;

                if (fr > 1e-6)
                {
                    s = X * s / fr;
                    double ss = s;
                    double q = t / fr;

                    int g_steps = static_cast<int>(std::floor(q / 5.0)) + 1;
                    q /= g_steps;
                    double id = (1.0 - std::pow(1.0 - (kg + ki), 1.0 / g_steps)) / (1.0 + kg / ki);
                    double gd = id * kg / ki;

                    for (int jj = 0; jj < g_steps; jj++)
                    {
                        double r1 = 0;
                        if (s >= Source.Sm)
                        {
                            r1 = q + s - Source.Sm;
                            rs = r1 * fr + rs;
                        }
                        else
                        {
                            double au = ms * (1.0 - std::pow(1.0 - s / Source.Sm, 1.0 / (1.0 + Source.Ex)));
                            if (au + q < ms)
                            {
                                r1 = q - Source.Sm + s + Source.Sm * std::pow(1.0 - (q + au) / ms, 1.0 + Source.Ex);
                                rs = r1 * fr + rs;
                            }
                            else
                            {
                                r1 = q + s - Source.Sm;
                                rs = r1 * fr + rs;
                            }
                        }
                        s = q - r1 + s;
                        double r2 = s * id;
                        ri = r2 * fr + ri;
                        double r3 = s * gd;
                        rg = r3 * fr + rg;
                        s = (jj + 1) * q + ss - (rs + ri + rg) / fr;
                    }
                }
                rs += rb;
            }
            else
            {
                if (fr > 1e-6)
                {
                    rg = s * kg * fr;
                    ri = rg * ki / kg;
                    s -= (rg + ri) / fr;
                }
            }

            // 三层蒸发分配 (原版逻辑)
            double d_val = WU + pe - r;
            if (d_val >= 0)
            {
                if (d_val <= Soil.Wum)
                {
                    WU = d_val;
                }
                else
                {
                    WU = Soil.Wum;
                    WL += d_val - Soil.Wum;
                    if (WL > Soil.Wlm)
                    {
                        WD += WL - Soil.Wlm;
                        WL = Soil.Wlm;
                        if (WD > Soil.Wdm)
                            WD = Soil.Wdm;
                    }
                }
            }
            else
            {
                WU = 0;
                double e2 = 0;
                if (WL / Soil.Wlm <= Soil.C)
                {
                    e2 = std::abs(d_val) * Soil.C;
                }
                else
                {
                    e2 = std::abs(d_val) * WL / Soil.Wlm;
                }
                WL -= e2;
                if (WL < 0)
                {
                    WD += WL;
                    WL = 0;
                }
            }
            w = WU + WL + WD;

            // 限制防越界并累加至总集合
            sumRS[i] += weight * std::max(0.0, rs);
            sumRI[i] += weight * std::max(0.0, ri);
            sumRG[i] += weight * std::max(0.0, rg);

            Out.pRS[i] += weight * std::max(0.0, rs);
            Out.pRI[i] += weight * std::max(0.0, ri);
            Out.pRG[i] += weight * std::max(0.0, rg);
            Out.pR[i] += Out.pRS[i] + Out.pRI[i] + Out.pRG[i];
        }
    }

    // ==========================================
    // 汇流演算 (三种水源消退 + 串联河道演算 + Lag平滑)
    // ==========================================
    std::vector<double> ExecuteRouting(int totalSteps, double weight0, const std::vector<double>& sumRS, const std::vector<double>& sumRI, const std::vector<double>& sumRG)
    {
        double ddt = Config.CalcSteps / 60.0;
        double dd = ddt / 24.0;

        double cs = std::pow(Route.LagCS, dd);
        double ci = std::pow(Route.Ci, dd);
        double cg = std::pow(Route.Cg, dd);
        double U = weight0 * Config.Area / (3.6 * ddt);

        // 决定初始初值
        double q1 = Init.QS0 + Init.QI0 + Init.QG0;
        double initQS = Init.QS0, initQI = Init.QI0, initQG = Init.QG0;
        if (q1 <= 0.0)
        {
            q1 = 1.0;
            initQS = 0.1;
            initQI = 0.5;
            initQG = 0.4;
        }

        double qs = weight0 * initQS;
        double qi = weight0 * initQI;
        double qg = weight0 * initQG;
        double q11 = q1 * weight0;

        // --- Muskingum 初始化准备 ---
        double c9 = Route.MskK * Route.MskX;
        double c8 = 0.5 * ddt;
        double c7 = Route.MskK - c9 + c8;

        double c0 = 0, c1 = 0, c2 = 0;
        if (c7 != 0)
        {
            c0 = (c8 - c9) / c7;
            c1 = (c8 + c9) / c7;
            c2 = (Route.MskK - c9 - c8) / c7;
        }

        // 修复原版的数组越界风险: ln 代表串联计算段数
        int ln = static_cast<int>(Route.rlen / ddt) + 1;
        std::vector<double> vtmpQ(std::max(1, ln), q11);

        std::vector<double> vQ(totalSteps, 0.0);
        vQ[0] = q11;

        for (int i = 0; i < totalSteps - 1; i++)
        {
            // 三水源各自水库消退
            qs = std::max(0.0, qs * cs + sumRS[i] * (1.0 - cs) * U);
            qi = std::max(0.0, qi * ci + sumRI[i] * (1.0 - ci) * U);
            qg = std::max(0.0, qg * cg + sumRG[i] * (1.0 - cg) * U);

            double tq2 = qs + qi + qg;

            // 原版的多段演进 (Muskingum 级联)
            if (Route.rlen > 0 && c7 != 0)
            {
                for (int j = 1; j < ln; j++)
                {
                    double tq1 = vtmpQ[j - 1];
                    vtmpQ[j - 1] = tq2;
                    tq2 = c0 * tq2 + c1 * tq1 + c2 * vtmpQ[j];
                    if (tq2 < 0)
                        tq2 = 0;
                }
                vtmpQ[ln - 1] = tq2;
            }

            vQ[i + 1] = std::max(0.0, tq2);
        }

        std::vector<double> vResultQ(totalSteps, 0.0);

        // --- 最终叠加外层的 Lag 滞后平滑 ---
        if (Route.MskK > 0)
        {
            ApplyLagSmoothing(totalSteps, vQ.data(), vResultQ.data(), Route.LagTM, Route.LagCS);
        }
        else
        {
            vResultQ = vQ;
        }

        Out.pQ = vResultQ;
        return vResultQ;
    }

    // ==========================================
    // 原版 Xaj3_LL 纯粹的滑动滞后加权
    // ==========================================
    void ApplyLagSmoothing(int qnum, double* Inq, double* Otq, double nLL, double nCS)
    {
        if (nLL <= 0)
        {
            for (int i = 0; i < qnum; i++)
                Otq[i] = Inq[i];
            return;
        }

        Otq[0] = Inq[0];
        for (int i = 1; i < qnum; i++)
        {
            Otq[i] = 0.0;
            if (i >= nLL)
            {
                int index = static_cast<int>(std::floor(i - nLL + 0.5));
                if (index > 0 && index < qnum)
                {
                    Otq[i] = Inq[i - 1] * nCS + Inq[index] * (1.0 - nCS);
                }
            }
        }
    }
};

#endif  // XAJ_HN43_HPP
