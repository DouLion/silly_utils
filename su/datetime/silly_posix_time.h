/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_posix_time.h
 * @description: silly_posix_time 除了时间戳,其他年月日时分秒都按照北京时间结算
 *                 中国人不放洋屁
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_POSIX_TIME_H
#define SILLY_UTILS_SILLY_POSIX_TIME_H
#include <su_marco.h>

const static char* DATE_FORMAT_1 = "%Y-%m-%d %H:%M:%S";
const static char* DATE_FORMAT_2 = "%Y%m%d%H";
const static char* DATE_FORMAT_3 = "%H";
const static char* DATE_FORMAT_4 = "%Y%m%d%H%M";
const static char* DATE_FORMAT_5 = "%Y-%m-%d %H:%M";
const static char* DATE_FORMAT_6 = "%Y%m%d";
const static char* DATE_FORMAT_7 = "%Y-%m-%d-%H-%M-%S";
const static char* DATE_FORMAT_8 = "%Y-%m-%d %H:00";

using silly_time_stamp = std::time_t;  // 始终以秒为准
// using silly_time_duration = std::chrono::system_clock::duration;
class silly_time_duration
{
  public:
    silly_time_duration() = default;
    silly_time_duration(int hours, int minutes, int seconds);
    silly_time_duration(silly_time_stamp secs);

    int total_days() const;
    int hours() const;
    int total_hours() const;
    int minutes() const;
    int total_minutes() const;
    int seconds() const;
    silly_time_stamp total_seconds() const;

  private:
    silly_time_stamp m_total_seconds{0};
};

static void check_std_tm(std::tm stm);

// 时间戳为标准时间戳精确到秒;
// 事件以北京时间为准,间fix_tm的实现;
class silly_posix_time
{
  public:
    static silly_posix_time now();
    static silly_posix_time time_from_string(const std::string& str, const std::string& fmt = DATE_FORMAT_1);
    static std::string time_to_string(const silly_posix_time& pt, const std::string& fmt = DATE_FORMAT_1);
    silly_posix_time();
    silly_posix_time(const silly_posix_time& time);
    // silly_posix_time(const silly_posix_time& other) = default;
    ~silly_posix_time() = default;

    bool from_string(const std::string& str, const std::string& fmt = DATE_FORMAT_1);
    std::string to_string(const std::string& fmt = DATE_FORMAT_1) const;

    silly_posix_time(const silly_time_stamp& stamp);

    bool operator>(const silly_posix_time& other) const;
    bool operator==(const silly_posix_time& other) const;
    bool operator<(const silly_posix_time& other) const;
    bool operator>=(const silly_posix_time& other) const;
    bool operator<=(const silly_posix_time& other) const;

    silly_posix_time operator=(const silly_posix_time& other);
    silly_posix_time operator+(const silly_time_duration& other) const;
    silly_posix_time& operator+=(const silly_time_duration& other);
    silly_posix_time operator-(const silly_time_duration& other) const;
    silly_posix_time& operator-=(const silly_time_duration& other);
    silly_time_duration operator-(const silly_posix_time& other) const;

    silly_time_stamp stamp_sec() const;

    std::chrono::system_clock::time_point time_point() const;

    std::tm tm() const;

    int year() const;    // 年
    int month() const;   // 月
    int day() const;     // 日
    int yday() const;    // 一年中的第几天
    int wday() const;    // 一周中的第几天
    int hour() const;    // 小时
    int minute() const;  // 分钟
    int second() const;  // 秒

  private:
    void fix_tm();

  private:
    std::string m_err;
    std::chrono::system_clock::time_point m_time_point;  // 这个是不区分时区的时间戳
    std::tm m_tm{};
    std::mutex m_mutex;
};

static silly_posix_time time_from_string(const std::string& str, const std::string& fmt = DATE_FORMAT_1);
static std::string time_to_string(const silly_posix_time& pt, const std::string& fmt = DATE_FORMAT_1);
#endif  // SILLY_UTILS_SILLY_POSIX_TIME_H
