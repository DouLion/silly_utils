/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-18
 * @file: silly_simpe_warn.cpp
 * @description: silly_simpe_warn实现
 * @version: v1.0.1 2024-10-18 dou li yang
 */
#include "silly_simple_warn.h"
std::string silly_simple_warn::code() const
{
    return m_code;
}
silly_posix_time silly_simple_warn::btm() const
{
    return m_btm;
}
int silly_simple_warn::grade() const
{
    return m_grade;
}
int silly_simple_warn::type() const
{
    return m_type;
}
int silly_simple_warn::status() const
{
    return m_status;
}
float silly_simple_warn::intv() const
{
    return m_intv;
}
float silly_simple_warn::rule() const
{
    return m_rule;
}
float silly_simple_warn::value() const
{
    return m_value;
}
void silly_simple_warn::code(const std::string& code)
{
    m_code = code;
}
void silly_simple_warn::btm(const silly_posix_time& btm)
{
    m_btm = btm;
}
void silly_simple_warn::grade(const int& grade)
{
    m_grade = grade;
}
void silly_simple_warn::type(const int& type)
{
    m_type = type;
}
void silly_simple_warn::status(const int& status)
{
    m_status = status;
}
void silly_simple_warn::intv(const float& intv)
{
    m_intv = intv;
}
void silly_simple_warn::rule(const float& rule)
{
    m_rule = rule;
}
void silly_simple_warn::value(const float& value)
{
    m_value = value;
}
