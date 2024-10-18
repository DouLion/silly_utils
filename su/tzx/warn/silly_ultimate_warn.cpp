/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-18
 * @file: silly_ultimate_warn.cpp
 * @description: silly_ultimate_warn实现
 * @version: v1.0.1 2024-10-18 dou li yang
 */
#include "silly_ultimate_warn.h"
std::string silly_ultimate_warn::name() const
{
    return m_name;
}
void silly_ultimate_warn::name(const std::string& name)
{
    m_name = name;
}
std::string silly_ultimate_warn::desc() const
{
    return m_desc;
}
void silly_ultimate_warn::desc(const std::string& desc)
{
    m_desc = desc;
}
std::string silly_ultimate_warn::adnm() const
{
    return m_adnm;
}
void silly_ultimate_warn::adnm(const std::string& adnm)
{
    m_adnm = adnm;
}
float silly_ultimate_warn::static_rule() const
{
    return m_static_rule;
}
void silly_ultimate_warn::static_rule(const float& static_rule)
{
    m_static_rule = static_rule;
}
float silly_ultimate_warn::dynamic_rule() const
{
    return m_dynamic_rule;
}
void silly_ultimate_warn::dynamic_rule(const float& dynamic_rule)
{
    m_dynamic_rule = dynamic_rule;
}
float silly_ultimate_warn::moisture_p() const
{
    return m_moisture_p;
}
void silly_ultimate_warn::moisture_p(const float& moisture_p)
{
    m_moisture_p = moisture_p;
}
