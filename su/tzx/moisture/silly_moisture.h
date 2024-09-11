/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_moisture.h
 * @description: silly_moisture 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_MOISTURE_H
#define SILLY_UTILS_SILLY_MOISTURE_H
#include <su_marco.h>
class soil_moisture_record
{
  public:
    int pid;
    int64_t time{0}; // 时间戳 秒, 暂时没用
    float moisture{0.};
    float moisture_percent{0.};
    float precipitation; // 过去一小时实测降雨
};

class silly_moisture
{
  public:
    silly_moisture();
    ~silly_moisture();
    /// 将同一个时间段的数据,序列化到一个文件中
    void serialize_by_time(const std::string& file, std::vector<soil_moisture_record>& records);

    /// 从序列化文件读取一个时间段的数据
    void deserialize_by_time(const std::string& file, std::vector<soil_moisture_record>& records);

  private:
};

#endif  // SILLY_UTILS_SILLY_MOISTURE_H
