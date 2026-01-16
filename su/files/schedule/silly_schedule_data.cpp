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

bool suScheduleData::init(const std::map<std::string, std::vector<cellDesc>>& celldesc)
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

double suScheduleData::get(const std::string& name, const std::string& key, const std::vector<char>& data)
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
        return ret;
    }
    for (const auto& desc : name2desc[name])
    {
        if (desc.key == key)
        {
            ret = desc.getValue(data);
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

// 绑定解析函数
void suScheduleData::cellDesc::bindFunc()
{
    switch (type)
    {
        case SCHEDULE_DATA_TYPE_INT8:
            func = [this](const std::vector<char>& data) { return extractValue<int8_t>(data, offset, scale); };
            break;
        case SCHEDULE_DATA_TYPE_INT16:
            func = [this](const std::vector<char>& data) { return extractValue<int16_t>(data, offset, scale); };
            break;
        case SCHEDULE_DATA_TYPE_INT32:
            func = [this](const std::vector<char>& data) { return extractValue<int32_t>(data, offset, scale); };
            break;
        case SCHEDULE_DATA_TYPE_INT64:
            func = [this](const std::vector<char>& data) { return extractValue<int64_t>(data, offset, scale); };
            break;
        case SCHEDULE_DATA_TYPE_FLOAT:
            func = [this](const std::vector<char>& data) { return extractValue<float>(data, offset, scale); };
            break;
        case SCHEDULE_DATA_TYPE_DOUBLE:
            func = [this](const std::vector<char>& data) { return extractValue<double>(data, offset, scale); };
            break;

        default:
            func = nullptr;
            break;
    }
}