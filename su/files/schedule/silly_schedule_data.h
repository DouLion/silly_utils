/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-11
 * @file: silly_schedule_data.cpp
 * @description: silly_schedule_data 头文件
 * @version: v1.0.1 2025-12-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SCHEDULE_DATA_H
#define SILLY_UTILS_SILLY_SCHEDULE_DATA_H
#include <files/silly_file.h>
class suScheduleData
{
public:
    struct Desc
    {
        std::time_t each = 300;// 每300秒(5分钟) 一批数据
        std::string name;
        suPath root= suPath("./tmp");
        bool single = false; // 是否作单个文件存储, 单个文件有利于读写分离
    };
};
#endif  // SILLY_UTILS_SILLY_SCHEDULE_DATA_H
