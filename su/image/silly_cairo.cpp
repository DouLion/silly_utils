/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 24-8-9 下午2:32
 * @version: 1.0.1
 * @description: silly_cairo 类实现
 */
#include "silly_cairo.h"
#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include <ft2build.h>
#include "geo/silly_projection.h"
#include FT_FREETYPE_H

const static double COLOR_UNSIGNED_CHAR_DOUBLE = 255.0;

#define PC_CC(a) static_cast<double>(a) / COLOR_UNSIGNED_CHAR_DOUBLE

std::map<std::string, FT_Face> silly_cairo::CAIRO_NAME_FONT = {};

FT_Library silly_cairo::CAIRO_FONT_LIB = nullptr;

bool silly_cairo::m_enable_font = false;

struct st_png_data
{
    char *data;
    uint32_t offset;
    uint32_t length;
};

static cairo_status_t cairo_read_func_mine(void *closure, unsigned char *data, uint32_t length)
{
    st_png_data *pPngData = (st_png_data *)closure;
    if (pPngData->length - pPngData->offset < length)
    {
        length = pPngData->length - pPngData->offset;
    }

    memcpy(data, pPngData->data + pPngData->offset, length);
    pPngData->offset += length;

    return CAIRO_STATUS_SUCCESS;
}

static cairo_surface_t *from_binary_data(const char *data, const int &size)
{
    cairo_surface_t *surface;
    st_png_data pngData = {(char *)data, 0, (uint32_t)size};
    surface = cairo_image_surface_create_from_png_stream(cairo_read_func_mine, &pngData);
    return surface;
}

static cairo_status_t cairo_write_func_mine(void *closure, const unsigned char *data, uint32_t length)
{
    std::string &bin = *((std::string *)closure);
    bin.resize(length);
    memcpy(&bin[0], data, length);
    return CAIRO_STATUS_SUCCESS;
}

bool silly_cairo::create(const size_t ww, const size_t &hh, const int &type)
{

    if (!ww || !hh)
    {
        return false;
    }
    m_height = hh;
    m_width = ww;
    m_format = CAIRO_FORMAT_ARGB32;
    switch (type)
    {
        case png_type::eptRGBA:
            m_format = CAIRO_FORMAT_ARGB32;
            break;
        case png_type::eptRGB:
            m_format = CAIRO_FORMAT_RGB24;
            break;
        case png_type::eptGRAY:
            m_format = CAIRO_FORMAT_A8;
            break;
        default:
            break;
    }
    if (!(m_surface = cairo_image_surface_create((cairo_format_t)m_format, m_width, m_height)))
    {
        return false;
    }
    if (!(m_cr = cairo_create(m_surface)))
    {
        return false;
    }
    return true;
}

bool silly_cairo::read(const std::string &path)
{
    if (!(m_surface = cairo_image_surface_create_from_png(path.c_str())))
    {
        return false;
    }
    m_cr = cairo_create(m_surface);
    m_width = cairo_image_surface_get_width(m_surface);
    m_height = cairo_image_surface_get_height(m_surface);
    return true;
}

bool silly_cairo::write(const std::string &path)
{
    return (CAIRO_STATUS_SUCCESS == cairo_surface_write_to_png(m_surface, path.c_str()));
}

bool silly_cairo::decode(const std::string &bin)
{
    return decode(bin.c_str(), bin.size());
}

bool silly_cairo::decode(const char *data, const size_t size)
{
    m_surface = from_binary_data(data, size);
    if (!m_surface)
    {
        return false;
    }
    m_width = cairo_image_surface_get_width(m_surface);
    m_height = cairo_image_surface_get_height(m_surface);
    m_cr = cairo_create(m_surface);
    if (m_height == 0 || m_width == 0 || !m_cr)
    {
        cairo_surface_destroy(m_surface);
        return false;
    }
    return true;
}

bool silly_cairo::encode(std::string &bin)
{
    cairo_surface_write_to_png_stream(m_surface, cairo_write_func_mine, &bin);
    if (bin.empty())
    {
        return false;
    }
    return true;
}

void silly_cairo::release()
{
    if (m_cr)
    {
        cairo_destroy(m_cr);
    }
    if (m_surface)
    {
        cairo_surface_destroy(m_surface);
    }
}

