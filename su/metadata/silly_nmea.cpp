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
silly_nmea::silly_nmea()
{
}
silly_nmea::~silly_nmea()
{
}
bool silly_nmea::open(const std::string& file)
{
    m_path = file;
    return true;
}
std::vector<std::string> silly_nmea::read(const std::string& type, const size_t& offset, const size_t& size)
{
    std::vector<std::string> ret;
    int _offset = 0, _size = 0;
    std::ifstream _file(m_path);
    if(!_file.is_open())
    {
        return ret;
    }
    std::string line;
    while(std::getline(_file, line))
    {
        if(match(line, type))
        {
            _offset++;
            if(_offset>=offset)
            {
                _size++;
                if(_size>size)
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
bool silly_nmea::close()
{
    return false;
}
bool silly_nmea::match(const std::string& str, const std::string& base)
{
    if(str.size()< base.size())
    {
        return false;
    }
    for(size_t i=0;i<base.size();i++)
    {
        if(str[i]!=base[i])
        {
            return false;
        }
    }
    return true;
}
silly_nmea::silly_nmea(const std::string& file)
{
    open(file);
}
