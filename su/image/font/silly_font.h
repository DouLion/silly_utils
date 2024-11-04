/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-04
 * @file: silly_font.h
 * @description: silly_font 类声明
 * @version: v1.0.1 2024-11-04 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_FONT_H
#define SILLY_UTILS_SILLY_FONT_H

#include <su_marco.h>
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

class silly_font
{
  public:
    silly_font();
    ~silly_font();


    bool add(const std::string& name, const std::string& tff_file);
    bool remove(const std::string& name);

    FT_Face* face(const std::string& name);

    void clean();

  private:
    std::map<std::string, FT_Face> m_name_font{};
    FT_Library m_ft_library;
};

#endif  // SILLY_UTILS_SILLY_FONT_H
