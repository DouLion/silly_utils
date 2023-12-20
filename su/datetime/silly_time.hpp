/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/20 18:28
 * @version: 1.0.1
 * @description: 时间处理工具
 */
#ifndef SILLY_UTILS_SILLY_TIME_HPP
#define SILLY_UTILS_SILLY_TIME_HPP

#include <datetime/simple_time.h>

using namespace simple_time;

#include <su_marco.h>



class silly_time {
    typedef unsigned int istint;
    constexpr const static istint COMMON_YEAR[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    constexpr const static istint LEAP_YEAR[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

public:
    /// <summary>
    /// 一年中的的第几天
    /// </summary>
    /// <param name="year"></param>
    /// <param name="month"></param>
    /// <param name="day"></param>
    /// <returns></returns>
    static istint days_of_year(istint year, istint month, istint day) {

        istint days = 0;
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            for (istint i = 0; i < month - 1; i++) {
                days += LEAP_YEAR[i];
            }
        } else {
            for (istint i = 0; i < month - 1; i++) {
                days += COMMON_YEAR[i];
            }
        }
        days += day;
        return days;
    }

    static istint hours_of_year(istint year, istint month, istint day, istint hour) {
        istint days = days_of_year(year, month, day);
        return (days - 1) * 24 + hour;
    }

    static istint minutes_of_year(istint year, istint month, istint day, istint hour, istint minute) {
        istint hours = hours_of_year(year, month, day, hour);
        return hours * 60 + minute;
    }

    static istint seconds_of_year(istint year, istint month, istint day, istint hour, istint minute, istint second) {
        istint minutes = minutes_of_year(year, month, day, hour, minute);
        return minutes * 60 + second;
    }

    static bool compress_time_format(const std::string &time, unsigned int &ymd, unsigned int &hms) {
        int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
        if (EOF == sscanf(time.c_str(), SU_STD_TIME_FORMAT1, &year, &month, &day, &hour, &minute, &second)) {
            SU_ERROR_PRINT("INVALID TIME FORMAT: %s", time.c_str());
            return false;
        }
        ymd = ((((0x00 | year) << 8) | month) << 8) | day;
        hms = ((((0x00 | hour) << 8) | minute) << 8) | second;
        return true;
    }

    static bool compress_time_format(const std::string &time, unsigned int& y_seconds) {
        int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
        if (EOF == sscanf(time.c_str(), SU_STD_TIME_FORMAT1, &year, &month, &day, &hour, &minute, &second)) {
            SU_ERROR_PRINT("INVALID TIME FORMAT: %s", time.c_str());
            return false;
        }
        y_seconds= 0x00000000;
        char c_after_2000 = year / 2000;// 是否在2000年之后
        char c_sub_year = year % 100;//年份的后两位
        if (c_after_2000) y_seconds = 0x01;
        y_seconds = (y_seconds << 7) | c_sub_year;
        istint minutes = minutes_of_year(year, month, day, hour, minute);

        y_seconds = (y_seconds<<24)|minutes;
        return true;
    }

    static bool decompress_time_format(unsigned int &ymd, unsigned int &hms, std::string &time) {
        int year = (ymd) >> 16;
        int month = (ymd & 0xFF00) >> 8;
        int day = (ymd & 0xFF);
        int hour = (hms) >> 16;
        int minute = (hms & 0xFF00) >> 8;
        int second = (hms & 0xFF);
        char buff[128] = {0};
        sprintf(buff, SU_STD_TIME_FORMAT1, year, month, day, hour, minute, second);
        time = buff;
        return true;
    }

    static bool decompress_time_format(const unsigned int& y_seconds, std::string& time)
    {
        int year = (y_seconds <<1)>>25;
        if(y_seconds>>31)
        {
            year += 2000;
        }
        else
        {
            year += 1900;
        }
        int minutes = 0x00FFFFFF & y_seconds;
        int days = minutes/MIN_IN_DAY;
        int hour = (minutes-days*MIN_IN_DAY)/MIN_IN_HOUR;
        int minute = minutes%MIN_IN_HOUR;
        int month = 0;

        int is_leap_year = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        while(days > 0)
        {
            if(is_leap_year)
            {
                days -= LEAP_YEAR[month];
            }
            else
            {
                days -= COMMON_YEAR[month];
            }
            month++;

        }
        int day =  (is_leap_year ? days +LEAP_YEAR[month-1] : days + COMMON_YEAR[month-1]) +1;
        int second = 0;
        char buff[128] = {0};
        sprintf(buff, SU_STD_TIME_FORMAT1, year, month, day, hour, minute, second);
        time = buff;
        return true;

    }
};

#endif //SILLY_UTILS_SILLY_TIME_HPP
