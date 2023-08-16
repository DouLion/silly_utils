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
#include <vector>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>

#include "jpeglib.h"
#include "jerror.h"
#include <setjmp.h>

#include "math/silly_matrix.h"
using namespace silly_math;

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
	JSAMPLE  gray{ 0 };    // 灰色分量
	// unsigned char = JSAMPLE
};

//template<typename T>
//struct matrix_2d
//{
//	T** data;
//	size_t rows;
//	size_t cols;
//};

class jpeg_data
{
public: 

	jpeg_data() = default;

	bool release();

    // row : 第几行   col :第几列
	bool set_pixel(const size_t& row, const size_t& col, const jpeg_pixel& pixel);

	template<typename T>
	bool matrix2d_to_rgb_jpeg(matrix_2d<T> matrix, std::vector<T> threshold, std::vector<jpeg_pixel> pixel_colors)
	{
        if (threshold.empty())
        {
            std::cout << "阈值为空 " << std::endl;
            return false;
        }
        if (pixel_colors.empty())
        {
            std::cout << "色彩位空 " << std::endl;
            return false;
        }
        if (threshold.size() != pixel_colors.size())
        {
            std::cout << "阈值与阈值颜色个数不匹配 " << std::endl;
            return false;
        }
        if (nullptr == matrix.get_data())
        {
            std::cout << "矩阵为空 " << std::endl;
            return false;
        }

        this->jpeg_width = matrix.col();
        this->jpeg_height = matrix.row();
        this->jpeg_components = 3;
        this->color_space = JCS_RGB;

        this->image_data = (unsigned char*)malloc(sizeof(unsigned char*) * jpeg_width * jpeg_height * jpeg_components);
        memset((void*)this->image_data, 255, sizeof(unsigned char*) * jpeg_width * jpeg_height * jpeg_components);
        int row_size = jpeg_width * jpeg_components;
        int  jmp = sizeof(unsigned char) * jpeg_components;
        int m = threshold.size() - 1;
        T** mtrix_data = matrix.get_data();
        for (int i = 0; i < jpeg_height; i++)
        {
            for (int j = 0; j < jpeg_width; j++)
            {
                bool change = false;
                for (int n = 0; n < threshold.size()-1; n++)
                {
                    if (mtrix_data[i][j] >= threshold[n] && mtrix_data[i][j] <= threshold[n + 1])
                    {
                        set_pixel(i, j, pixel_colors[n]);
                        change = true;
                        break;
                    }
                }
                if (false == change)
                {
                    set_pixel(i, j, pixel_colors[m]);
                }
            }
        }
        return true;
	}


	jpeg_data  operator=(const jpeg_data& other);
	// 参数-----------------
	JDIMENSION jpeg_width{ 0 };
	JDIMENSION jpeg_height{ 0 };
	int jpeg_components{ 3 };		// 色带通道数 1 ro 3
	J_COLOR_SPACE color_space;		// 色彩空间必须为枚举常量之一，通常为JCS_RGB或JCS_GRAYSCALE
	int quality{ 75 };
	int data_precision{ 8 };				/* 输入图像的数据精度 一般为8*/
    long fileSize;  // 该jpeg栈多少字节
	unsigned char* image_data{ nullptr };

private:

};


class jpeg_utils
{
public:

	static jpeg_data creat_empty_jpeg(const size_t& width, const size_t& height, const size_t& components, const J_COLOR_SPACE& color_space, const size_t& quality = 75, const size_t& data_precision = 8);


	bool write_jpeg_data(const char* path, const jpeg_data& jpeg_data);

	jpeg_data read_jpeg(const char* path);

    //将jpeg_data转编码为内存jpeg数据
    static bool encode_to_memory(const jpeg_data& jpeg_data, char** buf, size_t& len);


	jpeg_utils() = default;
	~jpeg_utils() = default;

private:

};

#endif //SILLY_UTILS_JPEG_UTILS_H
