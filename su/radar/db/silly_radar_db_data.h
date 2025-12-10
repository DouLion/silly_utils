/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-03
 * @file: silly_radar_db_data.cpp
 * @description: silly_radar_db_data 头文件
 * @version: v1.0.1 2025-12-03 dou li yang
 */
#ifndef SILLY_RADAR_DB_DATA_H
#define SILLY_RADAR_DB_DATA_H
#include <datetime/silly_posix_time.h>
#include <files/silly_file.h>
class suRadarDBData
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

#endif  // SILLY_RADAR_DB_DATA_H
