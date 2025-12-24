/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: M_PAIRS_L.h
 * @description: M_PAIRS_L 头文件
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef M_PAIRS_L_H
#define M_PAIRS_L_H
#include <hydro/line/PAIRS_L.h>
class MultiPairsL
{
public:
    MultiPairsL() = default;
    ~MultiPairsL() = default;
    void AddLine(const double& k, const PairsL& pl);
    void Clear();
protected:
    double GetXFromY(const double& k, const double& y, const int& type = 0) const;
    double GetYFromX(const double& k, const double& x, const int& type = 0) const;
    double GetValFromXY(const double& x, const double& y, const int& type = 0) const;

protected:
    std::map<double, PairsL> m_key2Pairs;
};
#endif  // M_PAIRS_L_H
