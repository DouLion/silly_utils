/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-04
 * @file: silly_path.c
 * @description: silly_path实现
 * @version: v1.0.1 2025-12-04 dou li yang
 */
#include "silly_path.h"
#include <system/silly_system.h>
suPath::suPath(const std::string& path)
{
#if _WIN32
    if (HAS_UTF8(path))
    {
        m_path = std::filesystem::path(UTF8ToGBK(path));
    }
    else
#endif
    {
        m_path = std::filesystem::path(path);
    }
}

#if _WIN32
suPath::suPath(const std::wstring& path)
{
    m_path = std::filesystem::path(path);
}
suPath& suPath::append(const std::wstring& node)
{
    m_path.append(node);
    return *this;
}
std::wstring suPath::name_ws() const
{
    return m_path.filename().wstring();
}
std::wstring suPath::stem_ws() const
{
    return m_path.stem().wstring();
}
std::wstring suPath::ext_ws() const
{
    return m_path.extension().wstring();
}

std::wstring suPath::wstring() const
{
    return m_path.wstring();
}
#endif

suPath& suPath::absolute()
{
    m_path = std::filesystem::absolute(m_path);
    return *this ;
}
suPath& suPath::relative(const suPath& root)
{
    m_path = std::filesystem::relative(m_path, root.m_path);
    return *this ;
}
suPath suPath::parent() const
{
    return suPath(m_path.parent_path());
}

suPath::suPath(const suPath& path)
{
    m_path = path.m_path;
}

suPath::suPath(const std::filesystem::path& path)
{
    m_path = path;
}

suPath suPath::current()
{
    suPath ret;
    ret.m_path = std::filesystem::current_path();
    return ret;
}

void suPath::chdir(const suPath& path)
{
    std::filesystem::current_path(path.m_path);
}

void suPath::mkdir(const suPath& path)
{
    std::filesystem::create_directories(path.m_path);
}

suPath& suPath::append(const std::string& node)
{
#if _WIN32
    if (HAS_UTF8(node))
    {
        m_path.append(UTF8ToGBK(node));
    }
    else
#endif
    {
        m_path.append(node);
    }
    return *this;
}

std::string suPath::name() const
{
    return m_path.filename().string();
}
std::string suPath::name_utf8() const
{
    return m_path.filename().u8string();
}
std::string suPath::stem() const
{
    return m_path.stem().string();
}
std::string suPath::stem_utf8() const
{
    return m_path.stem().u8string();
}
std::string suPath::ext() const
{
    return m_path.extension().string();
}
std::string suPath::ext_utf8() const
{
    return m_path.extension().u8string();
}
void suPath::mkdir() const
{
    std::filesystem::create_directories(m_path);
}
std::string suPath::string() const
{
    return m_path.string();
}
std::string suPath::u8string() const
{
    return m_path.u8string();
}