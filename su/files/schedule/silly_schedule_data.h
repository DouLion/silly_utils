/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-11
 * @file: silly_schedule_data.cpp
 * @description: silly_schedule_data 头文件
 * @version: v1.0.1 2025-12-11 dou li yang
 */
#ifndef SILLY_SCHEDULE_DATA_H
#define SILLY_SCHEDULE_DATA_H
#include <files/silly_file.h>
#include <log/silly_log.h>
#include <json/silly_jsonpp.h>


class suScheduleData
{
  public:
    /// <summary>
    /// 文件描述
    /// </summary>
    struct fileDesc
    {
        std::time_t each = 300;  // 每300秒(5分钟) 一批数据
        std::string name;
        suPath root = suPath("./tmp");
        bool single = false;  // 是否作单个文件存储, 单个文件有利于读写分离
    };

    /// <summary>
    /// 数据描述
    /// </summary>
    struct cellDesc
    {
        std::string key;
        std::string type;
        double scale;
    };

  public:
    suScheduleData() = default;
    // deprecated, 后面记得删除
    suScheduleData(const supath& file);
    /// <summary>
    /// 获取数据
    /// </summary>
    /// <param name="name"></param>
    /// <param name="key"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    double get(const std::string& name, const std::string& key, std::vector<char>& data);

    /// <summary>
    /// 批量获取数据
    /// </summary>
    /// <param name="name"></param>
    /// <param name="keys"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    std::map<std::string, double> get(const std::string& name, const std::vector<std::string>& keys, std::vector<char>& data);

  public:
    std::map<std::string, std::vector<cellDesc>> name2desc;
    std::map<std::string, size_t> name2size;

    template <typename T>
    static double extractValue(const std::vector<char>& data, size_t offset, int scale)
    {
        double ret = 0.0;
        if (offset + sizeof(T) > data.size())
        {
            SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(T));
            return ret;
        }

        T value;
        memcpy(&value, data.data() + offset, sizeof(T));

        if (scale > 0)
        {
            ret = value / static_cast<double>(scale);
        }
        return ret;
    }
};
#endif  // SILLY_SCHEDULE_DATA_H
