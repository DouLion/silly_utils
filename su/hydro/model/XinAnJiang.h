/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-25
 * @file: XinAnJiang.h
 * @description: 新安江模型
 * @version: v1.0.1 2025-12-25 dou li yang
 */
#ifndef XIN_AN_JIANG_H
#define XIN_AN_JIANG_H
#include <hydro/model/xaj/EvaEstimate.h>
#include <hydro/model/xaj/FlowRoute.h>
#include <hydro/model/xaj/RainRunoffSimu.h>
#include <hydro/model/xaj/RunoffSourceSeparate.h>
#include <hydro/model/xaj/FloodForecast.h>
#include <hydro/model/xaj/XAJ_HN43.hpp>
/* 新安江四个模块的逻辑流程
适应中国南方湿润地区：蓄满产流 + 三水源结构

Rainfall–Runoff Simulation	雨下够了，土饱和了，才开始“产水”
Evapotranspiration Estimation	先扣掉被蒸发/蒸腾掉的水，剩下的才算数
Runoff Source Separation	把产的水分成“快、中、慢”三种来源
Flow Routing and Flood Forecasting	让三种水按不同速度“跑”到河口，合成洪水过程

降雨 (P)
   ↓
蒸散发估算 (E) → 得到 净雨 (P' = P - E)
   ↓
降雨–产流模拟 → 得到 总产流量 (R)
   ↓
水源划分 → 得到 Rs, Rss, Rg
   ↓
汇流演算 → 得到 出口流量 Q(t)
   ↓
洪水预报 / 水资源管理
 */

#endif  // XIN_AN_JIANG_H
