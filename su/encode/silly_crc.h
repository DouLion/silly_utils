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

namespace  silly
{

class crc
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

}




#endif  // SILLY_UTILS_SILLY_CRC_H
