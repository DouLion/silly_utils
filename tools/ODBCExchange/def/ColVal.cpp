/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: ColVal.cpp
 * @description: ColVal实现
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#include "ColVal.h"

using namespace X;

ColVal::ColVal()
{
}
ColVal::ColVal(double_t v)
{
    Double(v);
}
ColVal::ColVal(float_t v)
{
    Float(v);
}
ColVal::ColVal(int32_t v)
{
    Int32(v);
}
ColVal::ColVal(uint32_t v)
{
    UInt32(v);
}
ColVal::ColVal(int16_t v)
{
    Int16(v);
}
ColVal::ColVal(int64_t v)
{
    Int64(v);
}
ColVal::ColVal(otl_datetime v)
{
    Time(v);
}
/*ColVal::ColVal(otl_value<otl_lob_stream> v)
{
    // Blob(v);
}*/
ColVal::ColVal(otl_long_string v)
{
    LStr(v);
}
ColVal::ColVal(std::string v)
{
    Str(v);
}

// -- 直接赋值, 基本实现
void ColVal::Double(double_t v)
{
    type = otl_var_double;
    d = v;
}
void ColVal::Float(float_t v)
{
    type = otl_var_float;
    d = v;
}
void ColVal::Int32(int32_t v)
{
    type = otl_var_int;
    i32 = v;
}
void ColVal::UInt32(uint32_t v)
{
    type = otl_var_unsigned_int;
    u32 = v;
}
void ColVal::Int16(int16_t v)
{
    type = otl_var_short;
    i16 = v;
}
void ColVal::Int64(int64_t v)
{
    type = otl_var_long_int;
    i64 = v;
}
void ColVal::Time(otl_datetime v)
{
    type = otl_var_timestamp;
    dt.FromOTL(v);
}
/*void ColVal::Blob(otl_value<otl_lob_stream> v)
{
    type = otl_var_blob;
    // str = olt_lob_stream_to_str(*v);
}*/
void ColVal::LStr(otl_long_string v)
{
    type = otl_var_varchar_long;
    str = otl_long_str_to_str( v);
}
void ColVal::Str(std::string v)
{
    type = otl_var_char;
    str = v;
}

void ColVal::Double(otl_value<double_t> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    Double(v.v);
}
void ColVal::Float(otl_value<float_t> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    Float(v.v);
}
void ColVal::Int16(otl_value<int16_t> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    Int16(v.v);
}
void ColVal::Int32(otl_value<int32_t> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    Int32(v.v);
}
void ColVal::Int64(otl_value<int64_t> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    Int64(v.v);
}
void ColVal::Str(otl_value<std::string> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    Str(v.v);
}
void ColVal::Blob(otl_value<std::string> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    type = otl_var_blob;
    str = v.v;
}
void ColVal::LStr(otl_value<otl_long_string> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    type = otl_var_varchar_long;
    str = otl_long_str_to_str(v.v);
}
void ColVal::Time(otl_value<otl_datetime> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    type = otl_var_timestamp;
    dt = v.v;
}
ColVal::~ColVal()
{
    str.~basic_string();
    dt.~DateTime();
}

ColVal::ColVal(DateTime v)
{
}
void ColVal::Time(DateTime v)
{
}
void ColVal::UInt32(otl_value<uint32_t> v)
{
    if(v.is_null())
    {
        null = true;
        return;
    }
    UInt32(v.v);
}
