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
        while (!iss.eof())
        {
            std::string code;
            size_t index;
            iss >> code >> index;

            if(index > max_index)
            {
                max_index = index;
            }
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
            //std::cout << "code:" << k << " index:" << v << std::endl;
            ss << " " << k << " " << v << std::endl;
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
    insert(code, index);
    return true;
}
bool dynamic_rule_code_index::remove(const std::string& code)
{
    std::lock_guard<std::mutex> lock(m_mutex);
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
bool dynamic_rule_code_index::add(const std::string& code)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (find(code) == end())
    {
        size_t tmp = ++max_index;
        return add(code, tmp);
    }
    return false;
}
std::string dynamic_rule_record::serialize() const
{
    std::vector<uint8_t> buff(10240);
    short total = 0;
    uint8_t fmt = (0b111 << 5) | static_cast<uint8_t>(code.size());
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
    short total = 0;
    std::vector<uint8_t> buff(10240);
    uint8_t fmt = DRFMT_CODE_INDEX;

    buff[total] = fmt;
    total++;
    memcpy(buff.data() + total, &code_index, sizeof(code_index));
    total += sizeof(code_index);
    memcpy(buff.data() + total, &moisture_percent, sizeof(moisture_percent));
    total += sizeof(moisture_percent);
    uint8_t len = 0;
    for (const auto& [intv, grade_th] : intv_grade_threshold)
    {
        for (const auto& [grade, th] : grade_th)
        {
            len++;
        }
    }
    memcpy(buff.data() + total, &len, sizeof(len));
    total+=sizeof(len);
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
    memcpy(&result[0], &buff[0], total);

    return result;
}
bool dynamic_rule_record::deserialize(const std::string& data)
{
    if (data.empty())
        return false;

    char* p = (char*)data.data();
    uint8_t fmt = static_cast<uint8_t>(p[0]);
    p++;
    if (fmt > DRFMT_CODE_LENGTH)
    {
        uint8_t len = static_cast<uint8_t>(p[0]) & 0b00011111;
        code = data.substr(1, len);
        p += len;
    }
    else if (fmt == DRFMT_CODE_INDEX)
    {
        index = ((int32_t*)p)[0];
        p += sizeof(int32_t);
    }
    moisture_percent = *(float*)(p);
    p += sizeof(moisture_percent);
    uint8_t len = static_cast<uint8_t>(p[0]);
    if(len > 100)
    {
        SLOG_WARN("数据可能有误,引起程序异常");
        return false;
    }
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
    /*p++;
    int total = int(*p);
    p+=sizeof (total);*/
    while (*p == 0x5F)
    {
        p++;
        int len = int(*p);
        if (len == 0)
        {
            // 最后一个完成
            return true;
        }
        p+=sizeof(len);

        std::string tmp;
        tmp.resize(len);
        memcpy(tmp.data(), p, len);
        p +=len;
        dynamic_rule_record record;
        record.deserialize(tmp);
        if(m_index.contains_value(record.index))
        {
            record.code = m_index.code(record.index);
            records[record.code] = record;
        }

    }
    return false;
}

bool silly_dynamic_rule::write_with_code_index(const std::string& path, const std::map<std::string, dynamic_rule_record>& records) const
{
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::map<std::string, int> index_map;
    std::map<int, std::string> code_map;
    for (const auto& [code, record] : records)
    {
        if (!m_index.contains(code))
        {
            std::cerr << "Invalid code: " << code << std::endl;
            return false;
        }
        index_map[code] = record.index;

    }
    std::string end_flag = std::string(5, 0x5f);
    end_flag[1] = 0;
    end_flag[2] = 0;
    end_flag[3] = 0;
    end_flag[4] = 0;
    std::string content;
    int total = 0;
    for (const auto& [code, record] : records)
    {
        size_t index;
        if (!m_index.get_value(code, index))
        {
            continue;
        }
        code_map[index] = code;
        std::string data = record.serialize(index);
        int len = data.size();

        total += (len + 5);
        std::string row(5, 0x5f);
        memcpy(row.data() + 1, &len, sizeof(len));
        row.append(data);
        content.append(row);
        // break;
    }

    content.append(end_flag);
    total += 5;

    /*int  fff = content.size();
    for(int i =0; i < total; i++)
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>((unsigned char)(content.data()[i]))<< " ";
    }*/
    silly_file::write(path, content);
    return true;
}
void silly_dynamic_rule::add_code_index(const std::string& code,  size_t& index)
{
    if(m_index.add(code))
    {
        index = m_index.index(code);
    }
}
void silly_dynamic_rule::set_index(const dynamic_rule_code_index& code_index)
{
    m_index.clear();
    for(auto [code, idx] : code_index)
    {
        m_index.add(code, idx);
    }
}
bool silly_dynamic_rule::read_with_code_index(const std::string& path, const std::string& code, dynamic_rule_record& record) const
{
    auto iter = m_index.find(code);
    if(iter == m_index.end())
    {
        return false;
    }
    std::string content;
    if (0 == silly_file::read(path, content))
    {
        return false;
    }
    uint8_t* p = (uint8_t*)content.data();
    /*p++;
    int total = int(*p);
    p+=sizeof (total);*/
    while (*p == 0x5F)
    {
        p++;
        int len = int(*p);
        if (len == 0)
        {
            // 最后一个完成
            return true;
        }
        p+=sizeof(len);
        uint8_t* n = p;
        n++;
        int32_t index = ((int32_t*)n)[0];
        if(index  == iter->second)
        {
            std::string tmp;
            tmp.resize(len);
            memcpy(tmp.data(), p, len);
            record.deserialize(tmp);
            return true;
        }

        p +=len;

    }
    return false;
}
