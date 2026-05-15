/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_posix_time.h
 * @description: 包含了常用的时间处理函数
 * @version: v1.0.1 2024-08-30 除了时间戳,其他年月日时分秒都按照北京时间结算
 * @version: v1.0.2 2024-09-01 支持了自定义时区
 */
#ifndef SILLY_POSIX_TIME_H
#define SILLY_POSIX_TIME_H
#include <iostream>
#include <time.h>
#include <chrono>
#include <mutex>
#include <string>

#define DATE_FORMAT_1 "%Y-%m-%d %H:%M:%S"
#define DATE_FORMAT_2 "%Y%m%d%H"
#define DATE_FORMAT_3 "%H"
#define DATE_FORMAT_4 "%Y%m%d%H%M"
#define DATE_FORMAT_5 "%Y-%m-%d %H:%M"
#define DATE_FORMAT_6 "%Y%m%d"
#define DATE_FORMAT_7 "%Y-%m-%d-%H-%M-%S"
#define DATE_FORMAT_8 "%Y-%m-%d %H:00"
#define DTFMT_Y "%Y"
#define DTFMT_YM "%Y-%m"
#define DTFMT_YMD "%Y-%m-%d"
#define DTFMT_YMDH "%Y-%m-%d %H"
#define DTFMT_YMDHM "%Y-%m-%d %H:%M"
#define DTFMT_YMDHMS "%Y-%m-%d %H:%M:%S"
#define DTFMT_YM_P "%Y%m"
#define DTFMT_YMD_P "%Y%m%d"
#define DTFMT_YMDH_P "%Y%m%d%H"
#define DTFMT_YMDHM_P "%Y%m%d%H%M"
#define DTFMT_YMDHMS_P "%Y%m%d%H%M%S"

#ifndef SU_TIME_ZONE
#define SU_TIME_ZONE 8
#endif

using sustamp = std::time_t; // 始终以秒为准
class suduration
{
public:
    suduration() = default;

    suduration(int hours, int minutes, int seconds);

    suduration(sustamp secs);

    int total_days() const;

    int hours() const;

    int total_hours() const;

    int minutes() const;

    int total_minutes() const;

    int seconds() const;

    sustamp total_seconds() const;

private:
    sustamp m_total_seconds{0};
};

// 时间戳为标准时间戳精确到秒;
// 时间以北京时间为准,见fix_tm的实现;
class sutime
{
public:
    /// <summary>
    /// 构造函数
    /// </summary>
    sutime();

    sutime(const sutime& time);

    sutime(const std::string& time);

    sutime(const sustamp& stamp);

    sutime(const std::tm& stm);

    // 析构函数
    ~sutime() = default;

    static sutime now();

    static sutime time_from_string(const std::string& str, const std::string& fmt = "%Y-%m-%d %H:%M:%S");

    static std::string time_to_string(const sutime& pt, const std::string& fmt = "%Y-%m-%d %H:%M:%S");

    bool parse(const std::string& str, const std::string& fmt = "%Y-%m-%d %H:%M:%S");

    bool from_string(const std::string& str, const std::string& fmt = "%Y-%m-%d %H:%M:%S");

    std::string to_string(const std::string& fmt = "%Y-%m-%d %H:%M:%S") const;

    std::string stringify(const std::string& fmt = "%Y-%m-%d %H:%M:%S") const;

    bool is_not_a_date_time() const;

    /// <summary>
    ///
    /// </summary>
    /// <param name="rh"></param>
    /// <returns></returns>
    bool operator>(const sutime& rh) const;

    bool operator==(const sutime& rh) const;

    bool operator<(const sutime& rh) const;

    bool operator>=(const sutime& rh) const;

    bool operator<=(const sutime& rh) const;

    sutime operator=(const sutime& rh);

    sutime operator+(const suduration& rh) const;

    sutime& operator+=(const suduration& rh);

    sutime operator-(const suduration& rh) const;

    sutime& operator-=(const suduration& rh);

    suduration operator-(const sutime& rh) const;

    sustamp stamp_sec() const;

    sustamp stamp() const;

    std::chrono::system_clock::time_point time_point() const;

    std::tm tm() const;

    int year() const;   // 年
    int month() const;  // 月
    int day() const;    // 日
    int yday() const;   // 一年中的第几天
    int wday() const;   // 一周中的第几天
    int hour() const;   // 小时
    int minute() const; // 分钟
    int second() const; // 秒

private:
    void fix_tm();

protected:
    std::string m_err;
    std::chrono::system_clock::time_point m_time_point; // 这个是不区分时区的时间戳
    std::tm m_tm{0, 0, 0, 0, 0, 0};
    std::mutex m_mutex;
};

static std::string TimeStringify(const sutime& t, const std::string& fmt = "%Y-%m-%d %H:%M")
{
    return t.stringify(fmt);
}

using silly_time_duration = suduration;
using silly_posix_time = sutime;
#endif  // SILLY_POSIX_TIME_H