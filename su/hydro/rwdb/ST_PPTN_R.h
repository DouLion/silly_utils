/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: ST_PPTN_R
 * @description: 降雨记录表 Precipitation Record
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef ST_PPTN_R_H
#define ST_PPTN_R_H
#include <su_marco.h>
namespace RWDB
{
/**
 * @brief 天气状况
 */
enum eWeather
{
    Invalid = 0,
    Snowy = 5, // 雪
    RainyAndSnowy = 6, // 雨夹雪
    Rainy = 7, // 雨
    Cloudy = 8,// 阴
    Sunny = 9, // 晴
};
struct ST_PPTN
{
    std::string STCD;
    std::time_t TM = 0;
    double DRP = 0.0; // N(5, 1) 时段内降雨量(mm)
    double INTV = 0.0; // DR 时段长, 一般 0.05表示5分钟
    double PDR = 0.0; // DRP 降雨历时
    double DPY = 0.0; // N(5, 1) 前一天(24h)降雨量(mm)
    eWeather WTH = eWeather::Invalid; // TM 时候的天气状况

    /**
     * @brief 时段长转秒/毫秒
     * @param intv 时段长, 习惯上1.05 表示 1小时5分钟
     * @return
     */
    static double INTV2S(const double& intv);
    static double INTV2MS(const double& intv);

};
}

#endif  // ST_PPTN_R_H
