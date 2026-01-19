/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-11
 * @file: silly_schedule_data.c
 * @description: silly_schedule_data实现
 * @version: v1.0.1 2025-12-11 dou li yang
 */
#include "silly_schedule_data.h"

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
static std::unordered_map<int, size_t> TYPE_SIZE = {{SCHEDULE_DATA_TYPE_INT8, sizeof(int8_t)},
                                                    {SCHEDULE_DATA_TYPE_INT16, sizeof(int16_t)},
                                                    {SCHEDULE_DATA_TYPE_INT32, sizeof(int32_t)},
                                                    {SCHEDULE_DATA_TYPE_INT64, sizeof(int64_t)},
                                                    {SCHEDULE_DATA_TYPE_FLOAT, sizeof(float)},
                                                    {SCHEDULE_DATA_TYPE_DOUBLE, sizeof(double)}};

static std::unordered_map<std::string, int> TYPE_INDEX =
    {{"int8_t", SCHEDULE_DATA_TYPE_INT8}, {"int16_t", SCHEDULE_DATA_TYPE_INT16}, {"int32_t", SCHEDULE_DATA_TYPE_INT32}, {"int64_t", SCHEDULE_DATA_TYPE_INT64}, {"float", SCHEDULE_DATA_TYPE_FLOAT}, {"double", SCHEDULE_DATA_TYPE_DOUBLE}};

bool suScheduleData::init(std::vector<cellDesc>& celldesc)
{
    if (celldesc.empty())
    {
        SLOG_ERROR("输入数据为空");
        return false;
    }
    m_descs = celldesc;

    int offset = 0;
    for (auto& desc : m_descs)
    {
        // 1. 检查类型是否合法，转enum 类型
        if (!HAS(TYPE_INDEX, desc.type_str))
        {
            SLOG_ERROR("类型{}不合法", desc.type_str);
            break;
        }
        desc.type = TYPE_INDEX[desc.type_str];

        // 2.bind函数
        desc.bindFunc();

        // 3. 计算偏移量
        desc.offset = offset;
        offset += TYPE_SIZE[desc.type];
    }
    //数据块大小
    m_size = offset;

    return true;
}

double suScheduleData::get(const std::string& key, const std::vector<char>& data)
{
    double ret = 0.0;

    if (data.size() < m_size)
    {
        SLOG_ERROR("数据长度不足:{} ", m_size);
        return ret;
    }
    for (const auto& desc : m_descs)
    {
        if (desc.key == key)
        {
            ret = desc.getValue(data);
            return ret;
        }
    }

    return ret;
}

std::map<std::string, double> suScheduleData::get(const std::vector<std::string>& keys, std::vector<char>& data)
{
    std::map<std::string, double> ret;
    for (auto& key : keys)
    {
        ret[key] = get(key, data);
    }
    return ret;
}

// 绑定解析函数
void suScheduleData::cellDesc::bindFunc()
{
    switch (type)
    {
        case SCHEDULE_DATA_TYPE_INT8:
            func = [this](const std::vector<char>& data) {
                if (offset + sizeof(int8_t) > data.size())
                {
                    SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(int8_t));
                    return 0.0;
                }
                int8_t* value = (int8_t*)(data.data() + offset);

                if (scale > 0 && scale != 1)
                {
                    return static_cast<double>(*value) / scale;
                }
                return static_cast<double>(*value);
            };
            break;
        case SCHEDULE_DATA_TYPE_INT16:
            func = [this](const std::vector<char>& data) {
                if (offset + sizeof(int16_t) > data.size())
                {
                    SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(int16_t));
                    return 0.0;
                }
                int16_t* value = (int16_t*)(data.data() + offset);

                if (scale > 0 && scale != 1)
                {
                    return static_cast<double>(*value) / scale;
                }
                return static_cast<double>(*value);
            };
            break;
        case SCHEDULE_DATA_TYPE_INT32:
            func = [this](const std::vector<char>& data) {
                if (offset + sizeof(int32_t) > data.size())
                {
                    SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(int32_t));
                    return 0.0;
                }
                int32_t* value = (int32_t*)(data.data() + offset);

                if (scale > 0 && scale != 1)
                {
                    return static_cast<double>(*value) / scale;
                }
                return static_cast<double>(*value);
            };
            break;
        case SCHEDULE_DATA_TYPE_INT64:
            func = [this](const std::vector<char>& data) {
                if (offset + sizeof(int64_t) > data.size())
                {
                    SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(int64_t));
                    return 0.0;
                }
                int64_t* value = (int64_t*)(data.data() + offset);

                if (scale > 0 && scale != 1)
                {
                    return static_cast<double>(*value) / scale;
                }
                return static_cast<double>(*value);
            };
            break;
        case SCHEDULE_DATA_TYPE_FLOAT:
            func = [this](const std::vector<char>& data) {
                if (offset + sizeof(float) > data.size())
                {
                    SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(float));
                    return 0.0;
                }
                float* value = (float*)(data.data() + offset);

                if (scale > 0 && scale != 1)
                {
                    return static_cast<double>(*value) / scale;
                }
                return static_cast<double>(*value);
            };
            break;
        case SCHEDULE_DATA_TYPE_DOUBLE:
            func = [this](const std::vector<char>& data) {
                if (offset + sizeof(double) > data.size())
                {
                    SLOG_ERROR("数据长度:{},计算长度:{}", data.size(), offset + sizeof(double));
                    return 0.0;
                }
                double* value = (double*)(data.data() + offset);

                if (scale > 0 && scale != 1)
                {
                    return static_cast<double>(*value) / scale;
                }
                return static_cast<double>(*value);
            };
            break;

        default:
            func = nullptr;
            break;
    }
}