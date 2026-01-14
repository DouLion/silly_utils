/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: ST_RSVR_R.cpp
 * @description: 水库水情记录 Reservoir Record
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef ST_RSVR_R_H
#define ST_RSVR_R_H
#include <database/otl/silly_otl.h>
namespace RWDB
{
struct ST_RSVR_R
{
    char STCD[8] = {};  // char[8] 测站编码
    std::time_t TM = 0;
    double RZ = 0.0;     // N(7,3) 库上水位 米(m)
    double BLRZ = 0.0;   // N(7,3) 库下水位 米(m) 非标准
    double INQ = 0.0;    // N(9,3) 入库流量 立方米每秒(m3/s)
    double OTQ = 0.0;    // N(9,3) 出库流量 立方米每秒(m3/s)
    double W = 0.0;      // N(9,3) 蓄水量 立方米(m3)
    char RWCHRCD = 0.0;  // C(1) 库水特征码
    char RWPTN = 0.0;    // C(1) 库水水势
    double INQDR = 0.0;  // N(5,2) 入流时段长
    char MSQMT[2] = {};  // C(2) 测流方法
};
}  // namespace RWDB

#endif  // ST_RSVR_R_H
