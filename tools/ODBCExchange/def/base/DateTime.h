/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: DateTime.h
 * @description: DateTime 类声明
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#ifndef SILLY_UTILS_DATETIME_H
#define SILLY_UTILS_DATETIME_H
#include <database/otl/silly_otl.h>
namespace X
{
class DateTime
{
  public:
    DateTime() = default;
    ~DateTime() = default;
    DateTime(const otl_datetime& dt);

    std::string AsBin();

    std::string StrFTime();

    void FromOTL(const otl_datetime& dt);

  public:
    uint16_t year = 0;
    uint8_t month = 0;      // 从 1开始
    uint8_t day = 0;        // 从 1开始
    uint8_t hour = 0;       // 从 0开始
    uint8_t minute = 0;     // 从 0开始
    uint8_t second = 0;     // 从 0开始
    uint16_t millisecond = 0;  //  毫秒 fraction / std::pow(10.0, frac_precision);
    // 年 2 月 1 日 1 时 1 分 1 秒 1 毫秒 2 小数位数 1
    static const uint8_t LEN = sizeof(year) + sizeof(month) + sizeof(day) + sizeof(hour) + sizeof(minute) + sizeof(second) + sizeof(millisecond);
};
}


#endif  // SILLY_UTILS_DATETIME_H
