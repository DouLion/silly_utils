/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: silly_pptn.h
 * @description: silly_pptn 实测降雨记录
 * @version: v1.0.1 2024-09-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_PPTN_H
#define SILLY_UTILS_SILLY_PPTN_H
#include <su_marco.h>
#define SILLY_PPTN_FORMAT_V1 1
#define SILLY_PPTN_FORMAT_V2 2
#define SILLY_PPTN_FORMAT_V3 3

class silly_pptn
{
  public:
    silly_pptn()
    {
    }
    silly_pptn(const std::string& stcd, const std::time_t& stamp, const float& drp, const float& intv) : stcd(stcd), stamp(stamp), drp(drp), intv(intv)
    {
    }

    std::string serialize(const int& ver = SILLY_PPTN_FORMAT_V1);
    bool deserialize(const std::string& data);

  private:
    std::string serialize_v1();
    bool deserialize_v1(const std::string& data);
    std::string serialize_v2();
    bool deserialize_v2(const std::string& data);
    std::string serialize_v3();
    bool deserialize_v3(const std::string& data);

  public:
    std::string stcd;
    uint32_t index{0};
    std::time_t stamp{0};
    float intv{0};
    float drp{0};
    // 固定长度  固定头1 版本信息 1 STCD 索引 4 int64_t 8, 时间戳 int 4, intv  4 uchar 雨量 float 4
    // static constexpr size_t SIZE_V1 = 1 + 1 + 4 + 8 + 4 + 4 + 4;

    static constexpr size_t SIZE_V1 = 1 + 1 + 4 + 4 + 4 + 4;
    static constexpr char PREFIX = 'Y';  // 固定头
    static constexpr char* FILE_NAME = "PPTN.dat";
};

#endif  // SILLY_UTILS_SILLY_PPTN_H
