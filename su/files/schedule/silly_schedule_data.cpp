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

bool suScheduleData::init(std::map<std::string, std::vector<cellDesc>>& celldesc)
{
    if (celldesc.empty())
    {
        SLOG_ERROR("输入数据为空");
        return false;
    }
    name2desc = celldesc;
    for (auto& [name, desc] : celldesc)
    {
        int type = desc[desc.size() - 1].type;
        name2size[name] = desc[desc.size() - 1].offset + TYPE_SIZE[type];
    }
    return true;
}

suScheduleData::suScheduleData(const supath& file)
{
    Json::Value root = silly_jsonpp::read(file);
    if (root.isNull() || !root.isObject())
    {
        SLOG_ERROR("读取json文件失败");
        return;
    }

    for (auto& member : root.getMemberNames())
    {
        std::string type;
        double scale = 0.0;
        std::string key;
        int offset = 0;
        for (auto& value : root[member])
        {
            if (!sujson::check_str(value, "key", key))
            {
                SLOG_ERROR("缺少key字段");
                return;
            }
            if (!sujson::check_str(value, "type", type))
            {
                SLOG_ERROR("缺少type字段");
                return;
            }
            if (!sujson::check_double(value, "scale", scale))
            {
                SLOG_ERROR("缺少scale字段");
                return;
            }
            name2desc[member].push_back(cellDesc{key, TYPE_INDEX[type], scale, offset});
            name2size[member] += TYPE_SIZE[TYPE_INDEX[type]];
            offset += TYPE_SIZE[TYPE_INDEX[type]];
        }
    }
}

double suScheduleData::get(const std::string& name, const std::string& key, std::vector<char>& data)
{
    double ret = 0.0;
    if (name2desc.count(name) == 0)
    {
        SLOG_ERROR("没有{}的数据", name);
        return ret;
    }

    if (data.size() < name2size[name])
    {
        SLOG_ERROR("数据长度不足:{} ", name);
    }
    for (const auto& desc : name2desc[name])
    {
        if (desc.key == key)
        {
            // ret = desc.getValue(data);
            ret = parseFunctions[desc.type](data.data() + desc.offset, desc.scale);
            return ret;
        }
    }

    return ret;
}

std::map<std::string, double> suScheduleData::get(const std::string& name, const std::vector<std::string>& keys, std::vector<char>& data)
{
    std::map<std::string, double> ret;
    for (auto& key : keys)
    {
        ret[key] = get(name, key, data);
    }
    return ret;
}


double suScheduleData::cellDesc::getValue(const std::vector<char>& data) const
{
    switch (type)
    {
        case SCHEDULE_DATA_TYPE_INT8:
            return extractValue<int8_t>(data, offset, scale);
        case SCHEDULE_DATA_TYPE_INT16:
            return extractValue<int16_t>(data, offset, scale);
        case SCHEDULE_DATA_TYPE_INT32:
            return extractValue<int32_t>(data, offset, scale);
        case SCHEDULE_DATA_TYPE_INT64:
            return extractValue<int64_t>(data, offset, scale);
        case SCHEDULE_DATA_TYPE_FLOAT:
            return extractValue<float>(data, offset, scale);
        case SCHEDULE_DATA_TYPE_DOUBLE:
            return extractValue<double>(data, offset, scale);
        default:
            SLOG_ERROR("未知类型:{}", type);
            break;
    }
    return 0.0;
}
