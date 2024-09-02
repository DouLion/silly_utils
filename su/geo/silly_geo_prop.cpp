//
// Created by dell on 2024/1/25.
//

#include "silly_geo_prop.h"

silly_geo_prop::silly_geo_prop(const std::string &s)
{
    m_data = s;
    m_type = enum_prop_type::eptString;
}

silly_geo_prop::silly_geo_prop(const int &i)
{
    m_data = i;
    m_type = enum_prop_type::eptInt;
}

silly_geo_prop::silly_geo_prop(const double &d)
{
    m_data = d;
    m_type = enum_prop_type::eptNumeric;
}

silly_geo_prop::silly_geo_prop(const long long &ll)
{
    m_data = ll;
    m_type = enum_prop_type::eptLong;
}

silly_geo_prop::silly_geo_prop(const silly_geo_prop &other)
{
    this->m_type = other.m_type;
    this->m_data = other.m_data;
}

silly_geo_prop::~silly_geo_prop()
{
    // SU_MEM_FREE(m_data)
}

void silly_geo_prop::release()
{
}

std::string silly_geo_prop::as_string() const
{
    if (m_data.has_value() && m_type == enum_prop_type::eptString)
    {
        return std::any_cast<std::string>(m_data);
    }
    return "";
}

int silly_geo_prop::as_int() const
{
    if (m_data.has_value() && m_type == enum_prop_type::eptInt)
    {
        return std::any_cast<int>(m_data);
    }
    return 0;
}

double silly_geo_prop::as_double() const
{
    if (m_data.has_value() && m_type == enum_prop_type::eptNumeric)
    {
        return std::any_cast<double>(m_data);
    }
    return 0.0;
}

std::vector<unsigned char> silly_geo_prop::as_binary() const
{
    if (m_data.has_value() && m_type == enum_prop_type::eptBinary)
    {
        return std::any_cast<std::vector<unsigned char>>(m_data);
    }
    return {};
}

long long silly_geo_prop::as_longlong() const
{
    if (m_data.has_value() && m_type == enum_prop_type::eptLong)
    {
        return std::any_cast<long long>(m_data);
    }
    return 0;
}

silly_geo_prop::enum_prop_type silly_geo_prop::value_type() const
{
    return m_type;
}
