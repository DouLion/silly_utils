/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-09
 * @file: silly_radar.cpp
 * @description: silly_radar 头文件
 * @version: v1.0.1 2025-12-09 dou li yang
 */
#pragma once

#include <files/schedule/silly_schedule_file.h>
#include <radar/format/RadarDataHN43.h>
#include <radar/format/RadarDataSLB.h>

#pragma pack(push, 1)
class NormalRainRaw
{
public:
    // 对应浮点数值 * 10 转short, 降雨只要保留1位小数即可
    int16_t rate = 0;  // 雨强
    int16_t ohp = 0;   // 过去1小时降雨
    int16_t dhp = 0;   // 过去2小时降雨
    int16_t thp = 0;   // 过去3小时降雨
    int16_t ohpf = 0;  // 预报1小时降雨
    int16_t dhpf = 0;  // 预报2小时降雨
    int16_t thpf = 0;  // 预报3小时降雨
};
#pragma pack(pop)


