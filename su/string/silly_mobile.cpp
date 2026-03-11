/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-04-18
 * @file: silly_mobile.cpp
 * @description: silly_mobile实现
 * @version: v1.0.1 2025-04-18 dou li yang
 */
#include "silly_mobile.h"

bool suMobileDB::Load(const supath& file)
{
    m_data_time = file.string().substr(file.string().length() - 10, 6);
    SLOG_INFO("数据收集时间: {}", m_data_time)
    std::vector<std::string> lines = sufile::readlines(file);
    lines.front().clear();
    for (const auto& line : lines)
    {
        std::vector<std::string> fields = SPLIT(line, ',');
        if (fields.size() == 8)
        {
            Info attr;
            attr.prefix = fields[0];
            attr.segment = fields[1];
            attr.province = fields[2];
            attr.city = fields[3];
            attr.carrier = fields[4];
            attr.area = fields[5];
            attr.postal = fields[6];
            attr.region = fields[7];

            m_prefix2info[attr.segment] = attr;
        }
    }
    return true;
}

std::optional<suMobileDB::Info> suMobileDB::Query(const std::string& number) const
{
    if (!IsValid(number))
    {
        return std::nullopt;
    }
    if (HAS(m_prefix2info, number.substr(0, 7)))
    {
        const auto& it = m_prefix2info.find(number.substr(0, 7));
        return it->second;
    }
    return std::nullopt;
}

std::vector<suMobileDB::Info> suMobileDB::Query(const std::vector<std::string>& number) const
{
    std::vector<suMobileDB::Info> infos;
    for (const auto& num : number)
    {
        auto opt = Query(num);
        if (opt.has_value())
        {
            infos.push_back(std::move(*opt));
        }
    }
    return infos;
}

bool suMobileDB::IsValid(const std::string& number) const
{
    if (number.length() != 11)
        return false;
    if (number[0] != '1')
        return false;
    if (number[1] < '3' || number[1] > '9')
        return false;
    for (char c : number)
    {
        if (!std::isdigit(c))
            return false;
    }
    return true;
}