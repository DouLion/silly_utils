/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: silly_crc.cpp
 * @description: silly_crc实现
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#include "silly_crc.h"

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


const std::vector<uint16_t> silly::crc::crc16_table = func_crc16_table();
const std::vector<uint32_t> silly::crc::crc32_table = func_crc32_table();
const std::vector<uint64_t> silly::crc::crc64_table = func_crc64_table();


uint16_t silly::crc::crc16(const char *data, size_t size)
{
    std::string str;
    str.assign(data, size);
    return crc16(str);
}

uint16_t silly::crc::crc16(const std::string &data)
{
    uint16_t crc = 0xFFFF;  // 初始值
    for (char byte : data)
    {
        uint8_t index = (crc >> 8) ^ static_cast<uint8_t>(byte);
        crc = (crc << 8) ^ crc16_table[index];
        crc &= 0xFFFF;  // 确保结果为16位
    }
    return crc ^ 0x0000;  // 最终异或（对于 CRC-16/CCITT，通常不进行最终异或）
}

uint32_t silly::crc::crc32(const char *data, size_t size)
{
    std::string str;
    str.assign(data, size);
    return crc32(str);
}

uint32_t silly::crc::crc32(const std::string &data)
{
    uint32_t crc = 0xFFFFFFFF;  // 初始值
    for (char byte : data)
    {
        crc = (crc >> 8) ^ crc32_table[(crc & 0xFF) ^ static_cast<uint8_t>(byte)];
    }
    return crc ^ 0xFFFFFFFF;  // 最终异或
}

uint64_t silly::crc::crc64(const char *data, size_t size)
{
    std::string str;
    str.assign(data, size);
    return crc64(str);
}

uint64_t silly::crc::crc64(const std::string &data)
{
    uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;  // 初始值
    for (char byte : data)
    {
        uint8_t index = static_cast<uint8_t>(crc >> 56) ^ static_cast<uint8_t>(byte);
        crc = (crc << 8) ^ crc64_table[index];
        crc &= 0xFFFFFFFFFFFFFFFFULL;  // 确保结果为64位
    }
    return crc ^ 0xFFFFFFFFFFFFFFFFULL;  // 最终异或
}
