//
// Created by dell on 2024/1/25.
//

#include "silly_geo_prop.h"

suGeoProp::suGeoProp(const std::string &s)
{
    m_data = s;
    m_type = eGeoFieldType::String;
}

suGeoProp::suGeoProp(const int &i)
{
    m_data = i;
    m_type = eGeoFieldType::Int;
}

suGeoProp::suGeoProp(const double &d)
{
    m_data = d;
    m_type = eGeoFieldType::Numeric;
}

suGeoProp::suGeoProp(const long long &ll)
{
    m_data = ll;
    m_type = eGeoFieldType::Long;
}
suGeoProp::suGeoProp(const std::vector<unsigned char> &bs)
{
}

suGeoProp::suGeoProp(const suGeoProp &other)
{
    this->m_type = other.m_type;
    this->m_data = other.m_data;
}

suGeoProp::~suGeoProp()
{
    // SU_MEM_FREE(m_data)
}

std::string suGeoProp::as_string() const
{
    if (m_data.has_value() && m_type == eGeoFieldType::String)
    {
        return std::any_cast<std::string>(m_data);
    }
    return "";
}

int suGeoProp::as_int32() const
{
    if (m_data.has_value() && m_type == eGeoFieldType::Int)
    {
        return std::any_cast<int>(m_data);
    }
    return 0;
}

double suGeoProp::as_double() const
{
    if (m_data.has_value())
    {
        if (m_type == eGeoFieldType::Numeric)
        {
            return std::any_cast<double>(m_data);
        }
        if (m_type == eGeoFieldType::Long)
        {
            return std::any_cast<long long>(m_data);
        }
        if (m_type == eGeoFieldType::Int)
        {
            return std::any_cast<int>(m_data);
        }
        if (m_type == eGeoFieldType::String)
        {
            try
            {
                return std::stod(as_string());
            }
            catch (...)
            {
            }
        }
    }
    return 0.0;
}

std::vector<unsigned char> suGeoProp::as_binary() const
{
    if (m_data.has_value() && m_type == eGeoFieldType::Binary)
    {
        return std::any_cast<std::vector<unsigned char>>(m_data);
    }
    return {};
}

long long suGeoProp::as_int64() const
{
    if (m_data.has_value() && m_type == eGeoFieldType::Long)
    {
        return std::any_cast<long long>(m_data);
    }
    return 0;
}

eGeoFieldType suGeoProp::type() const
{
    return m_type;
}
