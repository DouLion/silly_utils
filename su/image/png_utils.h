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

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->png_jmpbuf)
#endif


//#define SU_PNG_PIXEL_RGBA(p, r, g, b, a) {p.red = r;  p.green = g; p.blue = b; p.alpha = a;}
//#define SU_PNG_PIXEL_RGB(p, r, g, b) {p.red = r;  p.green = g; p.blue = b; }
//#define SU_PNG_PIXEL_GA(p, g, a) {p.gray = g; p.alpha = a;}

namespace silly_image
{
	enum png_type
	{
		eptRGB = PNG_COLOR_TYPE_RGB,
		eptRGBA = PNG_COLOR_TYPE_RGB_ALPHA,
		eptGRAY = PNG_COLOR_TYPE_GRAY,
		eptGRAYA = PNG_COLOR_TYPE_GRAY_ALPHA

	};

	struct png_pixel
	{
		png_pixel(png_byte r, png_byte g, png_byte b, png_byte a) : red(r), green(g), blue(b), alpha(a), gray(0)
		{

		}

		png_pixel(png_byte r, png_byte g, png_byte b) : red(r), green(g), blue(b), alpha(0), gray(0)
		{

		}

		png_pixel(png_byte g, png_byte a) : red(0), green(0), blue(0), alpha(a), gray(0)
		{

		}

		png_pixel(png_byte g) : red(0), green(0), blue(0), alpha(0), gray(g)
		{

		}

		png_byte gray{ 0 };
		png_byte red{ 0 };
		png_byte green{ 0 };
		png_byte blue{ 0 };
		png_byte alpha{ 0 };
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
		void set_pixel(const size_t& r, const size_t& c, const png_pixel& sp);
		png_data  operator=(const png_data& other);
		png_bytep* data{ nullptr };
		png_uint_32 width{ 0 };
		png_uint_32 height{ 0 };
		png_uint_32 bit_depth{ 8 };
		png_uint_32 color_type{ PNG_COLOR_TYPE_RGB_ALPHA };
		png_uint_32 pixel_size{ 0 };

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
		static png_data create_empty(const size_t& rows, const size_t& cols, const png_uint_32& color_type, const png_uint_32& depth = 8);

		/// <summary>
		/// 从png中读取一个数据块
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		static png_data read(const char* path);

		/// <summary>
		/// 将png数据块写回文件
		/// </summary>
		/// <param name="path"></param>
		/// <param name="data"></param>
		/// <returns></returns>
		static bool write(const char* path, const png_data& data);
	};
}
#endif //SILLY_UTILS_PNG_UTILS_H
