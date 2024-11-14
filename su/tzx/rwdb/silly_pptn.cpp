/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: silly_pptn.cpp
 * @description: silly_pptn实现
 * @version: v1.0.1 2024-09-30 dou li yang
 */
#include "silly_pptn.h"
#include "silly_turn_tool.h"

std::string silly_pptn::serialize(const int& ver) const
{
    std::string result;
    switch (ver)
    {
        case SILLY_PPTN_FORMAT_V1:
            result = serialize_v1();
            break;
        case SILLY_PPTN_FORMAT_V2:
            result = serialize_v2();
            break;
        case SILLY_PPTN_FORMAT_V3:
            result = serialize_v3();
            break;
        default:
            break;
    }
    return result;
}
bool silly_pptn::deserialize(const std::string& data)
{
    bool status = false;
    if (data.size() < 2 || data[0] != PREFIX)
    {
        return status;
    }
    char ver = data[1];
    switch (ver)
    {
        case SILLY_PPTN_FORMAT_V1:
            status = deserialize_v1(data);
            break;
        case SILLY_PPTN_FORMAT_V2:
            status = deserialize_v2(data);
            break;
        case SILLY_PPTN_FORMAT_V3:
            status = deserialize_v3(data);
            break;
        default:
            break;
    }

    return status;
}


std::string silly_pptn::serialize_v1() const 
{
    std::string result;
    result.resize(SIZE_V1);
    char* p = (char*)result.data();
    p[0] = PREFIX;
    p[1] = SILLY_PPTN_FORMAT_V1;
    p += 2;
    memcpy(p, &index, sizeof(index));
    p += sizeof(index);

    //memcpy(p, stcd.c_str(), sizeof(int64_t));
    //p += sizeof(int64_t);

    uint32_t stamp_sec = static_cast<uint32_t>(stamp);
    memcpy(p, &stamp_sec, sizeof(stamp_sec));
    p += sizeof(stamp_sec);
    
    int intv_i = static_cast<int>(intv);  // 整数部分
    float intv_f = intv - intv_i;         // 小数部分
    int intv_min = intv_i * 60 + intv_f * 100;
    memcpy(p, &intv_min, sizeof(intv_min));
    p += sizeof(intv_min);

    memcpy(p, &drp, sizeof(drp));

    return result;
}

bool silly_pptn::deserialize_v1(const std::string& data)
{
    bool status = false;
    if (data.size() != silly_pptn::SIZE_V1)
    {
        return status;
    }
    char* p = (char*)data.data();
    p += 2;
    index = P2UINT32(p);
    p += sizeof(index);

    //int64_t i64_stcd = P2INT64(p);
    //p += sizeof(int64_t);
    //memcpy(&stcd[0], &i64_stcd, sizeof(int64_t));

    uint32_t stamp_sec = P2UINT32(p);
    p += sizeof(stamp_sec);
    stamp = static_cast<std::time_t>(stamp_sec);
    int intv_min = P2INT32(p);
    p += sizeof(intv_min);
    intv = intv_min / 60 + (intv_min % 60) / 100.;
    drp = P2FLOAT(p);
    p += sizeof(drp);
    status = true;
    return status;
}

std::string silly_pptn::serialize_v2() const
{
    return std::string();
}
bool silly_pptn::deserialize_v2(const std::string& data)
{
    return false;
}
std::string silly_pptn::serialize_v3() const
{
    return std::string();
}
bool silly_pptn::deserialize_v3(const std::string& data)
{
    return false;
}
