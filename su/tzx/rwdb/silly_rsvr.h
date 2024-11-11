/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: silly_rsvr.h
 * @description: silly_rsvr 实测水库水位记录
 * @version: v1.0.1 2024-09-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_RSVR_H
#define SILLY_UTILS_SILLY_RSVR_H

#include <su_marco.h>
#define SILLY_RSVR_FORMAT_V1 1
#define SILLY_RSVR_FORMAT_V2 2
#define SILLY_RSVR_FORMAT_V3 3

class silly_rsvr
{
  public:
    silly_rsvr()
    {
    }
    //silly_rsvr(const std::string& stcd, const std::time_t& stamp, const float& z, const float& q) : stcd(stcd), stamp(stamp), zz(z), qq(q)
    //{
    //}

    std::string serialize(const int& ver = SILLY_RSVR_FORMAT_V1);
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
    uint32_t index{0}; // 索引 4
    std::time_t stamp{0}; // 时间戳 4
    float rz{0}; // 4
    float inq{0}; //4
    float w{0};            // 4
    float otq{0}; //4
    std::string rwchrcd;  // 一个字节 +长度2个字节
    std::string rwptn; // 一个字节+长度2个字节
    float inqdr{0};        // 4
    std::string msqmt; // 一个字节+长度2个字节
    float blrz{0};          // 4

    // 固定长度  固定头1 版本信息 1 STCD 索引 4 
    static constexpr size_t SIZE_V1 = 1 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2 + 4 + 2 + 4;

    static constexpr char PREFIX = 'H';  // 固定头
    static constexpr char* FILE_NAME = "RSVR.dat";
};








#endif  // SILLY_UTILS_SILLY_RSVR_H
