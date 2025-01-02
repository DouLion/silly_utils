/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-02
 * @file: silly_png.cpp
 * @description: silly_png实现
 * @version: v1.0.1 2025-01-02 dou li yang
 */
#include "silly_png.h"
#include <png.h>
#include <setjmp.h>

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->png_jmpbuf)
#endif

static silly::color::type png2sillyctype(const int &type)
{
    switch (type)
    {
        case PNG_COLOR_TYPE_GRAY:
            return silly::color::type::eptGRAY;
        case PNG_COLOR_TYPE_RGB:
            return silly::color::type::eptRGB;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            return silly::color::type::eptGRAYA;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            return silly::color::type::eptRGBA;
        default:
            throw std::exception("不支持的类型");
    }
}

static int silly2pngctype(silly::color::type &type)
{
    switch (type)
    {
        case silly::color::type::eptGRAY:
            return PNG_COLOR_TYPE_GRAY;
        case silly::color::type::eptRGB:
            return PNG_COLOR_TYPE_RGB;
        case silly::color::type::eptGRAYA:
            return PNG_COLOR_TYPE_GRAY_ALPHA;
        case silly::color::type::eptRGBA:
            return PNG_COLOR_TYPE_RGB_ALPHA;
        default:
            throw std::exception("不支持的类型");
    }
}

static void silly_png_write_callback(png_structp png_ptr, png_bytep m_bytes, png_size_t length)
{
    std::string *p = (std::string *)png_get_io_ptr(png_ptr);
    p->insert(p->end(), m_bytes, m_bytes + length);
}

static void silly_png_read_callback(png_structp png_ptr, png_bytep m_bytes, png_size_t length)
{
    std::string *pngData = (std::string *)png_get_io_ptr(png_ptr);
    if (length > pngData->size())
    {
        length = pngData->size();
    }
    memcpy(m_bytes, pngData->data(), length);
    pngData->erase(0, length);
}

bool silly::png::tools::rgb2rgba(const silly::png::data &src, silly::png::data &dst, const uint8_t &alpha)
{
    if (src.type() != silly::color::type::eptRGB)
    {
        return false;
    }
    dst.release();
    if (!dst.create(src.height(), src.width(), silly::color::type::eptRGBA, src.depth()))
    {
        return false;
    }

    for (size_t r = 0; r < src.height(); ++r)
    {
        png_bytep srcp = src.nbytes()[r];
        png_bytep dstp = dst.nbytes()[r];
        for (size_t c = 0; c < src.width(); ++c)
        {
            memcpy(dstp, srcp, 3);
            srcp += 3;
            dstp += 4;
        }
    }

    return true;
}
bool silly::png::tools::rgba2rgb(const silly::png::data &src, silly::png::data &dst)
{
    if (src.type() != silly::color::type::eptRGBA)
    {
        return false;
    }
    dst.release();
    if (!dst.create(src.height(), src.width(), silly::color::type::eptRGB, src.depth()))
    {
        return false;
    }
    for (size_t r = 0; r < src.height(); ++r)
    {
        png_bytep srcp = src.nbytes()[r];
        png_bytep dstp = dst.nbytes()[r];
        for (size_t c = 0; c < src.width(); ++c)
        {
            memcpy(dstp, srcp, 3);
            srcp += 4;
            dstp += 3;
        }
    }
    return true;
}

silly::png::data &silly::png::data::operator=(const silly::png::data &rh)
{
    this->m_bytes = rh.m_bytes;
    this->m_width = rh.m_width;
    this->m_height = rh.m_height;
    this->m_channels = rh.m_channels;
    this->m_depth = rh.m_depth;
    this->m_type = rh.m_type;
    this->m_pixel_size = rh.m_pixel_size;
    this->m_nbytes = rh.m_nbytes;
    return *this;
}

void silly::png::data::pixel(const size_t &r, const size_t &c, const silly::color &sp)
{
    if (!(r < m_height && c < m_width))
    {
        // // SLOG_DEBUG("invalid %zu < %u and %zu < %u.", r, m_height, c, m_width);
        return;
    }
    size_t col_pos = c * m_pixel_size;
    switch (m_type)
    {
        case silly::color::type::eptGRAY:
            m_nbytes[r][col_pos] = sp.gray;
            break;
        case silly::color::type::eptRGB:
            m_nbytes[r][col_pos] = sp.red;
            m_nbytes[r][col_pos + 1] = sp.green;
            m_nbytes[r][col_pos + 2] = sp.blue;
            break;
        case silly::color::type::eptGRAYA:
            m_nbytes[r][col_pos] = sp.gray;
            m_nbytes[r][col_pos + 1] = sp.alpha;
            break;
        case silly::color::type::eptRGBA:
            m_nbytes[r][col_pos] = sp.red;
            m_nbytes[r][col_pos + 1] = sp.green;
            m_nbytes[r][col_pos + 2] = sp.blue;
            m_nbytes[r][col_pos + 3] = sp.alpha;
            break;
        default:
            break;
    }
}

