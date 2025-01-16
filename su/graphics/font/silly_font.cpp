/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-04
 * @file: silly_font.cpp
 * @description: silly_font实现
 * @version: v1.0.1 2024-11-04 dou li yang
 */
#include "silly_font.h"
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include <ft2build.h>
silly_font::silly_font()
{
    if (FT_Init_FreeType(&m_ft_library))
    {
        throw std::runtime_error("初始化FT_Library失败");
    }
}
silly_font::~silly_font()
{
    clean();
}
bool silly_font::add(const std::string& name, const std::string& tff_file)
{
    bool status = false;
    FT_Face ft_face;

    FT_Error ft_error;

    if (!tff_file.empty())
    {
        if (!FT_New_Face(m_ft_library, tff_file.c_str(), 0, &ft_face))
        {
            m_name_font[name] = ft_face;
            status = true;
        }
    }
    return status;
}
bool silly_font::remove(const std::string& name)
{
    auto iter = m_name_font.find(name);
    if (iter != m_name_font.end())
    {
        FT_Done_Face(iter->second);
        m_name_font.erase(iter);
        return true;
    }
    return false;
}
void silly_font::clean()
{
    for (auto& [nm, ft] : m_name_font)
    {
        FT_Done_Face(ft);
    }
    m_name_font = {};
    FT_Done_FreeType(m_ft_library);
    m_ft_library = nullptr;
}
FT_Face* silly_font::face(const std::string& name)
{
    FT_Face* ret = nullptr;
    auto iter = m_name_font.find(name);
    if (iter != m_name_font.end())
    {
        ret = &iter->second;
    }

    return ret;
}
