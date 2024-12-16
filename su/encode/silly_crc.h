/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: silly_crc.h
 * @description: silly_crc 类声明
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_CRC_H
#define SILLY_UTILS_SILLY_CRC_H
#include <su_marco.h>

static auto func_crc32_table = []() {
    std::vector<uint32_t> t(256);
    uint32_t poly = 0xEDB88320;
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t crc = i;
        for (uint32_t j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ poly;
            } else {
                crc >>= 1;
            }
        }
        t[i] = crc;
    }
    return t;
};

static auto func_crc16_table = []() {
    std::vector<uint16_t> t(256);
    uint16_t poly = 0x1021; // CRC-16/CCITT 多项式
    for (uint16_t i = 0; i < 256; ++i) {
        uint16_t crc = i << 8;
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
            crc &= 0xFFFF; // 确保结果为16位
        }
        t[i] = crc;
    }
    return t;
};

static auto func_crc64_table = []() {
    std::vector<uint64_t> t(256);
    uint64_t poly = 0x42F0E1EBA9EA3693ULL; // CRC-64/ECMA 多项式
    for (uint64_t i = 0; i < 256; ++i) {
        uint64_t crc = i << 56;
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000000000000000ULL) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
            crc &= 0xFFFFFFFFFFFFFFFFULL; // 确保结果为64位
        }
        t[i] = crc;
    }
    return t;
};


class silly_crc
{
  public:
    static uint16_t crc16(const char *data, size_t size);
    static uint16_t crc16(const std::string &data);

    static uint32_t crc32(const char *data, size_t size);
    static uint32_t crc32(const std::string &data);

    static uint64_t crc64(const char *data, size_t size);
    static uint64_t crc64(const std::string &data);

  private:
    static const std::vector<uint16_t> crc16_table;
    static const std::vector<uint32_t> crc32_table;
    static const std::vector<uint64_t> crc64_table;


};




#endif  // SILLY_UTILS_SILLY_CRC_H
