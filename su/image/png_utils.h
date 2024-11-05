/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/25 10:11
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_PNG_UTILS_H
#define SILLY_UTILS_PNG_UTILS_H

#include <png.h>
#include <setjmp.h>
#include <math/silly_matrix.h>
#include <image/silly_color.h>

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->png_jmpbuf)
#endif

/// 1x1的空白png图片
const static std::vector<unsigned char> SILLY_1X1_RGBA_PNG = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                                                              0x01, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0x15, 0xC4, 0x89, 0x00, 0x00, 0x00, 0x0B, 0x49, 0x44, 0x41, 0x54, 0x08, 0x99, 0x63, 0x60, 0x00,
                                                              0x02, 0x00, 0x00, 0x05, 0x00, 0x01, 0x62, 0x55, 0x32, 0x88, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
class png_utils;
class png_data
{
    friend class png_utils;
  public:
    enum color_type
    {
        eptRGB = PNG_COLOR_TYPE_RGB,
        eptRGBA = PNG_COLOR_TYPE_RGB_ALPHA,
        eptGRAY = PNG_COLOR_TYPE_GRAY,
        eptGRAYA = PNG_COLOR_TYPE_GRAY_ALPHA

    };

  public:
    png_data() = default;

    void release();

    /// <summary>
    /// 设置指定像素位置的值(颜色)
    /// </summary>
    /// <param name="r"></param>
    /// <param name="c"></param>
    /// <param name="sp"></param>
    void set_pixel(const size_t &r, const size_t &c, const silly_color &sp);
    silly_color get_pixel(const size_t &r, const size_t &c) const;

    // 在赋值给另一个变量 data 的时候直接把自己地址传递给了另一个变量,没有拷贝一份数据
    png_data operator=(const png_data &other);
    static bool is_png(const std::string& bin);
    static bool is_png(const char *data, const size_t size);
    bool is_empty();

  protected:
    // private:
    png_bytep * m_data{nullptr};
    png_uint_32 m_width{0};
    png_uint_32 m_height{0};
    png_uint_32 m_bit_depth{8};
    png_uint_32 m_color_type{PNG_COLOR_TYPE_RGB_ALPHA};
    png_uint_32 m_pixel_size{0};
  private:
    // PNG 文件总是以一个8字节的签名开始，这个签名用来识别文件是否为 PNG 格式。
    static constexpr unsigned char HEADER[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
};

class png_utils
{
  public:
    /// <summary>
    /// 创建一个空的png数据块
    /// </summary>
    /// <param name="rows">行数</param>
    /// <param name="cols">列数</param>
    /// <param name="color_type">RGB,RGBA,GRAY,GRAY ALPHA</param>
    /// <param name="depth">这个参数暂时没用</param>
    /// <returns></returns>
    static png_data create_empty(const size_t &rows, const size_t &cols, const png_uint_32 &color_type = PNG_COLOR_TYPE_RGB_ALPHA, const png_uint_32 &depth = 8);

    /// <summary>
    /// 从png中读取一个数据块
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static png_data read(const std::string &path);

    /// <summary>
    /// 将png数据块写回文件, TODO: 这个函数在Centos上保存图片时,图片会有截断,导致保存内容有缺失,检查问题,优先使用encode_to_memory转为二进制流然后写入文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    static bool write(const std::string &path, const png_data &pd);

    static bool memory_encode(const png_data &data, std::string &buff);

    static bool memory_decode(const std::string &buff, png_data &pd);

    /// <summary>
    /// RGB图像转RGBA
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    static bool rgb_to_rgba(const png_data &src, png_data &dst, const png_byte &alpha = 255);

    /// <summary>
    /// RGBA图像转RGB
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    static bool rgba_to_rgb(const png_data &src, png_data &dst);
};
#endif  // SILLY_UTILS_PNG_UTILS_H
