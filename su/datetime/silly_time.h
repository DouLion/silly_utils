/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/2/4 10:49
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_TIME_H
#define SILLY_UTILS_SILLY_TIME_H

#include <su_marco.h>
#include <ctime>
#include <time.h>

#define SILLY_DATE_FMT1 = "%Y-%m-%d %H:%M:%S";
#define SILLY_DATE_FMT2 = "%Y%m%d%H";
#define SILLY_DATE_FMT3 = "%H";
#define SILLY_DATE_FMT4 = "%Y%m%d%H%M";
#define SILLY_DATE_FMT5 = "%Y-%m-%d %H:%M";
#define SILLY_DATE_FMT6 = "%Y%m%d";
#define SILLY_DATE_FMT7 = "%Y-%m-%d-%H-%M-%S";
#define SILLY_DATE_FMT8 = "%Y-%m-%d %H:00";
#define SILLY_DATE_FMT10 = "%Y%m%f%h00";

class silly_time2s
{
  public:
    constexpr const static unsigned int COMMON_YEAR[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    constexpr const static unsigned int LEAP_YEAR[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  public:
    /// <summary>
    /// 一年中的的第几天
    /// </summary>
    /// <param name="year"></param>
    /// <param name="month"></param>
    /// <param name="day"></param>
    /// <returns></returns>
    static unsigned int days_of_year(unsigned int year, unsigned int month, unsigned int day);

    static unsigned int hours_of_year(unsigned int year, unsigned int month, unsigned int day, unsigned int hour);

    static unsigned int minutes_of_year(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute);

    static unsigned int seconds_of_year(unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);

    static bool compress_time_format(const std::string& time, unsigned int& ymd, unsigned int& hms);

    static bool compress_time_format(const std::string& time, unsigned int& y_seconds);

    static bool decompress_time_format(unsigned int& ymd, unsigned int& hms, std::string& time);

    static bool decompress_time_format(const unsigned int& y_seconds, std::string& time);

    /// <summary>
    /// 返回一个时间戳,表示从1970年1月1日0时0分0秒）到当前时间的秒数
    /// </summary>
    /// <returns></returns>
    static time_t now_stamp();

    static std::tm now_tm(const bool& local = true);

    std::string now_str(const std::string& format = "%Y-%m-%d %H:%M:%S", const bool& local = true);

    static std::string tm2str(const std::tm& t, const std::string& format = "%Y-%m-%d %H:%M:%S");

    static std::string tm2str(const time_t& t, const bool& local = true, const std::string& format = "%Y-%m-%d %H:%M:%S");

    static std::tm str2tm(const std::string& str, const std::string& format = "%Y-%m-%d %H:%M:%S", const bool& local = true);
    static time_t str2stamp(const std::string& str, const std::string& format = "%Y-%m-%d %H:%M:%S", const bool& local = true);
};

/// <summary>
/// 以时间戳为基础的时间,精确到秒 normal time
/// </summary>
namespace silly_ntime
{
    class duration
    {
        duration() = default;
        duration(int h, int m, int s);

        int total_seconds();
        int hours{ 0 };
        int minutes{ 0 };
        int seconds{ 0 };
    };

    class time
    {
        void now();
        bool fstr(const std::string& str);
        std::string pstr();

        bool operator==(const time& t) const;
        bool operator>(const time& t) const;
        bool operator<(const time& t) const;
        bool operator>=(const time& t) const;
        bool operator<=(const time& t) const;

        time& operator+(const int& s);
        time& operator-(const int& s);

        time& operator+(const duration& d);
        time& operator-(const duration& d);

        time& operator+=(const int& s);
        time& operator-=(const int& s);

        time& operator+=(const duration& d);
        time& operator-=(const duration& d);


    private:
        time_t timestamp;
    };

 
}

/// <summary>
/// 单位更加精细的时间 super time
/// </summary>
namespace silly_stime
{

class time
{

};

class duration
{

};

}  // namespace silly_time

#endif  // SILLY_UTILS_SILLY_TIME_H
