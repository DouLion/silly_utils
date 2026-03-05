/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_posix_time.cpp
 * @description: sutime实现
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#include "silly_posix_time.h"
#include <iomanip>
#include <sstream>
#ifndef SU_TIME
#define SU_TIME
#define SEC_IN_MIN 60
#define MIN_IN_HOUR 60
#define HOUR_IN_DAY 24
#define SEC_IN_HOUR (SEC_IN_MIN * MIN_IN_HOUR)
#define MIN_IN_DAY (MIN_IN_HOUR * HOUR_IN_DAY)
#define SEC_IN_DAY (SEC_IN_MIN * MIN_IN_HOUR * HOUR_IN_DAY)
#endif

static void check_std_tm(std::tm stm)
{
    if (stm.tm_sec < 0 || stm.tm_sec > 59)
        throw std::runtime_error(std::string("秒(sec) ").append(std::to_string(stm.tm_sec)).append(" 需要 0 - 59"));
    if (stm.tm_min < 0 || stm.tm_min > 59)
        throw std::runtime_error(std::string("分(min) ").append(std::to_string(stm.tm_min)).append(" 需要 0 - 59"));
    if (stm.tm_hour < 0 || stm.tm_hour > 23)
        throw std::runtime_error(std::string("时(hour) ").append(std::to_string(stm.tm_hour)).append(" 需要 0 - 24"));
    if (stm.tm_mday < 1 || stm.tm_mday > 31)
        throw std::runtime_error(std::string("日(day) ").append(std::to_string(stm.tm_mday)).append(" 需要 1 - 31"));
    if (stm.tm_mon < 0 || stm.tm_mon > 11)
        throw std::runtime_error(std::string("月(mon) ").append(std::to_string(stm.tm_mon)).append(" 需要 0 - 11"));
    if (stm.tm_year <= 0)
        throw std::runtime_error(std::string("年(year) ").append(std::to_string(stm.tm_year)).append(" + 1900"));
}

suduration::suduration(int hours, int minutes, int seconds)
{
    m_total_seconds = hours * 3600 + minutes * 60 + seconds;
}

suduration::suduration(sustamp secs)
{
    m_total_seconds = secs;
}

int suduration::total_days() const
{
    return m_total_seconds / SEC_IN_DAY;
}

int suduration::hours() const
{
    return m_total_seconds % SEC_IN_DAY / SEC_IN_HOUR;
}

int suduration::minutes() const
{
    return m_total_seconds % SEC_IN_HOUR / SEC_IN_MIN;
}

int suduration::seconds() const
{
    return m_total_seconds % SEC_IN_MIN;
}

sustamp suduration::total_seconds() const
{
    return m_total_seconds;
}

int suduration::total_minutes() const
{
    return m_total_seconds / SEC_IN_MIN;
}

int suduration::total_hours() const
{
    return m_total_seconds / SEC_IN_HOUR;
}

sutime::sutime()
{
    std::scoped_lock lock(m_mutex);
    std::time_t stt = 8 * SEC_IN_HOUR;
    m_tm = *std::gmtime(&stt);
    m_time_point = std::chrono::system_clock::from_time_t(0);
}

sutime::sutime(const sutime& time)
{
    m_tm = time.m_tm;
    m_time_point = time.m_time_point;
}

sutime::sutime(const std::string& time)
{
    from_string(time);
}

sutime::sutime(const std::tm& stm)
{
    std::scoped_lock lock(m_mutex);
    m_tm = stm;
    std::time_t stamp = std::mktime(&m_tm);
    m_time_point = std::chrono::system_clock::from_time_t(stamp);
}

sutime::sutime(const sustamp& stamp)
{
    std::scoped_lock lock(m_mutex);
    m_time_point = std::chrono::system_clock::from_time_t(stamp);
    fix_tm();
}
bool sutime::parse(const std::string& str, const std::string& fmt)
{
    return from_string(str, fmt);
}

bool sutime::from_string(const std::string& str, const std::string& fmt)
{
    std::scoped_lock lock(m_mutex);
    bool status = false;
    try
    {
        std::istringstream ss(str);
        std::tm tmp = {};
        ss >> std::get_time(&tmp, fmt.c_str());
        check_std_tm(tmp);
        // 检查是否成功解析
        if (!ss.fail())
        {
            std::time_t stamp = std::mktime(&tmp);
            m_time_point = std::chrono::system_clock::from_time_t(stamp);
            m_tm = tmp;
            status = true;
        }
        else
        {
            m_err = "转换错误.";
        }
    }
    catch (std::exception& e)
    {
        m_err = e.what();
    }
    return status;
}

