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

using namespace silly_image;

class silly_cairo_text
{
  public:
    double x{0};
    double y{0};
    std::string text;
    double font_size{10};
    std::string font_family{MicroSoftYaHei};

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
    // bool encode(unsigned char** data, size_t& size);
    void set_color(silly_color color);

    void paint(const silly_cairo& other, const double& x = 0, const double& y = 0, const double& alpha = 1.);
    void draw(silly_cairo_text sct);

    void draw_poly(const silly_poly& poly, const silly_geo_rect& rect);

    void draw_poly_web_mercator(const silly_poly& poly, const silly_geo_rect& rect);

    void release();

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
};

#endif  // SILLY_UTILS_SILLY_CAIRO_H
