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
#include <log/silly_log.h>
#include <math/silly_matrix.h>
#include <image/silly_color.h>

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->png_jmpbuf)
#endif

namespace silly_image
{

enum png_type
{
    eptRGB = PNG_COLOR_TYPE_RGB,
    eptRGBA = PNG_COLOR_TYPE_RGB_ALPHA,
    eptGRAY = PNG_COLOR_TYPE_GRAY,
    eptGRAYA = PNG_COLOR_TYPE_GRAY_ALPHA

};

class png_data
{
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
    png_data operator=(const png_data &other);
  //private:
    png_bytep *data{nullptr};
    png_uint_32 width{0};
    png_uint_32 height{0};
    png_uint_32 bit_depth{8};
    png_uint_32 color_type{PNG_COLOR_TYPE_RGB_ALPHA};
    png_uint_32 pixel_size{0};
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
    static bool write(const std::string &path, const png_data &data);

    static bool memory_encode(const png_data &data, std::string &buff);

    static bool memory_decode(const std::string &buff, png_data &data);

    /// <summary>
    /// RGB图像转RGBA
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    static bool rgb_to_rgba(const png_data &src, png_data &dst, const png_byte& alpha = 255);

    /// <summary>
    /// RGBA图像转RGB
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    /// <returns></returns>
    static bool rgba_to_rgb(const png_data &src, png_data &dst);
};
}  // namespace silly_image
#endif  // SILLY_UTILS_PNG_UTILS_H
