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
    enum SCHEDULE_DATA_TYPE
    {
        SCHEDULE_DATA_TYPE_INT8 = 0,
        SCHEDULE_DATA_TYPE_INT16,
        SCHEDULE_DATA_TYPE_INT32,
        SCHEDULE_DATA_TYPE_INT64,
        SCHEDULE_DATA_TYPE_FLOAT,
        SCHEDULE_DATA_TYPE_DOUBLE
    };

    // 类型大小映射表
    static inline std::unordered_map<int, size_t> TYPE_SIZE = {{SCHEDULE_DATA_TYPE_INT8, sizeof(int8_t)},
                                                               {SCHEDULE_DATA_TYPE_INT16, sizeof(int16_t)},
                                                               {SCHEDULE_DATA_TYPE_INT32, sizeof(int32_t)},
                                                               {SCHEDULE_DATA_TYPE_INT64, sizeof(int64_t)},
                                                               {SCHEDULE_DATA_TYPE_FLOAT, sizeof(float)},
                                                               {SCHEDULE_DATA_TYPE_DOUBLE, sizeof(double)}};

    static inline std::unordered_map<std::string, int> TYPE_INDEX =
        {{"int8_t", SCHEDULE_DATA_TYPE_INT8}, {"int16_t", SCHEDULE_DATA_TYPE_INT16}, {"int32_t", SCHEDULE_DATA_TYPE_INT32}, {"int64_t", SCHEDULE_DATA_TYPE_INT64}, {"float", SCHEDULE_DATA_TYPE_FLOAT}, {"double", SCHEDULE_DATA_TYPE_DOUBLE}};

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
        int type = 0;
        double scale = 1.0;
        int offset = 0;

        std::function<double(const std::vector<char>&)> func;
        double getValue(const std::vector<char>& data) const
        {
            if (!func)
            {
                SLOG_ERROR("key:{}未定义解析函数", key);
                return 0.0;
            }
            return func(data);
        }
        cellDesc() = default;
        cellDesc(const std::string& k, int t = 0, double s = 1.0, int o = 0)  // t默认=0
            : key(k), type(t), scale(s), offset(o)
        {
            bindFunc();  // 自动绑定
        }

        // 拷贝构造函数：重新绑定函数
        cellDesc(const cellDesc& other) : key(other.key), type(other.type), scale(other.scale), offset(other.offset)
        {
            bindFunc();  // 重新绑定，避免this指针问题
        }

        //解析函数绑定
        void bindFunc();

      private:
        template <typename T>
        double extractValue(const std::vector<char>& data, int offset, double scale)
        {
            double ret = 0.0;
            if (offset + sizeof(T) > data.size())
            {
                SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(T));
                return ret;
            }

            T* value = (T*)(data.data() + offset);

            if (scale > 0 && scale != 1)
            {
                return static_cast<double>(*value) / scale;
            }
            return static_cast<double>(*value);
        }
    };

  public:
    suScheduleData() = default;

    /// <summary>
    /// 初始化
    /// </summary>
    /// <param name="celldesc"></param>
    /// <returns></returns>
    bool init(std::map<std::string, std::vector<cellDesc>>& celldesc);

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
};
#endif  // SILLY_SCHEDULE_DATA_H
