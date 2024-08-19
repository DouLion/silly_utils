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
#include <image/png_utils.h>
#include <image/silly_font.h>
#include <geo/silly_geo.h>
#include <cairo/cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace silly_image;

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
  public:
    silly_cairo() = default;

    bool create(const size_t h, const size_t& w, const int& type = png_type::eptRGBA);
    void clean(silly_color color = {0, 0, 0, 0});
    bool read(const std::string& path);
    bool write(const std::string& path);
    bool decode(const std::string& bin);
    bool decode(const char* data, const size_t size);
    bool encode(std::string& bin);

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
    void set_color(silly_color color);

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
    /// 释放cairo资源
    /// </summary>
    void release();

    size_t width() const;
    size_t height() const;

  private:
    void draw_ring(const silly_ring& ring, const silly_geo_rect& rect);
    void draw_ring_web_mercator(const silly_ring& ring, const silly_geo_rect& rect);

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
