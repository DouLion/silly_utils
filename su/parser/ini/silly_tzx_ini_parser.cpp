//
// Created by dell on 2023/12/18.
//

#include "silly_tzx_ini_parser.h"
#include <system/silly_system.h>
bool TzxIni::read()
{
    return false;
}
bool TzxIni::write(const bool& gbk) const
{
    return false;
}
bool TzxIni::set(const std::string& section, const std::string& property, const std::string& value, const std::string& comment)
{
    m_sect2prop2value[section][property] = value;

    /*if (!comment.empty())
    {
        m_sect2prop2comment[section][property] = comment;
    }
    if (MAP_HAS(m_sect2prop2value, section))
    {
        if (MAP_HAS(m_sect2prop2value.at(section), property))
        {
            return true;
        }
    }*/
    return false;
}

bool TzxIni::remove(const std::string& section, const std::string& property)
{
    if (!MAP_HAS(m_sect2prop2value, section))
    {
        return true;
    }
    if (!MAP_HAS(m_sect2prop2value.at(section), property))
    {
        return true;
    }

    return true;
}
double TzxIni::numeric(const std::string& section, const std::string& property) const
{
    double ret = std::numeric_limits<double>::quiet_NaN();
    try
    {
        if (MAP_HAS(m_sect2prop2value, section))
        {
            if (MAP_HAS(m_sect2prop2value.at(section), property))
            {
               ret =  std::stod(m_sect2prop2value.at(section).at(property));
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ret;

}

std::string TzxIni::string(const std::string& section, const std::string& property) const
{

    if (MAP_HAS(m_sect2prop2value, section))
    {
        if (MAP_HAS(m_sect2prop2value.at(section), property))
        {
            return m_sect2prop2value.at(section).at(property);
        }
    }
    return "";
}
