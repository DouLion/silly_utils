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

/// 自定义的颜色类型与png的类型转换
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
            throw std::runtime_error("不支持的类型");
    }
}

static int silly2pngctype(const silly::color::type &type)
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
            throw std::runtime_error("不支持的类型");
    }
}
struct PngMemoryReader
{
    const char *data;
    size_t size;
    size_t offset;
};

static void silly_png_write_callback(png_structp png_ptr, png_bytep m_bytes, png_size_t length)
{
    std::string *p = (std::string *)png_get_io_ptr(png_ptr);
    p->insert(p->end(), m_bytes, m_bytes + length);
}

static void silly_png_read_callback(png_structp pngPtr, png_bytep outBytes, png_size_t byteCount)
{
    PngMemoryReader *reader = static_cast<PngMemoryReader *>(png_get_io_ptr(pngPtr));

    if (reader->offset + byteCount > reader->size)
    {
        png_error(pngPtr, "Attempt to read past end of buffer");
        return;
    }

    memcpy(outBytes, reader->data + reader->offset, byteCount);
    reader->offset += byteCount;
}

bool silly::png::tools::rgb2rgba(const silly::png::data &src, silly::png::data &dst, const uint8_t &alpha)
{
    return false;
    /*if (src.type() != silly::color::type::eptRGB)
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

    return true;*/
}
bool silly::png::tools::rgba2rgb(const silly::png::data &src, silly::png::data &dst)
{
    return false;
    /* if (src.type() != silly::color::type::eptRGBA)
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
     return true;*/
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
    this->m_nbytes.resize(this->m_height);
    for (unsigned int y = 0; y < this->m_height; y++)
    {
        this->m_nbytes[y] = this->m_bytes.data() + y * this->m_width * this->m_pixel_size;
    }
    return *this;
}

