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
#include <tzx/rwdb/silly_rwdb_record.h>
#define SILLY_PPTN_FORMAT_V1 1
#define SILLY_PPTN_FORMAT_V2 2
#define SILLY_PPTN_FORMAT_V3 3

class silly_pptn : public silly_rwdb_record
{
  public:
    std::string serialize(const int& ver = SILLY_PPTN_FORMAT_V1) const override;
    bool deserialize(const std::string& data) override;

  private:
    std::string serialize_v1() const;
    bool deserialize_v1(const std::string& data);
    std::string serialize_v2() const;
    bool deserialize_v2(const std::string& data);
    std::string serialize_v3() const;
    bool deserialize_v3(const std::string& data);

  public:
    float intv{0};
    float drp{0};
    // 固定长度  固定头1 版本信息 1 STCD 索引 4 int64_t 8, 时间戳 int 4, intv  4 uchar 雨量 float 4
    // static constexpr size_t SIZE_V1 = 1 + 1 + 4 + 8 + 4 + 4 + 4;

    static constexpr const size_t SIZE_V1 = 1 + 1 + 4 + 4 + 4 + 4;
    static constexpr const char PREFIX = 'Y';  // 固定头
    static constexpr const char* FILE_NAME = "PPTN.dat";
};

#endif  // SILLY_UTILS_SILLY_PPTN_H
