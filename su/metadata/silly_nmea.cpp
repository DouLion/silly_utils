/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-13
 * @file: silly_nmea.cpp
 * @description: silly_nmea实现
 * @version: v1.0.1 2024-12-13 dou li yang
 */
#include "silly_nmea.h"

using namespace silly::meta;
nmea::nmea()
{
}

nmea::~nmea()
{
}

nmea::nmea(const std::string& file)
{
    open(file);
}

int64_t nmea ::seconds(const std::string& time)
{
    int64_t val = std::stod(time);
    int hours = (val / 10000);
    val = val % 10000;
    int minutes = (val / 100);
    val = val % 100;
    double seconds = (val % 100);

    return hours * 3600 + minutes * 60 + seconds;
}

double nmea::degree(const std::string& degree)
{
    double val = std::stod(degree);
    int64_t d = std::floor(val / 100);
    return d + (val - d * 100) / 60;
}

bool nmea::open(const std::string& file)
{
    m_path = file;
    return true;
}

std::vector<std::string> nmea::read(const std::string& type, const size_t& offset, const size_t& size)
{
    std::vector<std::string> ret;
    int _offset = 0, _size = 0;
    std::ifstream _file(m_path);
    if (!_file.is_open())
    {
        return ret;
    }
    std::string line;
    while (std::getline(_file, line))
    {
        if (match(line, type))
        {
            _offset++;
            if (_offset >= offset)
            {
                _size++;
                if (_size > size)
                {
                    return ret;
                }
                else
                {
                    ret.push_back(line);
                }
            }
        }
    }
    return ret;
}

bool nmea::close()
{
    return false;
}

bool nmea::match(const std::string& str, const std::string& base)
{
    if (str.size() < base.size())
    {
        return false;
    }
    for (size_t i = 0; i < base.size(); i++)
    {
        if (str[i] != base[i])
        {
            return false;
        }
    }
    return true;
}