void silly_cairo::draw_text(silly_cairo_text sct)
{
    if (sct.tff_name.empty())
    {
        cairo_select_font_face(m_cr, sct.font_family.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    }
    else
    {
        //
        cairo_font_face_t *cairo_font_face = nullptr;
        if (CAIRO_NAME_FONT.find(sct.tff_name) != std::end(CAIRO_NAME_FONT))
        {
            cairo_font_face = cairo_ft_font_face_create_for_ft_face(CAIRO_NAME_FONT.at(sct.tff_name), 0);
            cairo_set_font_face(m_cr, cairo_font_face);
        }
    }

    cairo_set_font_size(m_cr, sct.font_size);
    cairo_move_to(m_cr, sct.x, sct.y);  // 高
    cairo_show_text(m_cr, sct.text.c_str());
    cairo_stroke(m_cr);
}

void silly_cairo::set_color(silly_color color)
{
    switch (m_format)
    {
        case CAIRO_FORMAT_ARGB32:
            cairo_set_source_rgba(m_cr, PC_CC(color.red), PC_CC(color.green), PC_CC(color.blue), PC_CC(color.alpha));
            break;
        case CAIRO_FORMAT_RGB24:
            cairo_set_source_rgb(m_cr, PC_CC(color.red), PC_CC(color.green), PC_CC(color.blue));
            break;
        case CAIRO_FORMAT_A8:
            cairo_set_source_rgba(m_cr, PC_CC(color.alpha), PC_CC(color.alpha), PC_CC(color.alpha), PC_CC(color.alpha));
            break;
        default:
            break;
    }
}

void silly_cairo::clean(silly_color color)
{
    set_color(color);
    cairo_paint(m_cr);
}

void silly_cairo::draw_poly(const silly_poly &poly, const silly_geo_rect &rect)
{
    std::unique_lock loc(m_mtx);
    // 画外环
    draw_ring(poly.outer_ring, rect);

    // 画内环
    for (auto &ring : poly.inner_rings)
    {
        draw_ring(ring, rect);
    }
    cairo_set_fill_rule(m_cr, CAIRO_FILL_RULE_EVEN_ODD);
    cairo_fill_preserve(m_cr);
    cairo_stroke(m_cr);
}

void silly_cairo::draw_poly_web_mercator(const silly_poly &poly, const silly_geo_rect &rect)
{
    std::unique_lock loc(m_mtx);
    // 画外环
    draw_ring_web_mercator(poly.outer_ring, rect);

    // 画内环
    for (auto &ring : poly.inner_rings)
    {
        draw_ring_web_mercator(ring, rect);
    }
    cairo_set_fill_rule(m_cr, CAIRO_FILL_RULE_EVEN_ODD);
    cairo_fill_preserve(m_cr);
    cairo_stroke(m_cr);
}

void silly_cairo::draw_ring(const silly_ring &ring, const silly_geo_rect &rect)
{
    cairo_new_sub_path(m_cr);

    double x_pixel_per_degree = m_width / (rect.right - rect.left);
    double y_pixel_per_degree = m_height / (rect.top - rect.bottom);
    cairo_move_to(m_cr, (ring.points[0].lgtd - rect.left) * x_pixel_per_degree, (rect.top - ring.points[0].lttd) * y_pixel_per_degree);
    for (int i = 1; i < ring.points.size(); ++i)
    {
        cairo_line_to(m_cr, (ring.points[i].lgtd - rect.left) * x_pixel_per_degree, (rect.top - ring.points[i].lttd) * y_pixel_per_degree);
    }
    cairo_close_path(m_cr);
}

void silly_cairo::draw_ring_web_mercator(const silly_ring &ring, const silly_geo_rect &rect)
{
    double mc_left, mc_top, mc_right, mc_bottom;  // 多内环的情况这几个变量会重复计算,但是开销很小,可以暂时忽略
    silly_projection::geo_to_mercator(rect.left, rect.top, mc_left, mc_top);
    silly_projection::geo_to_mercator(rect.right, rect.bottom, mc_right, mc_bottom);

    cairo_new_sub_path(m_cr);
    double mcx, mcy;

    double x_pixel_per_degree = m_width / (mc_right - mc_left);
    double y_pixel_per_degree = m_height / (mc_top - mc_bottom);
    silly_projection::geo_to_mercator(ring.points[0].lgtd, ring.points[0].lttd, mcx, mcy);
    cairo_move_to(m_cr, (mcx - mc_left) * x_pixel_per_degree, (mc_top - mcy) * y_pixel_per_degree);
    for (int i = 1; i < ring.points.size(); ++i)
    {
        silly_projection::geo_to_mercator(ring.points[i].lgtd, ring.points[i].lttd, mcx, mcy);
        cairo_line_to(m_cr, (mcx - mc_left) * x_pixel_per_degree, (mc_top - mcy) * y_pixel_per_degree);
    }
    cairo_close_path(m_cr);
}
void silly_cairo::paint(const silly_cairo &other, const double &x, const double &y, const double &alpha)
{
    cairo_set_source_surface(m_cr, other.m_surface, x, y);
    cairo_paint_with_alpha(m_cr, alpha);
}
size_t silly_cairo::width() const
{
    return m_width;
}
size_t silly_cairo::height() const
{
    return m_height;
}
bool silly_cairo::add_font(const std::string &name, const std::string &path)
{
    bool status = false;
    FT_Face ft_face;

    FT_Error ft_error;

    if (!path.empty())
    {
        ft_error = FT_New_Face(CAIRO_FONT_LIB, path.c_str(), 0, &ft_face);
        if (!ft_error)
        {
            CAIRO_NAME_FONT[name] = ft_face;
            status = true;
        }
    }
    return status;
}
void silly_cairo::enable_fonts()
{
    if (!m_enable_font)
    {
        if (FT_Init_FreeType(&CAIRO_FONT_LIB))
        {
        }
    }
}

void silly_cairo::disable_fonts()
{
    for (auto &[nm, ft] : CAIRO_NAME_FONT)
    {
        FT_Done_Face(ft);
    }
    CAIRO_NAME_FONT = {};
    FT_Done_FreeType(CAIRO_FONT_LIB);
    CAIRO_FONT_LIB = nullptr;
    m_enable_font = false;
}
size_t silly_cairo::count_span(const std::string &u8str)
{
    size_t count = 0;
    for (size_t i = 0; i < u8str.size(); ++i)
    {
        // 对于UTF-8编码，中文字符占3或4个字节，其首字节均大于0x80
        if ((unsigned char)u8str[i] >= 0x80)
        {
            // 跳过整个中文字符的字节
            if ((u8str[i] & 0xF0) == 0xF0)
            {
                i += 3;  // 对于4字节的UTF-8字符
            }
            else
            {
                i += 2;  // 对于3字节的UTF-8字符
            }
            ++count;
            ++count;
        }
        else // 如果不是中文则认为是数字或者字母
        {
            ++count;
        }
    }
    return count;
}
void silly_cairo::set_operator(const int &opt)
{
    cairo_set_operator(m_cr, static_cast<cairo_operator_t>(opt));
}
