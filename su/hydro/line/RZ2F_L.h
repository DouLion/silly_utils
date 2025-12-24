/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: RZ2F_L.h
 * @description: 水位-流量关系曲线
 *              水位 z
 *              流量 f
 *              参数 e
 *
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef RZ2F_L_H
#define RZ2F_L_H
#include <hydro/line/M_PAIRS_L.h>
class RZ2F_L : public MultiPairsL
{
public:
    // 根据水位 z 和参数 e 获取流量 f
    double GetF(const double& e, const double& z) const;
    // 根据参数 e 和流量 f 获取水位 z
    double GetZ(const double& e, const double& f) const;
    // 根据水位 z 和流量 f 获取参数 e
    double GetE(const double& z, const double& f) const;
    // 获取最大参数 e 值
    double GetMaxE() const;
    // 获取最大水位 z 值
    double GetMaxZ() const;
    // 获取最大流量 f 值
    double GetMaxF() const;

};
#endif  // RZ2F_L_H
