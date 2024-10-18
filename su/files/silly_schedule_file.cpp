/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-18
 * @file: silly_schedule_file.cpp
 * @description: silly_schedule_file实现
 * @version: v1.0.1 2024-10-18 dou li yang
 */
#include "silly_schedule_file.h"
silly_schedule_file::silly_schedule_file(const std::string& root, const std::string& ext)
{
    m_root = root;
    m_extension = ext;
}
silly_schedule_file silly_schedule_file::copy() const
{
    silly_schedule_file ret;
    ret.m_root = m_root;
    ret.m_extension = m_extension;
    ret.m_format = m_format;

    return ret;
}
void silly_schedule_file::root(const std::string& r)
{
    m_root = r;
}
void silly_schedule_file::extension(const std::string& e)
{
    if(!e.empty() && e[0] != '.')
    {
        m_extension.append(".").append(e);
    }
    else
    {
        m_extension = e;
    }

}
void silly_schedule_file::format(const std::string& r)
{
    m_format = r;
}
std::string silly_schedule_file::filename(const std::string& tm)
{

    return std::string();
}
std::string silly_schedule_file::filename(const silly_posix_time& tm)
{
   m_filename = silly_posix_time::time_to_string(tm, m_format);
   m_filename.append(m_extension);

}
bool silly_schedule_file::create_dir(const std::string& tm)
{
    return false;
}
bool silly_schedule_file::create_dir(const silly_posix_time& tm)
{
    return false;
}
bool silly_schedule_file::exist(const std::string& tm)
{
    return false;
}
bool silly_schedule_file::exist(const silly_posix_time& tm)
{
    return false;
}
std::string silly_schedule_file::path()
{
    return std::string();
}
