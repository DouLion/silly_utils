/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 24-8-9 下午2:32
 * @version: 1.0.1
 * @description: silly_cairo 类声明
 */
#ifndef SILLY_UTILS_SILLY_CAIRO_H
#define SILLY_UTILS_SILLY_CAIRO_H

#include <cairo/cairo.h>
#include <geo/silly_geo.h>
#include <cairo/cairo-ft.h>
#include <ft2build.h>
#include <graphics/silly_color.h>
#include <graphics/font/silly_font.h>
#include FT_FREETYPE_H



enum enum_silly_cairo_align
{
    Middle = 1,  // 居中对齐
    LeftTop = 2,
    LeftBottom = 3,
    RightTop = 4,
    RightBottom = 5
};

class silly_cairo_text
{
  public:
    double x{0};
    double y{0};
    std::string text;
    double font_size{10};
    std::string font_family{MicroSoftYaHei};
    std::string tff_name;  // 自定义字体的名称
};

class silly_cairo
{
  /// <summary>
  /// 读写均只支持png 和 jpg 图片格式,其他未作兼容
  /// </summary>
  public:
    silly_cairo() = default;
    /// <summary>
    /// 创建
    /// </summary>
    /// <param name="ww"></param>
    /// <param name="hh"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    bool create(const size_t ww, const size_t& hh, const int& type = silly::color::type::eptRGBA);

    /// <summary>
    /// 使用颜色清空画布
    /// </summary>
    /// <param name="color"></param>
    void clean(silly::color color = {0, 0, 0, 0});

    /// <summary>
    /// 从文件读取图片
    /// </summary>
    /// <param name="path"></param>
    /// <param name="png">true 以png读取, false 以 jpeg 格式读取</param>
    /// <returns></returns>
    bool read(const std::string& path, const bool& png = true);

    /// <summary>
    /// 将图片写入文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="png">true 写为 png, false 写为 jpeg格式</param>
    /// <returns></returns>
    bool write(const std::string& path, const bool& png = true);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="bin"></param>
    /// <returns></returns>
    bool decode(const std::string& bin);
    bool decode(const unsigned char* data, const size_t size);
    bool encode(std::string& bin, const bool& png = true);

    /// <summary>
    /// 启用自定义字体(字体文件不在系统字体中)
    /// </summary>
    static void enable_fonts();

    static void disable_fonts();

    /// <summary>
    /// 添加一个字体
    /// </summary>
    /// <param name="name">自定义字体名称</param>
    /// <param name="path">字体文件(.tff)的路径</param>
    static bool add_font(const std::string& name, const std::string& path);

    /// <summary>
    /// 设置颜色, 自动由 0-255转为 0-1.0
    /// </summary>
    /// <param name="color"></param>
    void set(const silly::color& color);

    /// <summary>
    /// 设置操作
    /// </summary>
    /// <param name="opt"></param>
    void set(const cairo_operator_t& opt);

    /// <summary>
    /// 将另一张图片绘制到当前图片上
    /// </summary>
    /// <param name="other"></param>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <param name="alpha"></param>
    void paint(const silly_cairo& other, const double& x = 0, const double& y = 0, const double& alpha = 1.);

    /// <summary>
    /// 绘制文字
    /// </summary>
    /// <param name="sct"></param>
    void draw_text(silly_cairo_text sct);

    void draw_point(const silly_point& p, const double& size, const silly_geo_rect& rect);

    /// <summary>
    /// 计算字符串占用字符宽度, (约)中文2个宽度,数字字母一个宽度
    /// </summary>
    /// <param name="u8str"></param>
    static size_t count_span(const std::string& u8str);

    /// <summary>
    /// 绘制矢量面, 这个可以扩展出很多用法,
    /// rect用于其他任何形式的等比例缩放
    /// </summary>
    /// <param name="poly"></param>
    /// <param name="rect"></param>
    void draw_poly(const silly_poly& poly, const silly_geo_rect& rect);

    /// <summary>
    /// 绘制矢量面, 将geo坐标转为web mercator坐标后绘制
    /// </summary>
    /// <param name="poly"></param>
    /// <param name="rect"></param>
    void draw_poly_web_mercator(const silly_poly& poly, const silly_geo_rect& rect);

    /// <summary>
    /// 绘制矢量面, 这个可以扩展出很多用法,
    /// rect用于其他任何形式的等比例缩放
    /// </summary>
    /// <param name="poly"></param>
    /// <param name="rect"></param>
    void draw_line(const std::vector<silly_point>& line, const silly_geo_rect& rect);

    /// <summary>
    /// 绘制矢量面, 将geo坐标转为web mercator坐标后绘制
    /// </summary>
    /// <param name="poly"></param>
    /// <param name="rect"></param>
    void draw_line_web_mercator(const std::vector<silly_point>& line, const silly_geo_rect& rect);

    /// <summary>
    /// 释放cairo资源
    /// </summary>
    void release();

    size_t width() const;
    size_t height() const;

  private:
    void draw_ring(const silly_ring& ring, const silly_geo_rect& rect);
    void draw_ring_web_mercator(const silly_ring& ring, const silly_geo_rect& rect);

    // void draw_line(const std::vector<silly_point>& line, const silly_geo_rect& rect);
    // void draw_ring_web_mercator(const std::vector<silly_point>& line, const silly_geo_rect& rect);

  private:
    int m_format{0};
    size_t m_width{0};
    size_t m_height{0};
    cairo_t* m_cr{nullptr};
    cairo_surface_t* m_surface{nullptr};
    std::mutex m_mtx;  // 同时只能做一件事情
    static bool m_enable_font;
    static std::map<std::string, FT_Face> CAIRO_NAME_FONT;
    static FT_Library CAIRO_FONT_LIB;
};

#endif  // SILLY_UTILS_SILLY_CAIRO_H
