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
        std::string type_str;
        int type = 0;
        double scale = 1.0;
        int offset = 0;

        std::function<double(const std::vector<char>&)> func;
        std::function<std::vector<char>(const float&)> func_;
        double getValue(const std::vector<char>& data) const
        {
            if (!func)
            {
                SLOG_ERROR("key:{}未定义解析函数", key);
                return 0.0;
            }
            return func(data);
        }
        std::vector<char> convertValue(const float& data) const
        {
            std::vector<char> ret;
            if (!func_)
            {
                SLOG_ERROR("key:{}未定义解析函数", key);
                return ret;
            }
            return func_(data);
        }
        cellDesc() = default;

        // 解析函数绑定
        void bindFunc();
        void bindFunc_();
    };

  public:
    suScheduleData() = default;

    /// <summary>
    /// 初始化
    /// </summary>
    /// <param name="celldesc"></param>
    /// <returns></returns>
    bool init(std::vector<cellDesc>& celldesc);

    /// <summary>
    /// 获取数据
    /// </summary>
    /// <param name="key"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    double get(const std::string& key, const std::vector<char>& data);

    /// <summary>
    /// 批量获取数据
    /// </summary>
    /// <param name="keys"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    std::map<std::string, double> get(const std::vector<std::string>& keys, std::vector<char>& data);

    std::vector<char> convert(const std::vector<float>& code2data);

  public:
    std::vector<cellDesc> m_descs;  // 数据描述
    size_t m_size = 0;              // 数据块大小
};
#endif  // SILLY_SCHEDULE_DATA_H