void silly::png::data::pixel(const size_t &r, const size_t &c, const silly::color &sp)
{
    if (!(r < m_height && c < m_width))
    {
        // // SLOG_DEBUG("invalid %zu < %u and %zu < %u.", r, m_height, c, m_width);
        return;
    }
    size_t pos = r * m_width * m_pixel_size + c * m_pixel_size;
    switch (m_type)
    {
        case silly::color::type::eptGRAY:
            m_bytes[pos] = sp.gray;
            break;
        case silly::color::type::eptRGB:
            m_bytes[pos] = sp.red;
            m_bytes[pos + 1] = sp.green;
            m_bytes[pos + 2] = sp.blue;
            break;
        case silly::color::type::eptGRAYA:
            m_bytes[pos] = sp.gray;
            m_bytes[pos + 1] = sp.alpha;
            break;
        case silly::color::type::eptRGBA:
            m_bytes[pos] = sp.red;
            m_bytes[pos + 1] = sp.green;
            m_bytes[pos + 2] = sp.blue;
            m_bytes[pos + 3] = sp.alpha;
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

    size_t pos = r * m_width * m_pixel_size + c * m_pixel_size;
    switch (m_type)
    {
        case silly::color::type::eptGRAY:
            sp.gray = m_bytes[pos];
            break;
        case silly::color::type::eptRGB:
            sp.red = m_bytes[pos];
            sp.green = m_bytes[pos + 1];
            sp.blue = m_bytes[pos + 2];
            break;
        case silly::color::type::eptGRAYA:
            sp.gray = m_bytes[pos];
            sp.alpha = m_bytes[pos + 1];
            break;
        case silly::color::type::eptRGBA:
            sp.red = m_bytes[pos];
            sp.green = m_bytes[pos + 1];
            sp.blue = m_bytes[pos + 2];
            sp.alpha = m_bytes[pos + 3];
            break;
        default:
            break;
    }

    return sp;
}
bool silly::png::data::create(const size_t &width, const size_t &height, const silly::color::type &type, const uint8_t &depth)
{
    m_type = static_cast<silly::color::type>(type);
    m_channels = silly::color::channels(m_type);
    if (!m_channels)
    {
        return false;
    }
    m_pixel_size = sizeof(png_byte) * m_channels;
    m_width = width;
    m_height = height;
    if (!(m_pixel_size > 0 && m_width > 0 && m_height > 0))
    {
        return false;
    }

    m_depth = depth;
    m_bytes.resize(m_width * m_height * m_pixel_size, 0);

    m_nbytes.resize(m_height);
    for (unsigned int y = 0; y < m_height; y++)
    {
        m_nbytes[y] = m_bytes.data() + y * m_width * m_pixel_size;
    }

    return true;
}
bool silly::png::data::read(const std::string &file)
{
    bool status = false;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    int sig_read = 0;

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
    // png_get_IHDR(png_ptr, info_ptr, &m_width, &m_height, &bit_depth_1, &color_type_1, &interlace_type_1, nullptr, nullptr);

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
    png_read_image(png_ptr, m_nbytes.data());
    png_read_end(png_ptr, nullptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    fclose(fp);
    return true;
}
bool silly::png::data::write(const std::string &file) const
{
    if (!m_height || !m_width || m_bytes.empty())
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
    png_set_IHDR(png_write_ptr, png_w_info, m_width, m_height, m_depth, silly2pngctype(m_type), PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_write_ptr, png_w_info);
    png_write_image(png_write_ptr, (png_bytep*)m_nbytes.data());
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
    // 设置自定义读取函数
    PngMemoryReader reader = {bin.data(), bin.size(), 0};
    png_set_read_fn(png_ptr, &reader, silly_png_read_callback);

    // 读取PNG头部信息
    // 读取图像信息
    png_read_info(png_ptr, info_ptr);

    m_width = png_get_image_width(png_ptr, info_ptr);
    m_height = png_get_image_height(png_ptr, info_ptr);
    m_type = png2sillyctype(png_get_color_type(png_ptr, info_ptr));
    m_depth = png_get_bit_depth(png_ptr, info_ptr);

    // 转换为 8-bit 深度
    /* if (bitDepth == 16)
         png_set_strip_16(png_ptr);
     if (bitDepth < 8)
         png_set_packing(png_ptr);*/

    //// 添加 Alpha 通道（如果存在）
    // if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    //{
    //     png_set_tRNS_to_alpha(png_ptr);
    // }

    //// 转换颜色类型为 RGBA
    // if (colorType == PNG_COLOR_TYPE_PALETTE)
    //{
    //     png_set_palette_to_rgb(png_ptr);
    // }
    // if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
    //{
    //     png_set_expand_gray_1_2_4_to_8(png_ptr);
    // }
    // if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
    //{
    //     png_set_gray_to_rgb(png_ptr);
    // }
    // if (colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_PALETTE)
    //{
    //     png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);
    // }

    png_read_update_info(png_ptr, info_ptr);
    create(m_width, m_height, m_type, m_depth);
    std::vector<png_bytep> m_nbytes(m_height);
    for (unsigned int y = 0; y < m_height; y++)
    {
        m_nbytes[y] = m_bytes.data() + y * m_width * m_pixel_size;
    }

    png_read_image(png_ptr, m_nbytes.data());
    png_read_end(png_ptr, nullptr);

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
    int pngtype = silly2pngctype(m_type);
    png_set_IHDR(png_ptr, info_ptr, m_width, m_height, m_depth, pngtype, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    // png_set_compression_strategy(png_ptr, PNG_COMPRESSION_STRATEGY_DEFAULT);
    png_set_compression_level(png_ptr, m_compress_level);
    png_set_option(png_ptr, PNG_SKIP_sRGB_CHECK_PROFILE, PNG_OPTION_ON);  // 跳过sRGB校验
    png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_NO_FILTERS);        // 禁用滤波
    std::vector<png_bytep> m_nbytes(m_height);
    for (unsigned int y = 0; y < m_height; y++)
    {
        m_nbytes[y] = (png_bytep)m_bytes.data() + y * m_width * m_pixel_size;
    }
    png_set_rows(png_ptr, info_ptr, m_nbytes.data());
    png_set_write_fn(png_ptr, &buff, silly_png_write_callback, NULL);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return buff;
}