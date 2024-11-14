/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: silly_stbprp.cpp
 * @description: silly_stbprp实现
 * @version: v1.0.1 2024-09-30 dou li yang
 */
#include "silly_stbprp.h"
#include "silly_turn_tool.h"



std::string silly_stbprp::serialize() const
{
    std::string result;
    result.append("    ");
    result.append(v2bin(index));
    result.append(str2bin(STCD));
    result.append(str2bin(STNM));
    result.append(str2bin(RVNM));
    result.append(str2bin(HNNM));
    result.append(str2bin(BSNM));
    result.append(v2bin(LGTD));
    result.append(v2bin(LTTD));
    result.append(str2bin(STLC));
    result.append(str2bin(ADDVCD));
    result.append(str2bin(DTMNM));
    result.append(v2bin(DTMEL));
    result.append(v2bin(DTPR));
    result.append(str2bin(STTP));
    result.append(str2bin(FRGRD));
    result.append(str2bin(ESSTYM));
    result.append(str2bin(BGFRYM));
    result.append(str2bin(ATCUNIT));
    result.append(str2bin(ADMAUTH));
    result.append(str2bin(LOCALITY));
    result.append(str2bin(STBK));
    result.append(v2bin(STAzt));
    result.append(v2bin(DSTRVM));
    result.append(v2bin(DRNA));
    result.append(str2bin(PHCD));
    result.append(str2bin(USFL));
    result.append(str2bin(COMMENTS));
    result.append(str2bin(MODITIME));
    result.append(str2bin(HNNM0));
    result.append(str2bin(ADCD));
    result.append(str2bin(ADDVCD1));
    uint32_t size = result.size();
    memcpy(&result[0], &size, 4);
    return result;
}
bool silly_stbprp::deserialize(const std::string& data)
{
    if (data.size() < 4)
    {
        // 数据长度不足以包含头部大小信息
        return false;
    }

    uint32_t size;
    memcpy(&size, &data[0], 4);

    if (data.size() != size)
    {
        // 数据长度与头部记录的大小不符
        return false;
    }

    // 解析数据
    const char* ptr = &data[4];

    // 解析各个字段
    memcpy(&index, ptr, sizeof(index));
    ptr += sizeof(index);

    STCD = bin2str(ptr);
    ptr += 1 + STCD.size();

    STNM = bin2str(ptr);
    ptr += 1 + STNM.size();

    RVNM = bin2str(ptr);
    ptr += 1 + RVNM.size();

    HNNM = bin2str(ptr);
    ptr += 1 + HNNM.size();

    BSNM = bin2str(ptr);
    ptr += 1 + BSNM.size();

    memcpy(&LGTD, ptr, sizeof(LGTD));
    ptr += sizeof(LGTD);

    memcpy(&LTTD, ptr, sizeof(LTTD));
    ptr += sizeof(LTTD);

    STLC = bin2str(ptr);
    ptr += 1 + STLC.size();

    ADDVCD = bin2str(ptr);
    ptr += 1 + ADDVCD.size();

    DTMNM = bin2str(ptr);
    ptr += 1 + DTMNM.size();

    memcpy(&DTMEL, ptr, sizeof(DTMEL));
    ptr += sizeof(DTMEL);

    memcpy(&DTPR, ptr, sizeof(DTPR));
    ptr += sizeof(DTPR);

    STTP = bin2str(ptr);
    ptr += 1 + STTP.size();

    FRGRD = bin2str(ptr);
    ptr += 1 + FRGRD.size();

    ESSTYM = bin2str(ptr);
    ptr += 1 + ESSTYM.size();

    BGFRYM = bin2str(ptr);
    ptr += 1 + BGFRYM.size();

    ATCUNIT = bin2str(ptr);
    ptr += 1 + ATCUNIT.size();

    ADMAUTH = bin2str(ptr);
    ptr += 1 + ADMAUTH.size();

    LOCALITY = bin2str(ptr);
    ptr += 1 + LOCALITY.size();

    STBK = bin2str(ptr);
    ptr += 1 + STBK.size();

    memcpy(&STAzt, ptr, sizeof(STAzt));
    ptr += sizeof(STAzt);

    memcpy(&DSTRVM, ptr, sizeof(DSTRVM));
    ptr += sizeof(DSTRVM);

    memcpy(&DRNA, ptr, sizeof(DRNA));
    ptr += sizeof(DRNA);

    PHCD = bin2str(ptr);
    ptr += 1 + PHCD.size();

    USFL = bin2str(ptr);
    ptr += 1 + USFL.size();

    COMMENTS = bin2str(ptr);
    ptr += 1 + COMMENTS.size();

    MODITIME = bin2str(ptr);
    ptr += 1 + MODITIME.size();

    HNNM0 = bin2str(ptr);
    ptr += 1 + HNNM0.size();

    ADCD = bin2str(ptr);
    ptr += 1 + ADCD.size();

    ADDVCD1 = bin2str(ptr);
    ptr += 1 + ADDVCD1.size();

    // 检查是否解析完成
    if (ptr != &data[size])
    {
        return false;
    }

    return true;
}
