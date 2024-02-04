//
// Created by dell on 2024/2/4.
//

#include "silly_time.h"

unsigned int silly_time2s::days_of_year(unsigned int year, unsigned int month, unsigned int day)    {
    unsigned int days = 0;
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
    {
        for (unsigned int i = 0; i < month - 1; i++)
        {
            days += LEAP_YEAR[i];
        }
    }
    else
    {
        for (unsigned int i = 0; i < month - 1; i++)
        {
            days += COMMON_YEAR[i];
        }
    }
    days += day;
    return days;
}

unsigned int silly_time2s::hours_of_year(unsigned int year, unsigned int month, unsigned int day, unsigned int hour){
    unsigned int days = days_of_year(year, month, day);
    return (days - 1) * 24 + hour;
}

unsigned int silly_time2s::minutes_of_year(unsigned int year, unsigned int month, unsigned int day, unsigned int hour,
                                         unsigned int minute)
{
    unsigned int hours = hours_of_year(year, month, day, hour);
    return hours * 60 + minute;
}

unsigned int silly_time2s::seconds_of_year(unsigned int year, unsigned int month, unsigned int day, unsigned int hour,
                                         unsigned int minute, unsigned int second)     {
    unsigned int minutes = minutes_of_year(year, month, day, hour, minute);
    return minutes * 60 + second;
}

bool silly_time2s::compress_time_format(const std::string &time, unsigned int &ymd, unsigned int &hms) {
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    if (EOF == sscanf(time.c_str(), SU_STD_TIME_FORMAT1, &year, &month, &day, &hour, &minute, &second))
    {
        SU_ERROR_PRINT("INVALID TIME FORMAT: %s", time.c_str());
        return false;
    }
    ymd = ((((0x00 | year) << 8) | month) << 8) | day;
    hms = ((((0x00 | hour) << 8) | minute) << 8) | second;
    return true;
}

bool silly_time2s::compress_time_format(const std::string &time, unsigned int &y_seconds) {
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    if (EOF == sscanf(time.c_str(), SU_STD_TIME_FORMAT1, &year, &month, &day, &hour, &minute, &second))
    {
        SU_ERROR_PRINT("INVALID TIME FORMAT: %s", time.c_str());
        return false;
    }
    y_seconds = 0x00000000;
    char c_after_2000 = year / 2000;  // 是否在2000年之后
    char c_sub_year = year % 100;     // 年份的后两位
    if (c_after_2000)
        y_seconds = 0x01;
    y_seconds = (y_seconds << 7) | c_sub_year;
    unsigned int minutes = minutes_of_year(year, month, day, hour, minute);

    y_seconds = (y_seconds << 24) | minutes;
    return true;
}

bool silly_time2s::decompress_time_format(unsigned int &ymd, unsigned int &hms, std::string &time)   {
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
bool silly_time2s::decompress_time_format(const unsigned int &y_seconds, std::string &time)
{
    int year = (y_seconds << 1) >> 25;
    if (y_seconds >> 31)
    {
        year += 2000;
    }
    else
    {
        year += 1900;
    }
    int minutes = 0x00FFFFFF & y_seconds;
    int days = minutes / MIN_IN_DAY;
    int hour = (minutes - days * MIN_IN_DAY) / MIN_IN_HOUR;
    int minute = minutes % MIN_IN_HOUR;
    int month = 0;

    int is_leap_year = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    while (days > 0)
    {
        if (is_leap_year)
        {
            days -= LEAP_YEAR[month];
        }
        else
        {
            days -= COMMON_YEAR[month];
        }
        month++;
    }
    int day = (is_leap_year ? days + LEAP_YEAR[month - 1] : days + COMMON_YEAR[month - 1]) + 1;
    int second = 0;
    char buff[128] = {0};
    sprintf(buff, SU_STD_TIME_FORMAT1, year, month, day, hour, minute, second);
    time = buff;
    return true;
}
time_t silly_time2s::now_stamp() {
    time_t t;
    time(&t);
    return t;
}

std::tm silly_time2s::now_tm(const bool &local) {
    time_t timep;
    std::tm* p;

    time(&timep);
    if (local)
    {
        p = localtime(&timep);
    }
    else
    {
        p = gmtime(&timep);
    }
    return *p;
}

std::string silly_time2s::tm2str(const std::tm &t, const std::string &format)  {
    char buff[256] = {0};
    strftime(buff, 256, format.c_str(), &t);
    return buff;
}

std::string silly_time2s::tm2str(const time_t &t, const bool &local, const std::string &format) {
    std::tm* p;
    if (local)
    {
        p = localtime(&t);
    }
    else
    {
        p = gmtime(&t);
    }
    return tm2str(*p);
}

std::string silly_time2s::now_str(const std::string& format, const bool &local) {
    time_t t;
    time(&t);
    return tm2str(t, local,format);
}

std::tm silly_time2s::str2tm(const std::string &str, const std::string &format, const bool &local) {
    std::tm stm;
    
    return std::tm();
}

time_t silly_time2s::str2stamp(const std::string &str, const std::string &format, const bool &local) {
    return 0;
}
