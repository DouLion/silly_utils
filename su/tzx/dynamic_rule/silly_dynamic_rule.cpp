/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_dynamic_rule.cpp
 * @description: silly_dynamic_rule实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_dynamic_rule.h"
#include "files/silly_file.h"
bool dynamic_rule_code_index::load(const std::string& path)
{
    try
    {
        std::string content;
        if (0 == silly_file::read(path, content))
        {
            return false;
        }
        std::istringstream iss(content);
        while (iss.eof())
        {
            std::string code;
            size_t index;
            iss >> code >> index;
            add(code, index);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}
bool dynamic_rule_code_index::save(const std::string& path)
{
    try
    {
        std::stringstream ss;
        for (const auto [k, v] : *this)
        {
            ss << " " << k << "" << v << std::endl;
        }
        silly_file::write(path, ss.str());
    }
    catch (...)
    {
        return false;
    }

    return true;
}
bool dynamic_rule_code_index::add(const std::string& code, const size_t index)
{
    if (code.empty())
        return false;
    if (find(code) == end())
    {
        insert(code, index);
        return true;
    }
    return false;
}
bool dynamic_rule_code_index::remove(const std::string& code)
{
    if (find(code) != end())
    {
        erase(code);
        return true;
    }
    return false;
}

size_t dynamic_rule_code_index::index(const std::string& code) const
{
    auto iterator = find(code);
    if (iterator != end())
        return iterator->second;
    return 0;
}
std::string dynamic_rule_code_index::code(const size_t index) const
{
    const auto iterator = find_value(index);

    if (iterator != end_value())
        return iterator->second;
    return "";
}
std::string dynamic_rule_record::serialize() const
{
    uint8_t fmt = (0b111 << 5) | static_cast<uint8_t>(code.size());
    int total = 0;
    std::vector<uint8_t> buff(10240);
    buff[total] = fmt;
    total++;
    memcpy(buff.data() + total, code.data(), code.size());
    total += code.size();
    memcpy(buff.data() + total, &moisture_percent, sizeof(moisture_percent));
    total += sizeof(moisture_percent);
    uint8_t len = static_cast<uint8_t>(intv_grade_threshold.size());
    memcpy(buff.data() + total, &len, sizeof(len));
    total++;
    for (const auto& [intv, grade_th] : intv_grade_threshold)
    {
        for (const auto& [grade, th] : grade_th)
        {
            memcpy(buff.data() + total, &intv, sizeof(intv));
            total += sizeof(intv);
            memcpy(buff.data() + total, &grade, sizeof(grade));
            total += sizeof(grade);
            memcpy(buff.data() + total, &th, sizeof(th));
            total += sizeof(th);
        }
    }
    std::string result;
    result.resize(total);
    memcpy(result.data(), buff.data(), total);

    return result;
}
std::string dynamic_rule_record::serialize(const int& code_index) const
{
    uint8_t fmt = DRFMT_CODE_INDEX;
    int total = 0;
    std::vector<uint8_t> buff(10240);
    buff[total] = fmt;
    total++;
    memcpy(buff.data() + total, &code_index, sizeof(code_index));
    total += sizeof(code_index);
    memcpy(buff.data() + total, &moisture_percent, sizeof(moisture_percent));
    total += sizeof(moisture_percent);
    uint8_t len = static_cast<uint8_t>(intv_grade_threshold.size());
    memcpy(buff.data() + total, &len, sizeof(len));
    total++;
    for (const auto& [intv, grade_th] : intv_grade_threshold)
    {
        for (const auto& [grade, th] : grade_th)
        {
            memcpy(buff.data() + total, &intv, sizeof(intv));
            total += sizeof(intv);
            memcpy(buff.data() + total, &grade, sizeof(grade));
            total += sizeof(grade);
            memcpy(buff.data() + total, &th, sizeof(th));
        }
    }

    std::string result;
    result.resize(total);
    memcpy(result.data(), buff.data(), total);

    return result;
}
bool dynamic_rule_record::deserialize(const std::string& data)
{
    if (data.empty())
        return false;
    char* p = (char*)data.data();
    uint8_t fmt = static_cast<uint8_t>(p[0]);
    p++;
    if (fmt > DRFMT_CODE_LENLEN)
    {
        uint8_t len = static_cast<uint8_t>(p[0]) & 0b00011111;
        code = data.substr(1, len);
        p += len;
    }
    else if (fmt == DRFMT_CODE_INDEX)
    {
        index = *(int*)(p + 1);
        p += sizeof(int);
    }
    moisture_percent = *(float*)(p);
    p += sizeof(moisture_percent);
    uint8_t len = static_cast<uint8_t>(p[0]);
    p += sizeof(len);
    for (uint8_t i = 0; i < len; i++)
    {
        uint8_t intv = static_cast<uint8_t>(p[0]);
        p++;
        uint8_t grade = static_cast<uint8_t>(p[0]);
        p++;
        float threshold = *(float*)(p);
        p += sizeof(threshold);
        intv_grade_threshold[intv][grade] = threshold;
    }

    return true;
}
silly_dynamic_rule::silly_dynamic_rule()
{
    m_index.clear();
}
silly_dynamic_rule::~silly_dynamic_rule()
{
}
bool silly_dynamic_rule::load_code_index(const std::string& path)
{
    return m_index.load(path);
}
bool silly_dynamic_rule::save_code_index(const std::string& path)
{
    return m_index.save(path);
}
bool silly_dynamic_rule::read_with_code_index(const std::string& path, std::map<std::string, dynamic_rule_record>& records) const
{
    std::string content;
    if (0 == silly_file::read(path, content))
    {
        return false;
    }
    uint8_t* p = (uint8_t*)content.data();
    while (*p == 0x5F)
    {
        p++;
        int len = *(int*)(p);
        if (len == 0)
        {
            // 最后一个完成
            return true;
        }

        std::string tmp;
        tmp.resize(len);
        memcpy(tmp.data(), p, len);
        p += sizeof(len) + len;
        dynamic_rule_record record;
        record.deserialize(tmp);
        // record.code = m_index
        records[record.code] = record;
    }
    return false;
}

bool silly_dynamic_rule::write_with_code_index(const std::string& path, const std::map<std::string, dynamic_rule_record>& records) const
{
    for (const auto& [code, record] : records)
    {
        if (!m_index.contains(code))
        {
            std::cerr << "Invalid code: " << code << std::endl;
            return false;
        }
    }
    std::string content;
    int total = 0;
    for (const auto& [code, record] : records)
    {
        size_t index;
        if (!m_index.get_value(code, index))
        {
            continue;
        }
        std::string data = record.serialize(index);
        int len = data.size();
        char buff[5] = {0x5F, 0x00, 0x00, 0x00, 0x00};
        memcpy(buff + 1, &len, sizeof(len));
        total += (len + 5);
        std::string row(buff);
        row.append(data);
        content.append(row);
    }

    content.append(std::string({0x5F, 0x00, 0x00, 0x00, 0x00}));
    total += 5;
    std::ofstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing." << std::endl;
        return false;
    }
    file.write(content.c_str(), total);
    // 关闭文件
    file.close();
    return false;
}
void silly_dynamic_rule::add_code_index(const std::string& code, const size_t index)
{
    m_index.add(code, index);
}
