/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: DateTime.cpp
 * @description: DateTime实现
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#include "DateTime.h"

X::DateTime::DateTime(const otl_datetime& dt)
{
    FromOTL(dt);
}

std::string X::DateTime::AsBin()

{
    std::string ret;
    ret.resize(LEN);
    char* p = ret.data();
    *(uint16_t*)p  = year;
    p += sizeof(uint16_t);
    *(uint8_t*)p  = month;
    p += sizeof(uint8_t);
    *(uint8_t*)p  = day;
    p += sizeof(uint8_t);
    *(uint8_t*)p  = hour;
    p += sizeof(uint8_t);
    *(uint8_t*)p  = minute;
    p += sizeof(uint8_t);
    *(uint8_t*)p  = second;
    p += sizeof(uint8_t);
    *(uint16_t*)p  = millisecond;
    return ret;
}
void X::DateTime::FromOTL(const otl_datetime& dt)
{
    year = dt.year;
    month = dt.month;
    day = dt.day;
    hour = dt.hour;
    minute = dt.minute;
    second = dt.second;
    millisecond = dt.fraction/1e6;
    // frac_precision = dt.frac_precision;
}
std::string X::DateTime::StrFTime()
{
    char buff[64] = {0};

    sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d.%03d", year, month, day, hour, minute, second, millisecond);
    return std::string(buff);

}
