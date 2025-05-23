/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: silly_river.cpp
 * @description: silly_river实现
 * @version: v1.0.1 2024-09-30 dou li yang
 */
#include "silly_river.h"
#include "silly_turn_tool.h"

std::string silly_river::serialize(const int& ver) const
{
    std::string result;
    switch (ver)
    {
        case SILLY_RIVER_FORMAT_V1:
            result = serialize_v1();
            break;
        case SILLY_RIVER_FORMAT_V2:
            result = serialize_v2();
            break;
        case SILLY_RIVER_FORMAT_V3:
            result = serialize_v3();
            break;
        default:
            break;
    }
    return result;
}
bool silly_river::deserialize(const std::string& data)
{
    bool status = false;
    if (data.size() < 2 || data[0] != PREFIX)
    {
        return status;
    }
    char ver = data[1];
    switch (ver)
    {
        case SILLY_RIVER_FORMAT_V1:
            status = deserialize_v1(data);
            break;
        case SILLY_RIVER_FORMAT_V2:
            status = deserialize_v2(data);
            break;
        case SILLY_RIVER_FORMAT_V3:
            status = deserialize_v3(data);
            break;
        default:
            break;
    }

    return status;
}

std::string silly_river::serialize_v1() const
{
    std::string result;
    result.resize(SIZE_V1);
    char* p = (char*)result.data();
    p[0] = PREFIX;
    p[1] = SILLY_RIVER_FORMAT_V1;
    p += 2;
    memcpy(p, &index, sizeof(index));
    p += sizeof(index);

    // memcpy(p, stcd.c_str(), sizeof(int64_t));
    // p += sizeof(int64_t);

    uint32_t stamp_sec = static_cast<uint32_t>(stamp);
    memcpy(p, &stamp_sec, sizeof(stamp_sec));
    p += sizeof(stamp_sec);

    memcpy(p, &zz, sizeof(zz));
    p += sizeof(zz);
    memcpy(p, &qq, sizeof(qq));

    return result;
}


bool silly_river::deserialize_v1(const std::string& data)
{
    bool status = false;
    if (data.size() != silly_river::SIZE_V1)
    {
        return status;
    }
    char* p = (char*)data.data();
    p += 2;
    index = P2UINT32(p);
    p += sizeof(index);

    // int64_t i64_stcd = P2INT64(p);
    // p += sizeof(int64_t);
    // memcpy(&stcd[0], &i64_stcd, sizeof(int64_t));

    uint32_t stamp_sec = P2UINT32(p);
    p += sizeof(stamp_sec);
    stamp = static_cast<std::time_t>(stamp_sec);
    zz = P2FLOAT(p);
    p += sizeof(zz);
    qq = P2FLOAT(p);
    status = true;
    return status;
}

std::string silly_river::serialize_v2() const
{
    std::string result;
    result.resize(SIZE_V2);
    char* p = (char*)result.data();
    p[0] = PREFIX;
    p[1] = SILLY_RIVER_FORMAT_V2;
    p += 2;
    memcpy(p, &index, sizeof(index));
    p += sizeof(index);

    // memcpy(p, stcd.c_str(), sizeof(int64_t));
    // p += sizeof(int64_t);

    uint32_t stamp_sec = static_cast<uint32_t>(stamp);
    memcpy(p, &stamp_sec, sizeof(stamp_sec));
    p += sizeof(stamp_sec);

    memcpy(p, &zz, sizeof(zz));
    p += sizeof(zz);
    memcpy(p, &qq, sizeof(qq));
    p += sizeof(qq);
    std::string tmp_wptn = wptn;
    if (wptn.empty())
    {
        tmp_wptn = "0";
    }
    std::string wp = str2bin(tmp_wptn);
    memcpy(p, &wp[0], 2);

    return result;
}
bool silly_river::deserialize_v2(const std::string& data)
{
    bool status = false;
    if (data.size() != silly_river::SIZE_V2)
    {
        return status;
    }
    char* p = (char*)data.data();
    p += 2;
    index = P2UINT32(p);
    p += sizeof(index);

    // int64_t i64_stcd = P2INT64(p);
    // p += sizeof(int64_t);
    // memcpy(&stcd[0], &i64_stcd, sizeof(int64_t));

    uint32_t stamp_sec = P2UINT32(p);
    p += sizeof(stamp_sec);
    stamp = static_cast<std::time_t>(stamp_sec);
    zz = P2FLOAT(p);
    p += sizeof(zz);
    qq = P2FLOAT(p);
    p += sizeof(qq);
    wptn = bin2str(p);
    status = true;
    return status;
}
std::string silly_river::serialize_v3() const
{
    return std::string();
}
bool silly_river::deserialize_v3(const std::string& data)
{
    return false;
}














