/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: silly_rsvr.cpp
 * @description: silly_rsvr实现
 * @version: v1.0.1 2024-09-30 dou li yang
 */

#include "silly_rsvr.h"
#include "silly_turn_tool.h"


std::string silly_rsvr::serialize(const int& ver)
{
    std::string result;
    switch (ver)
    {
        case SILLY_RSVR_FORMAT_V1:
            result = serialize_v1();
            break;
        case SILLY_RSVR_FORMAT_V2:
            result = serialize_v2();
            break;
        case SILLY_RSVR_FORMAT_V3:
            result = serialize_v3();
            break;
        default:
            break;
    }
    return result;
}
bool silly_rsvr::deserialize(const std::string& data)
{
    bool status = false;
    if (data.size() < 2 || data[0] != PREFIX)
    {
        return status;
    }
    char ver = data[1];
    switch (ver)
    {
        case SILLY_RSVR_FORMAT_V1:
            status = deserialize_v1(data);
            break;
        case SILLY_RSVR_FORMAT_V2:
            status = deserialize_v2(data);
            break;
        case SILLY_RSVR_FORMAT_V3:
            status = deserialize_v3(data);
            break;
        default:
            break;
    }

    return status;
}

std::string silly_rsvr::serialize_v1()
{
    std::string result;
    result.resize(SIZE_V1);
    char* p = (char*)result.data();
    p[0] = PREFIX;
    p[1] = SILLY_RSVR_FORMAT_V1;
    p += 2;
    memcpy(p, &index, sizeof(index));
    p += sizeof(index);
    uint32_t stamp_sec = static_cast<uint32_t>(stamp);
    memcpy(p, &stamp_sec, sizeof(stamp_sec));
    p += sizeof(stamp_sec);

    memcpy(p, &rz, sizeof(rz));
    p += sizeof(rz);
    memcpy(p, &inq, sizeof(inq));
    p += sizeof(inq);
    memcpy(p, &w, sizeof(w));
    p += sizeof(w);
    memcpy(p, &otq, sizeof(otq));
    p += sizeof(otq);
    std::string t_rwchrcd = str2bin(rwchrcd);
    memcpy(p, &t_rwchrcd[0], 2);
    p += 2;
    std::string t_rwptn = str2bin(rwptn);
    memcpy(p, &t_rwptn[0], 2);
    p += 2;
    memcpy(p, &inqdr, sizeof(inqdr));
    p += sizeof(inqdr);
    std::string t_msqmt = str2bin(msqmt);
    memcpy(p, &t_msqmt[0], 2);
    p += 2;
    memcpy(p, &blrz, sizeof(blrz));

    return result;
}

bool silly_rsvr::deserialize_v1(const std::string& data)
{
    bool status = false;
    if (data.size() != silly_rsvr::SIZE_V1)
    {
        return status;
    }
    const char* p = data.data();
    p += 2;
    index = P2UINT32(p);
    p += sizeof(index);

    uint32_t stamp_sec = P2UINT32(p);
    p += sizeof(stamp_sec);
    stamp = static_cast<std::time_t>(stamp_sec);
    rz = P2FLOAT(p);
    p += sizeof(rz);
    inq = P2FLOAT(p);
    p += sizeof(inq);
    w = P2FLOAT(p);
    p += sizeof(w);
    otq = P2FLOAT(p);
    p += sizeof(otq);
    rwchrcd = bin2str(p);
    p += 2;
    rwptn = bin2str(p);
    p += 2;
    inqdr = P2FLOAT(p);
    p += 2;
    msqmt = bin2str(p);
    p += 2;
    blrz = P2FLOAT(p);

    status = true;
    return status;
}

std::string silly_rsvr::serialize_v2()
{
    return std::string();
}
bool silly_rsvr::deserialize_v2(const std::string& data)
{
    return false;
}
std::string silly_rsvr::serialize_v3()
{
    return std::string();
}
bool silly_rsvr::deserialize_v3(const std::string& data)
{
    return false;
}
