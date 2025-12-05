/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_moisture.h
 * @description: 土壤含水文件格式
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_MOISTURE_H
#define SILLY_UTILS_SILLY_MOISTURE_H
#include <files/silly_file.h>

class MoistureIndex
{
  public:
    struct Info
    {
        int pid = 0;
        uint32_t index = 0;
        float lon = 0;
        float lat = 0;
    };
    using Cache = std::map<int, Info>;

  public:
    bool read(const suPath& file);
    bool write(const suPath& file, const MoistureIndex::Cache& cache);
    Cache m_cache;
};

class MoistureFile
{
  public:
    class Record
    {
      public:
        int pid = 0;
        int64_t time = 0;  // 时间戳 秒, 暂时没用
        float moisture = 0.0;
        float moisture_percent = 0.0;
        float precipitation = 0.0;  // 过去一小时实测降雨

        std::string serialize() const;
        bool deserialize(const std::string& data);
        static constexpr size_t serialized_size = 4 + 4 + 4 + 4 + 1;
    };
    /**
     * 将同一个时间段的数据,序列化到一个文件中
     * @param file
     * @param records
     */
    void serialize(const suPath& file, const std::vector<Record>& records);

    /**
     * 从序列化文件读取一个时间段的数据
     * @param file
     * @param records
     */
    void deserialize(const suPath& file, std::vector<Record>& records);

    /**
     * 读取指定格点序号的数据
     * @param file 土壤含水文件
     * @param cache 索引缓存
     * @param pid 格点序号
     * @param record 返回值
     * @return 是否存在指定格点信息
     */
    bool deserialize(const suPath& file, const MoistureIndex::Cache& cache, const int& pid, Record& record);

  private:
    int m_num = 0;
};

#endif  // SILLY_UTILS_SILLY_MOISTURE_H
