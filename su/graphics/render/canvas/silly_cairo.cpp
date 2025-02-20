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
#include <graphics/silly_png.h>
#include <graphics/silly_jpeg.h>

#if ENABLE_JPEG
// 安装的是 libjpeg-turbo
#include "jpeglib.h"
#include "jerror.h"
#include <setjmp.h>
#endif

#include FT_FREETYPE_H

const static double COLOR_UNSIGNED_CHAR_DOUBLE = 255.0;

#define DESTROY_SURFACE(surf)        \
    if (surf)                        \
    {                                \
        cairo_surface_destroy(surf); \
        surf = nullptr;              \
    }

#define DESTROY_CONTEXT(ctx) \
    if (ctx)                 \
    {                        \
        cairo_destroy(ctx);  \
        ctx = nullptr;       \
    }

#define CLR_CVT(a) static_cast<double>(a) / COLOR_UNSIGNED_CHAR_DOUBLE

// 输出jpeg的压缩质量
#define JPEG_QUALITY (90)

std::map<std::string, FT_Face> silly_cairo::CAIRO_NAME_FONT = {};

FT_Library silly_cairo::CAIRO_FONT_LIB = nullptr;

bool silly_cairo::m_enable_font = false;
namespace silly
{

namespace cairo
{
namespace png
{
struct st_data
{
    char *data = nullptr;
    uint32_t offset = 0;
    uint32_t length = 0;
};

static cairo_status_t _read_func(void *closure, unsigned char *data, uint32_t length)
{
    st_data *_data = (st_data *)closure;
    if (_data->length - _data->offset < length)
    {
        length = _data->length - _data->offset;
    }

    memcpy(data, _data->data + _data->offset, length);
    _data->offset += length;

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t _write_func(void *closure, const unsigned char *data, uint32_t length)
{
    std::string &bin = *((std::string *)closure);
    bin.resize(length);
    memcpy(&bin[0], data, length);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_surface_t *surface_create_from_stream(const unsigned char *data, const int &size)
{
    cairo_surface_t *surface;
    st_data _data = {(char *)data, 0, (uint32_t)size};
    surface = cairo_image_surface_create_from_png_stream(_read_func, &_data);
    return surface;
}

static cairo_surface_t *surface_create_from_file(const char *filename)
{
    return cairo_image_surface_create_from_png(filename);
}

static cairo_status_t surface_to_stream(cairo_surface_t *sfc, std::string &bin)
{
    return cairo_surface_write_to_png_stream(sfc, _write_func, (void *)bin.data());
}

static cairo_status_t surface_write(cairo_surface_t *sfc, const char *filename)
{
    return cairo_surface_write_to_png(sfc, filename);
}

}  // namespace png
#if ENABLE_JPEG
namespace jpeg
{
/// https://github.com/rahra/cairo_jpg/blob/master/src/cairo_jpg.c
using ssize_t = unsigned int;
#define CAIRO_JPEG_IO_BLOCK_SIZE 1

static cairo_status_t _write_func(void *closure, const unsigned char *data, unsigned int length)
{
    return ::write((intptr_t)closure, data, (size_t)length) < (ssize_t)length ? CAIRO_STATUS_WRITE_ERROR : CAIRO_STATUS_SUCCESS;
}

///////////////////////////   下面是 写回 JPEG 图片的代码   ///////////////////////////

/// <summary>
/// !!!!!! 这个是实现核心 !!!!!!
/// 将surface 转换成 jpeg内存数据
/// </summary>
/// <param name="sfc"></param>
/// <param name="data"></param>
/// <param name="len"></param>
/// <param name="quality"></param>
/// <returns></returns>

static cairo_status_t _surface_to_stream(cairo_surface_t *sfc, unsigned char **data, size_t *len)  // unsigned char **data, size_t *len)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    cairo_surface_t *other = NULL;

    // check valid input format (must be IMAGE_SURFACE && (ARGB32 || RGB24))
    if (cairo_surface_get_type(sfc) != CAIRO_SURFACE_TYPE_IMAGE || (cairo_image_surface_get_format(sfc) != CAIRO_FORMAT_ARGB32 && cairo_image_surface_get_format(sfc) != CAIRO_FORMAT_RGB24))
    {
        // create a similar surface with a proper format if supplied input format
        // does not fulfill the requirements
        double x1, y1, x2, y2;
        other = sfc;
        cairo_t *ctx = cairo_create(other);
        // get extents of original surface
        cairo_clip_extents(ctx, &x1, &y1, &x2, &y2);
        cairo_destroy(ctx);

        // create new image surface
        sfc = cairo_surface_create_similar_image(other, CAIRO_FORMAT_RGB24, x2 - x1, y2 - y1);
        if (cairo_surface_status(sfc) != CAIRO_STATUS_SUCCESS)
            return CAIRO_STATUS_INVALID_FORMAT;

        // paint original surface to new surface
        ctx = cairo_create(sfc);
        cairo_set_source_surface(ctx, other, 0, 0);
        cairo_paint(ctx);
        cairo_destroy(ctx);
    }

    // finish queued drawing operations
    cairo_surface_flush(sfc);

    // init jpeg compression structures
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // set compression parameters
    unsigned long jpeg_len = *len;
    jpeg_mem_dest(&cinfo, data, &jpeg_len);
    cinfo.image_width = cairo_image_surface_get_width(sfc);
    cinfo.image_height = cairo_image_surface_get_height(sfc);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    // cinfo.in_color_space = JCS_EXT_BGRX;
    cinfo.in_color_space = cairo_image_surface_get_format(sfc) == CAIRO_FORMAT_ARGB32 ? JCS_EXT_BGRA : JCS_EXT_BGRX;
#else
    // cinfo.in_color_space = JCS_EXT_XRGB;
    cinfo.in_color_space = cairo_image_surface_get_format(sfc) == CAIRO_FORMAT_ARGB32 ? JCS_EXT_ARGB : JCS_EXT_XRGB;
#endif
    cinfo.input_components = 4;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE);

    // start compressor
    jpeg_start_compress(&cinfo, TRUE);

    unsigned char *pixels = cairo_image_surface_get_data(sfc);
    int stride = cairo_image_surface_get_stride(sfc);

    // loop over all lines and compress
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = pixels + (cinfo.next_scanline * stride);
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // finalize and close everything
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    *len = jpeg_len;

    // destroy temporary image surface (if available)
    if (other != NULL)
        cairo_surface_destroy(sfc);

    return CAIRO_STATUS_SUCCESS;
}

cairo_status_t _surface_to_stream(cairo_surface_t *sfc, cairo_write_func_t wf, void *closure)
{
    cairo_status_t e;
    unsigned char *data = NULL;
    size_t len = 0;
    // create JPEG data in memory from surface
    if ((e = _surface_to_stream(sfc, &data, &len)) != CAIRO_STATUS_SUCCESS)
        return e;

    // write whole memory block with stream function
    e = wf(closure, (const unsigned char *)data, len);

    // free JPEG memory again and return the return value
    ::free(data);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t surface_write(cairo_surface_t *sfc, const char *filename)
{
    cairo_status_t e;
    int outfile;

    // Open/create new file
    if ((outfile = open(filename, O_BINARY | O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
        return CAIRO_STATUS_DEVICE_ERROR;

    // write surface to file
    e = _surface_to_stream(sfc, _write_func, (void *)(intptr_t)outfile);

    // close file again and return
    close(outfile);
    return e;
}
static cairo_status_t surface_to_stream(cairo_surface_t *sfc, std::string &bin)
{
    cairo_status_t e;
    unsigned char *data = NULL;
    size_t len = 0;
    // create JPEG data in memory from surface
    if ((e = _surface_to_stream(sfc, &data, &len)) != CAIRO_STATUS_SUCCESS)
        return e;
    bin.assign((char *)data, len);
    ::free(data);
    return CAIRO_STATUS_SUCCESS;
}
///////////////////////////   下面是 读取 JPEG 图片的代码   ///////////////////////////

/// <summary>
/// !!!!!! 这个是实现核心 !!!!!!
/// 在内存中解压JPEG图片,并且创建Cairo image surface
/// </summary>
/// <param name="data">JPEG数据的指针</param>
/// <param name="len">JPEG数据长度</param>
/// <returns></returns>
static cairo_surface_t *surface_create_from_stream(void *data, size_t len)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    cairo_surface_t *sfc;

    // initialize jpeg decompression structures
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (const unsigned char *)data, len);
    (void)jpeg_read_header(&cinfo, TRUE);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    cinfo.out_color_space = JCS_EXT_BGRA;
#else
    cinfo.out_color_space = JCS_EXT_ARGB;
#endif

    // start decompressor
    (void)jpeg_start_decompress(&cinfo);

    // create Cairo image surface
    sfc = cairo_image_surface_create(CAIRO_FORMAT_RGB24, cinfo.output_width, cinfo.output_height);
    if (cairo_surface_status(sfc) != CAIRO_STATUS_SUCCESS)
    {
        jpeg_destroy_decompress(&cinfo);
        return sfc;
    }

    // loop over all scanlines and fill Cairo image surface
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *row_address = cairo_image_surface_get_data(sfc) + (cinfo.output_scanline * cairo_image_surface_get_stride(sfc));
        row_pointer[0] = row_address;
        (void)jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    // finish and close everything
    cairo_surface_mark_dirty(sfc);
    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    // set jpeg mime data
    cairo_surface_set_mime_data(sfc, CAIRO_MIME_TYPE_JPEG, (const unsigned char *)data, len, free, data);

    return sfc;
}

cairo_surface_t *surface_create_from_file(const char *filename)
{
    void *data;
    int infile;
    struct stat stat;

    // open input file
    if ((infile = ::open(filename, O_BINARY | O_RDONLY)) == -1)
        return nullptr;

    // get stat structure for file size
    if (::fstat(infile, &stat) == -1)
    {
        ::close(infile);
        return nullptr;
    }

    // allocate memory
    if ((data = ::malloc(stat.st_size)) == NULL)
    {
        ::close(infile);
        return nullptr;
    }

    // read data
    if (::read(infile, data, stat.st_size) < stat.st_size)
    {
        ::close(infile);
        return nullptr;
    }

    ::close(infile);

    cairo_surface_t *surf = surface_create_from_stream(data, stat.st_size);
    // ::free(data); 不能释放,会在cairo_surface_destroy释放
    return surf;
}

}  // namespace jpeg
#endif
}  // namespace cairo
}  // namespace silly

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
        case silly::color::type::eptRGBA:
            m_format = CAIRO_FORMAT_ARGB32;
            break;
        case silly::color::type::eptRGB:
            m_format = CAIRO_FORMAT_RGB24;
            break;
        case silly::color::type::eptGRAY:
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

bool silly_cairo::read(const std::string &path, const bool &png)
{
    if (png)
    {
        if (!(m_surface = silly::cairo::png::surface_create_from_file(path.c_str())))
        {
            return false;
        }
    }
    else
    {
#if ENABLE_JPEG

        if (!(m_surface = silly::cairo::jpeg::surface_create_from_file(path.c_str())))
        {
            return false;
        }
#else
        return false;

#endif
    }
    m_cr = cairo_create(m_surface);
    m_width = cairo_image_surface_get_width(m_surface);
    m_height = cairo_image_surface_get_height(m_surface);
    return true;
}

bool silly_cairo::write(const std::string &path, const bool &png)
{
    if (png)
    {
        return (CAIRO_STATUS_SUCCESS == silly::cairo::png::surface_write(m_surface, path.c_str()));
    }
#if ENABLE_JPEG
    else
    {
        return (CAIRO_STATUS_SUCCESS == silly::cairo::jpeg::surface_write(m_surface, path.c_str()));
    }
#endif
    return false;
}

bool silly_cairo::decode(const std::string &bin)
{
    return decode((const unsigned char *)bin.c_str(), bin.size());
}

bool silly_cairo::decode(const unsigned char *data, const size_t size)
{
    if (silly::png::data().valid((const char *)data, size))
    {
        m_surface = silly::cairo::png::surface_create_from_stream(data, size);
    }
#if ENABLE_JPEG
    else if (silly::jpeg::data().valid((const char *)data, size))
    {
        m_surface = silly::cairo::jpeg::surface_create_from_stream((void *)data, size);
    }
#endif
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

bool silly_cairo::encode(std::string &bin, const bool &png)
{
    if (png)
    {
        silly::cairo::png::surface_to_stream(m_surface, bin);
    }
#if ENABLE_JPEG
    else
    {
        silly::cairo::jpeg::surface_to_stream(m_surface, bin);
    }
#endif

    return !bin.empty();
}



void silly_cairo::release()
{
    DESTROY_CONTEXT(m_cr)
    DESTROY_SURFACE(m_surface)
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

void silly_cairo::set(const silly::color &color)
{
    switch (m_format)
    {
        case CAIRO_FORMAT_ARGB32:
            cairo_set_source_rgba(m_cr, CLR_CVT(color.red), CLR_CVT(color.green), CLR_CVT(color.blue), CLR_CVT(color.alpha));
            break;
        case CAIRO_FORMAT_RGB24:
            cairo_set_source_rgb(m_cr, CLR_CVT(color.red), CLR_CVT(color.green), CLR_CVT(color.blue));
            break;
        case CAIRO_FORMAT_A8:
            cairo_set_source_rgba(m_cr, CLR_CVT(color.alpha), CLR_CVT(color.alpha), CLR_CVT(color.alpha), CLR_CVT(color.alpha));
            break;
        default:
            break;
    }
}

void silly_cairo::clean(silly::color color)
{
    set(color);
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

    double x_pixel_per_degree = m_width / (rect.max.x - rect.min.x);
    double y_pixel_per_degree = m_height / (rect.max.y - rect.min.y);
    cairo_move_to(m_cr, (ring.points[0].x - rect.min.x) * x_pixel_per_degree, (rect.max.y - ring.points[0].y) * y_pixel_per_degree);
    for (int i = 1; i < ring.points.size(); ++i)
    {
        double x = (ring.points[i].x - rect.min.x) * x_pixel_per_degree;
        double y = (rect.max.y - ring.points[i].y) * y_pixel_per_degree;
        cairo_line_to(m_cr, x, y);
    }
    cairo_close_path(m_cr);
}

void silly_cairo::draw_ring_web_mercator(const silly_ring &ring, const silly_geo_rect &rect)
{
    if (ring.points.empty())
    {
        return;
    }
    double mc_left, mc_top, mc_right, mc_bottom;  // 多内环的情况这几个变量会重复计算,但是开销很小,可以暂时忽略
    silly_projection::geo_to_mercator(rect.min.x, rect.max.y, mc_left, mc_top);
    silly_projection::geo_to_mercator(rect.max.x, rect.min.y, mc_right, mc_bottom);

    cairo_new_sub_path(m_cr);
    double mcx, mcy;

    double x_pixel_per_degree = m_width / (mc_right - mc_left);
    double y_pixel_per_degree = m_height / (mc_top - mc_bottom);
    silly_projection::geo_to_mercator(ring.points[0].x, ring.points[0].y, mcx, mcy);
    cairo_move_to(m_cr, (mcx - mc_left) * x_pixel_per_degree, (mc_top - mcy) * y_pixel_per_degree);
    for (int i = 1; i < ring.points.size(); ++i)
    {
        silly_projection::geo_to_mercator(ring.points[i].x, ring.points[i].y, mcx, mcy);
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

size_t silly_cairo::count_occupy(const std::string &u8str)
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
        else  // 如果不是中文则认为是数字或者字母
        {
            ++count;
        }
    }
    return count;
}

void silly_cairo::set(const cairo_operator_t &opt)
{
    cairo_set_operator(m_cr, opt);
}

void silly_cairo::draw_line(const std::vector<silly_point> &line, const silly_geo_rect &rect)
{
    double x_pixel_per_degree = m_width / (rect.max.x - rect.min.x);
    double y_pixel_per_degree = m_height / (rect.max.y - rect.min.y);
    cairo_move_to(m_cr, (line[0].x - rect.min.x) * x_pixel_per_degree, (rect.max.y - line[0].y) * y_pixel_per_degree);
    for (int i = 1; i < line.size(); ++i)
    {
        cairo_line_to(m_cr, (line[i].x - rect.min.x) * x_pixel_per_degree, (rect.max.y - line[i].y) * y_pixel_per_degree);
    }
    // 实际绘制线条
    cairo_stroke(m_cr);
}

void silly_cairo::draw_line_web_mercator(const std::vector<silly_point> &line, const silly_geo_rect &rect)
{
    double mc_left, mc_top, mc_right, mc_bottom;  // 多内环的情况这几个变量会重复计算,但是开销很小,可以暂时忽略
    silly_projection::geo_to_mercator(rect.min.x, rect.max.y, mc_left, mc_top);
    silly_projection::geo_to_mercator(rect.max.x, rect.min.y, mc_right, mc_bottom);

    double mcx, mcy;

    double x_pixel_per_degree = m_width / (mc_right - mc_left);
    double y_pixel_per_degree = m_height / (mc_top - mc_bottom);
    silly_projection::geo_to_mercator(line[0].x, line[0].y, mcx, mcy);
    cairo_move_to(m_cr, (mcx - mc_left) * x_pixel_per_degree, (mc_top - mcy) * y_pixel_per_degree);
    for (int i = 1; i < line.size(); ++i)
    {
        silly_projection::geo_to_mercator(line[i].x, line[i].y, mcx, mcy);
        cairo_line_to(m_cr, (mcx - mc_left) * x_pixel_per_degree, (mc_top - mcy) * y_pixel_per_degree);
    }
    cairo_stroke(m_cr);
}

void silly_cairo::draw_point(const silly_point &p, const double &size, const silly_geo_rect &rect)
{
    cairo_set_line_width(m_cr, size);  // 圆点的直径

    double x = (p.x - rect.min.x) / (rect.max.x - rect.min.x) * m_width;
    double y = (rect.max.y - p.y) / (rect.max.y - rect.min.y) * m_height;

    // 绘制圆点
    cairo_arc(m_cr, x, y, size / 2., 0, 2 * silly::math::pi);  // 圆心位置 (100, 100), 半径 5
    cairo_stroke(m_cr);
}