silly::color silly::png::data::pixel(const size_t &r, const size_t &c) const
{
    silly::color sp;

    if (!(r < m_height && c < m_width))
    {
        return sp;
    }

    size_t col_pos = c * m_pixel_size;
    switch (m_type)
    {
        case silly::color::type::eptGRAY:
            sp.gray = m_nbytes[r][col_pos];
            break;
        case silly::color::type::eptRGB:
            sp.red = m_nbytes[r][col_pos];
            sp.green = m_nbytes[r][col_pos + 1];
            sp.blue = m_nbytes[r][col_pos + 2];
            break;
        case silly::color::type::eptGRAYA:
            sp.gray = m_nbytes[r][col_pos];
            sp.alpha = m_nbytes[r][col_pos + 1];
            break;
        case silly::color::type::eptRGBA:
            sp.red = m_nbytes[r][col_pos];
            sp.green = m_nbytes[r][col_pos + 1];
            sp.blue = m_nbytes[r][col_pos + 2];
            sp.alpha = m_nbytes[r][col_pos + 3];
            break;
        default:
            break;
    }

    return sp;
}
bool silly::png::data::create(const size_t &width, const size_t &height, const silly::color::type &type, const uint8_t &depth)
{
    m_type = static_cast<silly::color::type>(type);
    switch (m_type)
    {
        case silly::color::type::eptGRAY:
            m_channels = 1;
            break;
        case silly::color::type::eptRGB:
            m_channels = 3;
            break;
        case silly::color::type::eptGRAYA:
            m_channels = 2;
            break;
        case silly::color::type::eptRGBA:
            m_channels = 4;
            break;
        default:
            break;
    }

    m_pixel_size = sizeof(png_byte) * m_channels;
    if (!(m_pixel_size > 0 && m_width > 0 && m_height > 0))
    {
        return false;
    }
    m_width = width;
    m_height = height;
    m_depth = 8;
    m_bytes = new uint8_t[m_width * m_height * m_pixel_size];
    memset(m_bytes, 0, m_width * m_height * m_pixel_size);
    size_t row_bytes = m_width * m_pixel_size;
    m_nbytes.resize(m_height);
    for (size_t i = 0; i < m_height; ++i)
    {
        m_nbytes[i] = reinterpret_cast<unsigned char *>(m_bytes + i * row_bytes);
    }

    return true;
}
bool silly::png::data::read(const std::string &file)
{
    bool status = false;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    int sig_read = 0;
    uint32_t m_width, m_height;
    int bit_depth_1, color_type_1, interlace_type_1;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (nullptr == png_ptr)
    {
        return status;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (nullptr == info_ptr)
    {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return status;
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return status;
    }
    FILE *fp = fopen(file.c_str(), "rb");
    if (!fp)
    {
        return status;
    }

    // 设置输入流
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 0);
    // 读取png文件头
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &m_width, &m_height, &bit_depth_1, &color_type_1, &interlace_type_1, nullptr, nullptr);

    m_width = png_get_image_width(png_ptr, info_ptr);
    m_height = png_get_image_height(png_ptr, info_ptr);
    m_type = png2sillyctype(png_get_color_type(png_ptr, info_ptr));
    m_depth = png_get_bit_depth(png_ptr, info_ptr);
    m_channels = png_get_channels(png_ptr, info_ptr);
    if (!create(m_width, m_height, m_type, m_depth))
    {
        return status;
    }

    // 读取PNG图像像素数据
    png_read_image(png_ptr, (unsigned char **)&m_nbytes);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    fclose(fp);
    return true;
}
bool silly::png::data::write(const std::string &file) const
{
    if (!m_height || !m_width || !m_bytes)
    {
        // SLOG_DEBUG("invalid png m_bytes.");
        return false;
    }

    FILE *output_fp;
    output_fp = fopen(file.c_str(), "wb");

    png_structp png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (nullptr == png_write_ptr)
    {
        // SLOG_DEBUG("png_create_write_struct failed.");
        return false;
    }
    png_infop png_w_info = png_create_info_struct(png_write_ptr);
    if (nullptr == png_write_info)
    {
        // SLOG_DEBUG("png_create_info_struct failed.");
        return false;
    }
    if (setjmp(png_jmpbuf(png_write_ptr)))
    {
        png_destroy_read_struct(&png_write_ptr, &png_w_info, nullptr);
        fclose(output_fp);
        return false;
    }
    png_init_io(png_write_ptr, output_fp);
    png_set_IHDR(png_write_ptr, png_w_info, m_width, m_height, m_depth, m_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_write_ptr, png_w_info);
    png_write_image(png_write_ptr, (unsigned char **)&m_nbytes);
    png_write_end(png_write_ptr, nullptr);

    png_destroy_write_struct(&png_write_ptr, &png_w_info);
    fclose(output_fp);
    return true;
}
bool silly::png::data::decode(const std::string &bin)
{
    // 检查字符串是否为空
    if (!valid(bin))
    {
        return false;
    }

    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        m_err = "初始化PNG读取结构失败.";
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        m_err = "初始化PNG信息结构失败.";
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        m_err = "PNG读取错误.";
        return false;
    }

    // 设置输入数据作为PNG读取源
    png_set_read_fn(png_ptr, const_cast<std::string *>(&bin), silly_png_read_callback);

    // 读取PNG头部信息
    png_read_info(png_ptr, info_ptr);

    // 获取PNG图像的基本信息
    uint32_t width, height;
    int depth, png_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &depth, &png_type, NULL, NULL, NULL);
    png_read_update_info(png_ptr, info_ptr);
    create(height, width, png2sillyctype(png_type), depth);

    png_read_image(png_ptr, (unsigned char **)&m_nbytes);

    // 清理资源
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

    return true;
}
std::string silly::png::data::encode() const
{
    std::string buff;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        return buff;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return buff;
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return buff;
    }
    // std::vector<uint8_t> buff_vec;
    png_set_IHDR(png_ptr, info_ptr, m_width, m_height, m_depth, m_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_rows(png_ptr, info_ptr, (unsigned char **)&m_nbytes);
    png_set_write_fn(png_ptr, &buff, silly_png_write_callback, NULL);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return buff;
}