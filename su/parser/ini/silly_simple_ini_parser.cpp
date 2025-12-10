//
// Created by dell on 2023/12/18.
//

#include "silly_simple_ini_parser.h"

bool suSimpleIni::read()
{
    if (m_simple_ini.LoadFile(m_file.string().c_str()) == SI_OK)
    {
        return true;
    }
    return false;
}

bool suSimpleIni::write() const
{
    std::unique_lock<std::mutex> lock(m_write_mtx);
    bool status = false;
    if (m_simple_ini.SaveFile(m_file.string().c_str(), false) == SI_OK)
    {
        status = true;
    }
    return status;
}

bool suSimpleIni::set(const std::string& section, const std::string& property, const std::string& value, const std::string& comment)
{
    std::unique_lock<std::mutex> lock(m_write_mtx);
    bool status = false;
    if (!comment.empty())
    {
        // 添加注释尽在从新创建是可添加,如果有注释就先删除然后再新建
        const char* c1 = m_simple_ini.GetValue(section.c_str(), property.c_str());
        int is_del = m_simple_ini.DeleteValue(section.c_str(), property.c_str(), c1);
    }
    int is_set;
    if (comment.empty())
    {
        is_set = m_simple_ini.SetValue(section.c_str(), property.c_str(), value.c_str());
    }
    else
    {
        is_set = m_simple_ini.SetValue(section.c_str(), property.c_str(), value.c_str(), comment.c_str());
    }

    if (is_set == SI_UPDATED || is_set == SI_INSERTED)  // 更新成或插入成功
    {
        status = true;
    }
    return status;
}

int suSimpleIni::get_int(const std::string& section, const std::string& property) const
{
    std::unique_lock<std::mutex> lock(m_write_mtx);
    int value = static_cast<int>(m_simple_ini.GetLongValue(section.c_str(), property.c_str()));
    return value;
}

bool suSimpleIni::get_bool(const std::string& section, const std::string& property) const
{
    std::unique_lock<std::mutex> lock(m_write_mtx);
    bool value = m_simple_ini.GetBoolValue(section.c_str(), property.c_str());
    return value;
}

float suSimpleIni::get_float(const std::string& section, const std::string& property) const
{
    std::unique_lock<std::mutex> lock(m_write_mtx);
    float value = static_cast<float>(m_simple_ini.GetDoubleValue(section.c_str(), property.c_str()));
    return value;
}

double suSimpleIni::get_double(const std::string& section, const std::string& property) const
{
    std::unique_lock<std::mutex> lock(m_write_mtx);
    double value = m_simple_ini.GetDoubleValue(section.c_str(), property.c_str());
    return value;
}

long suSimpleIni::get_long(const std::string& section, const std::string& property) const
{
    std::unique_lock<std::mutex> lock(m_write_mtx);
    long value = m_simple_ini.GetLongValue(section.c_str(), property.c_str());
    return value;
}

std::string suSimpleIni::get(const std::string& section, const std::string& property) const
{
    std::unique_lock<std::mutex> lock(m_write_mtx);
    std::string res;
    const char* value = m_simple_ini.GetValue(section.c_str(), property.c_str());
    if (value)
    {
        res = value;
    }
    return res;
}
