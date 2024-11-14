/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-14
 * @file: silly_rwdb_record.h
 * @description: silly_rwdb_record 类声明
 * @version: v1.0.1 2024-11-14 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_RWDB_RECORD_H
#define SILLY_UTILS_SILLY_RWDB_RECORD_H
#include <su_marco.h>

class silly_rwdb_record
{
  public:

    virtual std::string serialize(const int& ver = 1) const = 0;
    virtual bool deserialize(const std::string& data) = 0;

  public:
    std::string stcd;
    uint32_t index{0};
    std::time_t stamp{0};

    static constexpr size_t SIZE_V1 = 0;

    static constexpr char PREFIX = 'R';  // 固定头
    static constexpr char* FILE_NAME = "";
};

#endif  // SILLY_UTILS_SILLY_RWDB_RECORD_H
