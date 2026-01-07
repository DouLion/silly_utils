/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: ST_RIVER_R.cpp
 * @description: 河道水情表
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef ST_RIVER_R_H
#define ST_RIVER_R_H
#include <su_macro.h>
namespace RWDB
{
/**
 * @brief 水势
 */
enum class eWaterTrend : char
{
    Invalid = 0,
    Falling = 4,   // 落
    Rising = 5,    // 涨
    Stagnant = 6,  // 平
};

struct ST_RIVER
{
    std::string STCD;
    std::time_t TM;
    double Z = 0.0;                           // N(7,3) 水位 米(m)
    double Q = 0.0;                           // N(9,3)  流量 立方米每秒(m3/s)
    double XSA = 0.0;                         // N(9,3) 断面过水面积 平方米(m2)
    double XSAVV = 0.0;                       // N(5,3) 断面平均流速 米每秒(m/s)
    double XSMXV = 0.0;                       // N(5,3) 断面最大流速 米每秒(m/s)
    eWaterTrend WPTN = eWaterTrend::Invalid;  // 水势
    char MSQMT = 0;                           // 测流方法
    char MSAMT = 0;                           // 测积方法
    char MSVMT = 0;                           // 测速方法
};
}  // namespace RWDB
#endif  // ST_RIVER_R_H
