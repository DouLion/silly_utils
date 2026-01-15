/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: PAPR_L.h
 * @description: 降雨-径流关系曲线
 *  Pa 前期降雨 P 本次降雨  R 径流(产流量)
 *  不同前雨下的关系曲线是不一样的，
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef PAPR_L_H
#define PAPR_L_H
#include <hydro/line/M_PAIRS_L.h>
class PaPR_L : public MultiPairsL
{
public:
    // 获取最大降雨量 Pa
    double GetMaxPa() const;
    // 根据降雨量 pa 和总雨量 p 获取径流量 r
    double GetR(const double& pa, const double& p) const;
    // 根据总雨量 p 和径流量 r 获取降雨量 pa
    double GetPA(const double& p, const double& r) const;
    // 根据降雨量 pa 和径流量 r 获取总雨量 p
    double GetP(const double& pa, const double& r, const double& Wm = -1) const;
};
#endif  // PAPR_L_H
