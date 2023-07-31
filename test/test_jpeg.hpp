/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/31 10:31
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_TEST_JPEG_HPP
#define SILLY_UTILS_TEST_JPEG_HPP

#define BOOST_TEST_INCLUDED
#include "image/jpeg_utils.h"
#include <boost/test/unit_test.hpp>
#include <filesystem>

BOOST_AUTO_TEST_SUITE(TestJPEG)


BOOST_AUTO_TEST_CASE(JPEG_READ)      // 根据二维数组value 创建一个jpeg图片
{
	std::cout << "\r\n\r\n****************" << "matrix_2d_to_jpeg" << "****************" << std::endl;

	jpeg_utils ju;
	matrix_2d<unsigned char> matrix;
	matrix.rows = 400;
	matrix.cols = 300;
	matrix.data = (unsigned char**)malloc(matrix.cols * sizeof(unsigned char*));
	for (int row = 0; row < matrix.cols; ++row)
	{
		matrix.data[row] = (unsigned char*)malloc(matrix.rows * sizeof(unsigned char*));
	}

	std::vector<unsigned char> threshold{ (unsigned char)0,(unsigned char)5,(unsigned char)15,(unsigned char)25,(unsigned char)50,(unsigned char)100 };

	std::vector<jpeg_pixel> pixel_colors{ jpeg_pixel((char)0xc9, (char)0xec, (char)0xc9), jpeg_pixel((char)0x9b, (char)0xdd, (char)0xa9), jpeg_pixel((char)0x7e, (char)0xcc, (char)0x7b), jpeg_pixel((char)0x89, (char)0x99, (char)0x00), jpeg_pixel((char)0x8f, (char)0x3a, (char)0x00), jpeg_pixel((char)0xFA, (char)0x00, (char)0xFA) };

	jpeg_data jd;
	jd.matrix2d_to_rgb_jpeg(matrix, threshold, pixel_colors);
	std::string ma = "./jpeg/matrix_to_color_1.jpeg";
	ju.write_jpeg_data(ma.c_str(), jd);
	jd.release();

	for (int row = 0; row < matrix.cols; ++row)
	{
		free(matrix.data[row]);
	}
	free(matrix.data);
}


BOOST_AUTO_TEST_CASE(WRITE_JPEG)      // 写入 JPEG
{
	std::cout << "\r\n\r\n****************" << "WRITE_JPEG" << "****************" << std::endl;

}


BOOST_AUTO_TEST_CASE(WRITE_JPEG_2)      // 写入 JPEG
{
	std::cout << "\r\n\r\n****************" << "WRITE_JPEG" << "****************" << std::endl;

}

BOOST_AUTO_TEST_SUITE_END()


#endif //SILLY_UTILS_TEST_JPEG_HPP
