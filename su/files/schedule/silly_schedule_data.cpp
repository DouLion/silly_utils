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

suScheduleData::suScheduleData(std::string& jsonfile)
{
    Json::Value root;
    root = silly_jsonpp::read(suPath(jsonfile));
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
        for (auto& value : root[member])
        {
            if (!silly::jsonpp::check_str(value, "key", key))
            {
                SLOG_ERROR("缺少key字段");
                return;
            }
            if (!silly::jsonpp::check_str(value, "type", type))
            {
                SLOG_ERROR("缺少type字段");
                return;
            }
            if (!silly::jsonpp::check_double(value, "scale", scale))
            {
                SLOG_ERROR("缺少scale字段");
                return;
            }
            name2desc[member].push_back(cellDesc{key, type, scale});
            name2size[member] += TYPE_SIZE[type];
        }
    }
}

double suScheduleData::get(const std::string& name, const std::string& key, std::vector<char>& data)
{
    double ret = 0.0;
    if (name2desc.count(name) == 0)
    {
        SLOG_ERROR("缺少key字段");
        return ret;
    }

    int offset = 0;
    for (auto& desc : name2desc[name])
    {
        if (desc.key == key)
        {
            if (desc.type == "int16_t")
            {
                ret = extractValue<int16_t>(data, offset, desc.scale);
                break;
            }
            else if (desc.type == "float")
            {
                ret = extractValue<float>(data, offset, desc.scale);
                break;
            }
            else if (desc.type == "double")
            {
                ret = extractValue<double>(data, offset, desc.scale);
                break;
            }
            else if (desc.type == "int32_t")
            {
                ret = extractValue<int32_t>(data, offset, desc.scale);
                break;
            }
        }
        else
        {
            offset += TYPE_SIZE[desc.type];
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