std::string sutime::stringify(const std::string& fmt) const
{
    return to_string(fmt);
}

std::string sutime::to_string(const std::string& fmt) const
{
    std::string result;
    try
    {
        check_std_tm(m_tm);
        // 创建一个输出流用于格式化输出
        std::ostringstream oss;
        // 设置格式为 "YYYY-MM-DD HH:MM:SS"
        oss << std::put_time(&m_tm, fmt.c_str());

        // 返回格式化的字符串
        return oss.str();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return result;
}

sutime sutime::operator+(const suduration& td) const
{
    sutime result;
    result.m_time_point = m_time_point + std::chrono::seconds(td.total_seconds());
    result.fix_tm();
    return result;
}

sutime& sutime::operator+=(const suduration& td)
{
    std::scoped_lock lock(m_mutex);
    m_time_point += std::chrono::seconds(td.total_seconds());
    fix_tm();
    return *this;
}

sutime sutime::operator-(const suduration& td) const
{
    sutime result;
    result.m_time_point = m_time_point - std::chrono::seconds(td.total_seconds());
    result.fix_tm();
    return result;
}
sutime& sutime::operator-=(const suduration& td)
{
    std::scoped_lock lock(m_mutex);
    m_time_point -= std::chrono::seconds(td.total_seconds());
    fix_tm();
    return *this;
}

suduration sutime::operator-(const sutime& rh) const
{
    auto td = std::chrono::duration_cast<std::chrono::seconds>(m_time_point - rh.m_time_point);
    return suduration(td.count());
}

sustamp sutime::stamp_sec() const
{
    return std::chrono::system_clock::to_time_t(m_time_point);
}

std::chrono::system_clock::time_point sutime::time_point() const
{
    return m_time_point;
}

std::tm sutime::tm() const
{
    return m_tm;
}

int sutime::year() const
{
    return m_tm.tm_year + 1900;
}

int sutime::month() const
{
    return m_tm.tm_mon + 1;
}

int sutime::day() const
{
    return m_tm.tm_mday;
}

int sutime::hour() const
{
    return m_tm.tm_hour;
}

int sutime::minute() const
{
    return m_tm.tm_min;
}

int sutime::second() const
{
    return m_tm.tm_sec;
}

int sutime::yday() const
{
    return m_tm.tm_yday;
}

int sutime::wday() const
{
    return m_tm.tm_wday;
}

void sutime::fix_tm()
{
    std::time_t stt = std::chrono::system_clock::to_time_t(m_time_point);
    stt += SU_TIME_ZONE * SEC_IN_HOUR;
    // 将 time_t 转换为 tm 结构
    m_tm = *std::gmtime(&stt);
}

bool sutime::operator>(const sutime& rh) const
{
    return m_time_point > rh.m_time_point;
}

bool sutime::operator==(const sutime& rh) const
{
    return m_time_point == rh.m_time_point;
}

bool sutime::operator<(const sutime& rh) const
{
    return m_time_point < rh.m_time_point;
}

bool sutime::operator>=(const sutime& rh) const
{
    return m_time_point >= rh.m_time_point;
}

bool sutime::operator<=(const sutime& rh) const
{
    return m_time_point <= rh.m_time_point;
}

sutime sutime::operator=(const sutime& rh)
{
    std::scoped_lock lock(m_mutex);
    m_time_point = rh.m_time_point;
    m_tm = rh.m_tm;
    return *this;
}

sutime sutime::now()
{
    sutime result;
    result.m_time_point = std::chrono::system_clock::now();
    result.fix_tm();
    return result;
}

sutime sutime::time_from_string(const std::string& str, const std::string& fmt)
{
    sutime result;
    result.from_string(str, fmt);
    return result;
}

std::string sutime::time_to_string(const sutime& pt, const std::string& fmt)
{
    return pt.to_string(fmt);
}

bool sutime::is_not_a_date_time() const
{
    try
    {
        check_std_tm(m_tm);
    }
    catch (...)
    {
        return true;
    }

    return false;
}

extern std::string TimeStringify(const sutime& tm, const std::string& fmt)
{
    return tm.stringify(fmt);
}
