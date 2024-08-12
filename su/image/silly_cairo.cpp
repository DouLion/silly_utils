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
#include FT_FREETYPE_H

static std::map<std::string, FT_Face> CAIRO_NAME_FONT{};
static FT_Library CAIRO_FONT_LIB;



struct st_png_data
{
    char* data;
    uint32_t offset;
    uint32_t length;
};
static cairo_status_t cairo_read_func_mine(void* closure, unsigned char* data, uint32_t length)
{
    st_png_data* pPngData = (st_png_data*)closure;
    if (pPngData->length - pPngData->offset < length)
    {
        length = pPngData->length - pPngData->offset;
    }

    memcpy(data, pPngData->data + pPngData->offset, length);
    pPngData->offset += length;

    return CAIRO_STATUS_SUCCESS;
}
static cairo_surface_t* from_binary_data(const char* data, const int& size)
{
    cairo_surface_t* surface;
    st_png_data pngData = {(char*)data, 0, (uint32_t)size};
    surface = cairo_image_surface_create_from_png_stream(cairo_read_func_mine, &pngData);
    return surface;
}

static cairo_status_t cairo_write_func_mine(void* closure, const unsigned char* data, uint32_t length)
{
    std::string& bin = *((std::string*)closure);
    bin.resize(length);
    memcpy(&bin[0], data, length);
    return CAIRO_STATUS_SUCCESS;
}

bool silly_cairo::create(const size_t h, const size_t& w, const int& type)
{
    if (!h || !w)
    {
        return false;
    }
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
    if (!(m_surface = cairo_image_surface_create((cairo_format_t)m_format, w, h)))
    {
        return false;
    }
    if (!(m_cr = cairo_create(m_surface)))
    {
        return false;
    }
    return true;
}
bool silly_cairo::read(const std::string& path)
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
bool silly_cairo::write(const std::string& path)
{
    return (CAIRO_STATUS_SUCCESS == cairo_surface_write_to_png(m_surface, path.c_str()));
}
bool silly_cairo::decode(const std::string& bin)
{
    return decode(bin.c_str(), bin.size());
}
bool silly_cairo::decode(const char* data, const size_t size)
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
bool silly_cairo::encode(std::string& bin)
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

void silly_cairo::draw(silly_cairo_text sct)
{
    /*auto iter = CAIRO_NAME_FONT.find(sct.font_family);
    if( != std::end(m))*/
    cairo_set_source_rgba (m_cr, 1, 0.2, 0.2, 0.6);
    cairo_select_font_face(m_cr, sct.font_family.c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(m_cr, sct.font_size);
    cairo_move_to(m_cr, sct.x, sct.y);  // 高
    cairo_show_text(m_cr, sct.text.c_str());
    cairo_stroke(m_cr);
}
void silly_cairo::set_color(silly_color color)
{
    const double icc = 255.0;
    switch (m_format)
    {
        case CAIRO_FORMAT_ARGB32:
            cairo_set_source_rgba(m_cr, static_cast<double>(color.red) / icc, static_cast<double>(color.green) / icc, static_cast<double>(color.blue) / icc, static_cast<double>(color.alpha) / icc);
            break;
        case CAIRO_FORMAT_RGB24:
            cairo_set_source_rgb(m_cr, static_cast<double>(color.red) / icc, static_cast<double>(color.green) / icc, static_cast<double>(color.blue) / icc);
            break;
        case CAIRO_FORMAT_A8:
            cairo_set_source_rgba(m_cr, static_cast<double>(color.gray) / icc, static_cast<double>(color.gray) / icc, static_cast<double>(color.gray) / icc, static_cast<double>(color.alpha) / icc);
            break;
        default:
            break;
        
    }
}
void silly_cairo::clean(silly_color color)
{
    set_color(color);
    cairo_rectangle(m_cr, 0, 0, m_width, m_height);
    cairo_fill(m_cr);
}
