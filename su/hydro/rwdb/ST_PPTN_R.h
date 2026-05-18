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
#include <su_macro.h>
namespace RWDB
{
/**
 * @brief 天气状况
 */
enum class eWeather
{
    Invalid = 0,
    Snowy = 5, // 雪
    RainyAndSnowy = 6, // 雨夹雪
    Rainy = 7, // 雨
    Cloudy = 8,// 阴
    Sunny = 9, // 晴
};
struct ST_PPTN_R
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
    /**
     * @brief 降雨数据,按左开右闭区间 (t−step, t]
     *  重采样到指定时间窗口,
     *  如果输入频率高于时间窗口,做累加
     *  如果输入平吕低于时间窗口,均匀分摊到各窗口
     *  输出每个聚合窗口结束时刻对应的累计雨量.
     *
     * @param tm2rain 原始降雨序列
     * @param bt 开始时间戳
     * @param et 结束时间戳
     * @param intv 时间步长 秒
     * @return 输出结果会从bt+step开始,表示从bt到 bt+step内的累计雨量
     */
    std::map<std::time_t, double> Resample(const std::map<std::time_t, double>& tm2rain, const std::time_t& bt, const std::time_t& et, const std::time_t& intv);

    /**
     * @brief 危险区(或其他)关联的雨量站中, 降雨量最大的站
     * @param code2stcds 目标关联的雨量站
     * @param stcd2tm2drp 雨量站的降雨信息
     * @return
     */
    std::map<std::string, std::string> MaxDrpStation(const std::map<std::string, std::vector<std::string>>& code2stcds, const std::map<std::string, std::map<std::time_t, float>>& stcd2tm2drp);
};


}

#endif  // ST_PPTN_R_H
