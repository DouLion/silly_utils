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

#if IS_WIN32
// 宋体
#define SimSun "SimSun"
// 微软雅黑常规
#define MicroSoftYaHei "Microsoft YaHei"
// 楷体
#define KaiTi "KaiTi"
// 隶书
#define LiSu "LiSu"
#elif IS_MACOS

#define SimSun "SimSun"
#define MicroSoftYaHei "Microsoft YaHei"
#define KaiTi "KaiTi"
#define LiSu "LiSu"

#else

#define SimSun "SimSun"
#define MicroSoftYaHei "Microsoft YaHei"
#define KaiTi "KaiTi"
#define LiSu "LiSu"

#endif

class silly_font
{
  public:
    silly_font();
    ~silly_font();

    /// <summary>
    /// 向字体集合中添加字体
    /// </summary>
    /// <param name="name">字体的名称</param>
    /// <param name="tff_file">字体文件的路径</param>
    /// <returns></returns>
    bool add(const std::string& name, const std::string& tff_file);

    /// <summary>
    /// 从字体集合中删除一个字体
    /// </summary>
    /// <param name="name">删除的字体的名称</param>
    /// <returns></returns>
    bool remove(const std::string& name);

    /// <summary>
    /// 根据名称返回字体对象
    /// </summary>
    /// <param name="name"></param>
    /// <returns></returns>
    FT_Face* face(const std::string& name);

    /// <summary>
    /// 清空字体集合
    /// </summary>
    void clean();

  private:
    std::map<std::string, FT_Face> m_name_font{};
    FT_Library m_ft_library;
};

#endif  // SILLY_UTILS_SILLY_FONT_H
