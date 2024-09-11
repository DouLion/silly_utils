//
// Created by dell on 2023/12/18.
//

#include "silly_boost_ini_parser.h"
#ifdef SILLY_UTILS_BOOST_ENABLE
#include "boost/property_tree/ini_parser.hpp"
#include "su_marco.h"
#include <boost/algorithm/string.hpp>

bool silly_boost_ini_parser::load(const std::string& path)
{
    bool status = false;
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        m_path = path;
        boost::property_tree::ini_parser::read_ini(m_path, m_tree);
        status = true;
    }
    catch (const boost::property_tree::ini_parser_error& e)
    {
        SU_ERROR_PRINT("Error:%s \n ", e.what());
        status = false;
    }
    return status;
}

bool silly_boost_ini_parser::save()
{
    bool status = false;
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        boost::property_tree::ini_parser::write_ini(m_path, m_tree);
        status = true;
    }
    catch (const boost::property_tree::ini_parser_error& e)
    {
        SU_ERROR_PRINT("Error:%s \n ", e.what());
        status = false;
    }
    return status;
}

bool silly_boost_ini_parser::write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment)
{
    bool status = false;
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        auto sectionIt = m_tree.find(section);
        if (sectionIt != m_tree.not_found())
        {
            auto& sectionNode = sectionIt->second;
            auto propertyIt = sectionNode.find(property);
            if (propertyIt != sectionNode.not_found())
            {
                // 修改属性
                propertyIt->second.data() = value;
            }
            else
            {
                // 添加属性
                sectionNode.put(property, value);
            }
        }
        else
        {
            // 添加节点,并添加节点,支持节点名称中有.
            boost::property_tree::ptree newNode;
            std::pair<std::string, boost::property_tree::ptree> newPair;
            newPair.first = section;
            newNode.put(property, value);
            newPair.second = newNode;
            m_tree.push_back(newPair);
        }
        status = true;
    }
    catch (const boost::property_tree::ini_parser_error& e)
    {
        SU_ERROR_PRINT("Error:%s \n ", e.what());
        status = false;
    }
    return status;
}

int silly_boost_ini_parser::read_int(const std::string& section, const std::string& property)
{
    int res = 0;
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        boost::optional<int> value;
        auto sectionIt = m_tree.find(section);
        if (sectionIt != m_tree.not_found())
        {
            auto& sectionNode = sectionIt->second;
            auto propertyIt = sectionNode.find(property);
            if (propertyIt != sectionNode.not_found())
            {
                value = propertyIt->second.get_value_optional<int>();
            }
        }
        if (value)
        {
            res = value.value();
        }
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
        SU_ERROR_PRINT("Error: %s\n", e.what());
    }
    return res;  // 返回默认值
}

bool silly_boost_ini_parser::read_bool(const std::string& section, const std::string& property)
{
    bool res = false;
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        boost::optional<bool> value;
        auto sectionIt = m_tree.find(section);
        if (sectionIt != m_tree.not_found())
        {
            auto& sectionNode = sectionIt->second;
            auto propertyIt = sectionNode.find(property);
            if (propertyIt != sectionNode.not_found())
            {
                value = propertyIt->second.get_value_optional<bool>();
            }
        }
        if (value)
        {
            res = value.value();
        }
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
        SU_ERROR_PRINT("Error: %s\n", e.what());
    }
    return res;  // 返回默认值
}

float silly_boost_ini_parser::read_float(const std::string& section, const std::string& property)
{
    float res = 0.0;
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        boost::optional<float> value;
        auto sectionIt = m_tree.find(section);
        if (sectionIt != m_tree.not_found())
        {
            auto& sectionNode = sectionIt->second;
            auto propertyIt = sectionNode.find(property);
            if (propertyIt != sectionNode.not_found())
            {
                value = propertyIt->second.get_value_optional<float>();
            }
        }
        if (value)
        {
            res = value.value();
        }
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
        SU_ERROR_PRINT("Error: %s\n", e.what());
    }
    return res;
}

double silly_boost_ini_parser::read_double(const std::string& section, const std::string& property)
{
    double res = 0.0;
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        boost::optional<double> value;
        auto sectionIt = m_tree.find(section);
        if (sectionIt != m_tree.not_found())
        {
            auto& sectionNode = sectionIt->second;
            auto propertyIt = sectionNode.find(property);
            if (propertyIt != sectionNode.not_found())
            {
                value = propertyIt->second.get_value_optional<double>();
            }
        }
        if (value)
        {
            res = value.value();
        }
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
        SU_ERROR_PRINT("Error: %s\n", e.what());
    }
    return res;
}

long silly_boost_ini_parser::read_long(const std::string& section, const std::string& property)
{
    long res = 0;
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        boost::optional<long> value;
        auto sectionIt = m_tree.find(section);
        if (sectionIt != m_tree.not_found())
        {
            auto& sectionNode = sectionIt->second;
            auto propertyIt = sectionNode.find(property);
            if (propertyIt != sectionNode.not_found())
            {
                value = propertyIt->second.get_value_optional<long>();
            }
        }
        if (value)
        {
            res = value.value();
        }
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
        SU_ERROR_PRINT("Error: %s\n", e.what());
    }
    return res;
}

std::string silly_boost_ini_parser::read(const std::string& section, const std::string& property)
{
    std::string res = "";
    try
    {
        std::unique_lock<std::mutex> lock(m_write_mtx);
        boost::optional<std::string> value;
        auto sectionIt = m_tree.find(section);
        if (sectionIt != m_tree.not_found())
        {
            auto& sectionNode = sectionIt->second;
            auto propertyIt = sectionNode.find(property);
            if (propertyIt != sectionNode.not_found())
            {
                value = propertyIt->second.get_value_optional<std::string>();
            }
        }

        if (value)
        {
            res = value.value();
        }
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
        SU_ERROR_PRINT("Error: %s\n", e.what());
    }
    return res;
}

#endif
