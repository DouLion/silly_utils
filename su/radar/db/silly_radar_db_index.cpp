/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-03
 * @file: silly_radar_db_index.c
 * @description: silly_radar_db_index实现
 * @version: v1.0.1 2025-12-03 dou li yang
 */
#include "silly_radar_db_index.h"

#include "files/silly_file.h"

void suRadarDBIndex::add(const std::string& code)
{
    std::string ncode = TRIM(code);
    if (m_code2sort.find(ncode) == m_code2sort.end())
    {
        m_code2sort.insert({ncode, m_code2sort.size()});
        m_added = true;
    }
}
bool suRadarDBIndex::contains(const std::string& code) const
{
    return m_code2sort.find(code) != m_code2sort.end();
}
size_t suRadarDBIndex::sort(const std::string& code) const
{
    if (m_code2sort.find(code) == m_code2sort.end())
    {
        return SIZE_MAX;
    }
    return m_code2sort.at(code);
}
size_t suRadarDBIndex::size() const
{
    return m_code2sort.size();
}
bool suRadarDBIndex::write(const suPath& file) const
{
    if (m_added)
    {
        std::vector<std::string> lines;
        lines.reserve(m_code2sort.size());
        for (const auto&[code, sort]: m_code2sort)
        {
            lines.push_back(code + "," + std::to_string(sort));
        }

        return sufile::write(file, lines) > 0;
    }
    return true;
}

bool suRadarDBIndex::open(const suPath& file)
{
    m_file = file;
    if (!file.exists())
    {
        // TODO: 这里需要考虑自动初始化的情况

        SLOG_INFO("新索引文件:{}",m_file.u8string());
        return true;
    }
    auto lines = sufile::readlines(file, [&](const std::string& line)->bool {
        std::vector<std::string> parts = SPLIT(line, ',');
        try
        {
            m_code2sort.insert({TRIM(parts[0]), std::stod(parts[1])});
        }
        catch (...)
        {
            // 不用管
        }
        return false;
    });
    if (m_code2sort.empty())
    {
        supath::rmfile(m_file);
    }
    return true;
}