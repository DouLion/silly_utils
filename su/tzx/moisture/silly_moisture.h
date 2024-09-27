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

struct moisture_index_info
{
    int pid;
    uint32_t index;
    float lgtd;
    float lttd;
};
using moisture_index_cache = std::map<int, moisture_index_info> ;

class soil_moisture_record
{
  public:
    int pid;
    int64_t time{0}; // 时间戳 秒, 暂时没用
    float moisture{0.};
    float moisture_percent{0.};
    float precipitation; // 过去一小时实测降雨

    std::string serialize() const;
    bool deserialize(const std::string& data);
    const static size_t serialized_size = 4 + 4 + 4 + 4 + 1;
};

class silly_moisture_index
{
  public:
    bool read(const std::string& file);
    bool write(const std::string& file, const moisture_index_cache& cache);
    moisture_index_cache cache;
};

class silly_moisture
{
  public:
    silly_moisture() = default;
    ~silly_moisture() = default;
    /// 将同一个时间段的数据,序列化到一个文件中
    void serialize(const std::string& file, const std::vector<soil_moisture_record>& records);

    /// 从序列化文件读取一个时间段的数据
    void deserialize(const std::string& file, std::vector<soil_moisture_record>& records);

    /// 根据偏移位置读取一个
    bool deserialize(const std::string& file, const moisture_index_cache& cache, const int& pid, soil_moisture_record& record);

  private:
};

#endif  // SILLY_UTILS_SILLY_MOISTURE_H
