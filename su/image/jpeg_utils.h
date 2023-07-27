/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/26 15:56
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_JPEG_UTILS_H
#define SILLY_UTILS_JPEG_UTILS_H

#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "jpeglib.h"
#include "jerror.h"
#include <setjmp.h>
   

 // 像素颜色结构体 pixel_color
struct jpeg_pixel
{
	jpeg_pixel() : red(0), green(0), blue(0), gray(0)
	{

	}
	jpeg_pixel(JSAMPLE r, JSAMPLE g, JSAMPLE b) : red(r), green(g), blue(b), gray(0)
	{

	}
	jpeg_pixel(JSAMPLE g) : red(0), green(0), blue(0), gray(g)
	{

	}
	JSAMPLE  red{ 0 };     // 红色分量
	JSAMPLE  green{ 0 };   // 绿色分量
	JSAMPLE  blue{ 0 };    // 蓝色分量
	JSAMPLE  gray{ 0 };    // 蓝色分量
	// unsigned char = JSAMPLE
};

class jpeg_data
{
public:

	jpeg_data() = default;

	bool release();

	bool set_pixel(const size_t& row, const size_t& col, const jpeg_pixel& pixel);


	// 参数-----------------
	JDIMENSION jpeg_width{ 0 };
	JDIMENSION jpeg_height{ 0 };
	int jpeg_components{ 3 };		// 色带通道数 1 ro 3
	J_COLOR_SPACE color_space;		// 色彩空间必须为枚举常量之一，通常为JCS_RGB或JCS_GRAYSCALE
	int quality{ 75 };
	int data_precision;				/* 输入图像的数据精度 一般为8*/

	unsigned char* image_data{ nullptr };

private:

};

class jpeg_utils
{
public:

	jpeg_data creat_empty_jpeg(const size_t& width, const size_t& height, const size_t& components, const J_COLOR_SPACE& color_space, const size_t& quality = 75, const size_t& data_precision = 8);


	bool write_jpeg_data(const char* path, const jpeg_data& jpeg_data);

	jpeg_data read_jpeg(const char* path);




	jpeg_utils() = default;
	~jpeg_utils() = default;
};

#endif //SILLY_UTILS_JPEG_UTILS_H
