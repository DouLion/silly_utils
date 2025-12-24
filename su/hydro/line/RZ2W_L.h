/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: RZ2W_L.cpp
 * @description: 水位库容(蓄水量)关系曲线线  参照 库(湖)容曲线表 ST_ZVARL_B
 *              SELECT RZ, W FROM ST_ZVARL_B
 *              水位单位 米
 *              库容(蓄水量)单位 10^6 立方米
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef RZ2W_L_H
#define RZ2W_L_H
#include <hydro/line/PAIRS_L.h>
class RZ2W_L : public PairsL
{
public:
    RZ2W_L() = default;
    RZ2W_L(const double& unit);
    ~RZ2W_L() = default;

    /**
     * 根据水位插值计算库容
     * @param z 水位
     * @param type 插值方式
     * @return
     */
    double GetW( const double& z, const int& type = 4) const;

    /**
     * 根据库容, 插值计算水位
     * @param w 库容
     * @param type 插值方式
     * @return
     */
    double GetZ( const double& w, const int& type = 4) const ;
protected:
    double m_unit = 1e6; // 单位 这个现在似乎没有吊用
};
#endif  // RZ2W_L_H
