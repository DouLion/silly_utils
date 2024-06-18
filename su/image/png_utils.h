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
#include <vector>
#include <iostream>
#include "math/silly_matrix.h"
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
    void set_pixel(const size_t &r, const size_t &c, const unsigned char &sp);
    unsigned char* operator=(const png_data &other);
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
    static png_data read(const char *path);

    /// <summary>
    /// 将png数据块写回文件, TODO: 这个函数在Centos上保存图片时,图片会有截断,导致保存内容有缺失,检查问题,优先使用encode_to_memory转为二进制流然后写入文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    static bool write(const char *path, const png_data &data);

    /// <summary>
    /// 将png_data转编码为内存png数据
    /// </summary>
    /// <param name="data"></param>
    /// <param name="buf"></param>
    /// <param name="len"></param>
    /// <returns></returns>
    static bool encode_to_memory(const png_data &data, char **buf, size_t &len);

    static std::string encode_to_memory(const png_data &data);

    /// <summary>
    /// 将两个矩阵转为png,一个矩阵存储RGB,一个矩阵存阴影值
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="evel">存储这RGB的值</param>
    /// <param name="share">只存储阴影值</param>
    /// <param name="threshold">阈值</param>
    /// <param name="pixel_colors">颜色</param>
    /// <returns></returns>
    template <typename T>
    static png_data evel_share_to_png(silly_math::matrix_2d<T> evel, silly_math::matrix_2d<T> share, std::vector<T> threshold, std::vector<unsigned char> pixel_colors)
    {
        int height = evel.row();
        int width = evel.col();
        if (height != share.row() || width != share.col())
        {
            std::cout << "The size of the elevation matrix and shadow matrix is not equal " << std::endl;
            return png_data();
        }
        png_data block_image = silly_image::png_utils::create_empty(height, width, PNG_COLOR_TYPE_RGB_ALPHA);
        for (int r = 0; r < evel.row(); r++)
        {
            for (int c = 0; c < evel.col(); c++)
            {
                T value = evel.at(r, c);
                size_t n;
                for (n = 0; n < threshold.size(); n++)  // 从1开始，因为初始值不需要比较
                {
                    if (value < threshold.at(n))
                    {
                        break;  // 找到匹配的值后，跳出内循环
                    }
                }
                size_t col_pos = c * block_image.pixel_size;
                block_image.data[r][col_pos] = pixel_colors[n].red;
                block_image.data[r][col_pos + 1] = pixel_colors[n].green;
                block_image.data[r][col_pos + 2] = pixel_colors[n].blue;
                block_image.data[r][col_pos + 3] = share.at(r, c);
            }
        }
        return block_image;
    }
};
}  // namespace silly_image
#endif  // SILLY_UTILS_PNG_UTILS_H
