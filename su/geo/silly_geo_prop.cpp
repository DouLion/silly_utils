//
// Created by dell on 2024/1/25.
//

#include "silly_geo_prop.h"

silly_geo_prop::silly_geo_prop(const unsigned char *d, const size_t &l, const silly_geo_prop::enum_prop_type &t)
{
    type = t;
    len = l;
    if ((data = (unsigned char *)malloc(len)))
    {
        memcpy(data, d, len);
    }
}

silly_geo_prop::silly_geo_prop(const std::string &s)
{
    type = enum_prop_type::eptString;
    len = s.size();
    if ((data = (unsigned char *)malloc(len)))
    {
        memcpy(data, &s[0], len);
    }
}

silly_geo_prop::silly_geo_prop(const int &i)
{
    type = enum_prop_type::eptInt;
    len = sizeof(i);
    if ((data = (unsigned char *)malloc(len)))
    {
        memcpy(data, &i, len);
    }
}

silly_geo_prop::silly_geo_prop(const double &d)
{
    type = enum_prop_type::eptNumeric;
    len = sizeof(d);
    if ((data = (unsigned char *)malloc(len)))
    {
        memcpy(data, &d, len);
    }
}

silly_geo_prop::silly_geo_prop(const char *c, const size_t &l)
{
    type = enum_prop_type::eptBinary;
    len = l;
    if ((data = (unsigned char *)malloc(len)))
    {
        memcpy(data, c, len);
    }
}

silly_geo_prop::silly_geo_prop(const long long &ll)
{
    type = enum_prop_type::eptLong;
    len = sizeof(ll);
    if ((data = (unsigned char *)malloc(len)))
    {
        memcpy(data, &ll, len);
    }
}

silly_geo_prop::silly_geo_prop(const silly_geo_prop &other)
{
    this->type = other.type;
    this->len = other.len;
    if ((this->data = (unsigned char *)malloc(other.len)))
    {
        memcpy(this->data, other.data, len);
    }
}

silly_geo_prop::~silly_geo_prop()
{
    SU_MEM_FREE(data)
}

void silly_geo_prop::destroy()
{
    type = enum_prop_type::eptNone;
    len = 0;
    SU_MEM_FREE(data)
}

silly_geo_prop &silly_geo_prop::operator=(const silly_geo_prop &other)
{
    if (this != &other)  // 避免自我赋值
    {
        this->type = other.type;
        this->len = other.len;
        SU_MEM_FREE(this->data)
        if ((this->data = (unsigned char *)malloc(other.len)))
        {
            memcpy(this->data, other.data, len);
        }
    }
    return *this;
}

std::string silly_geo_prop::as_string()
{
    std::string result;
    if (data)
    {
        result.resize(len+1);
        memcpy(&result[0], data, len);
    }
    return result;
}

int silly_geo_prop::as_int()
{
    if (data)
    {
        return ((int *)data)[0];
    }
    return 0;
}

double silly_geo_prop::as_double()
{
    if (data)
    {
        return ((double *)data)[0];
    }
    return 0.0;
}

unsigned char *silly_geo_prop::as_binary(size_t &l)
{
    return nullptr;
}

long long silly_geo_prop::as_longlong()
{
    if (data)
    {
        return ((long long *)data)[0];
    }
    return 0;
}

silly_geo_prop::enum_prop_type silly_geo_prop::value_type()
{
    return type;
}
