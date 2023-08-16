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
#include <fstream>

BOOST_AUTO_TEST_SUITE(TestJPEG)


BOOST_AUTO_TEST_CASE(encode_to_memory)      // 根据二维数组value 创建一个jpeg图片
{
	std::cout << "\r\n\r\n****************" << "encode_to_memory" << "****************" << std::endl;
	// 对边读一张图片，并另存
	jpeg_utils ju;
	std::filesystem::path data_root(DEFAULT_DATA_DIR);
	data_root += "/jpeg/color_2.jpeg";

	jpeg_data temp_jpeg_7 = ju.read_jpeg(data_root.string().c_str());
	
	char* buf = nullptr;
	size_t len = 0;

	if (ju.encode_to_memory(temp_jpeg_7, &buf, len))
	{
		std::cout << "转换完成" << std::endl;

		std::filesystem::path path_root(DEFAULT_DATA_DIR);
		path_root += "/jpeg/1.jpeg";

		std::ofstream outfile(path_root.string().c_str(), std::ios::binary);
		if (!outfile)
		{
			std::cout << "文件打开失败" << std::endl;
		}

		outfile.write(buf, len);
		outfile.close();

		free(buf);
	}
	else
	{
		std::cout << "转换失败" << std::endl;

		// 编码到内存失败
	}
	//std::filesystem::path data_root_2(DEFAULT_DATA_DIR);
	//data_root_2 += "/jpeg/color_2.jpeg";
	//ju.write_jpeg_data(data_root_2.string().c_str(), temp_jpeg_7);
	temp_jpeg_7.release();


}



BOOST_AUTO_TEST_CASE(JPEG_READ)      // 根据二维数组value 创建一个jpeg图片
{
	std::cout << "\r\n\r\n****************" << "JPEG_READ" << "****************" << std::endl;
	// 对边读一张图片，并另存
	jpeg_utils ju;
	std::filesystem::path data_root(DEFAULT_DATA_DIR);
	data_root += "/jpeg/color_1.jpeg";
	//std::string ru_1 = "./jpeg/color_1.jpeg";
	jpeg_data temp_jpeg_7 = ju.read_jpeg(data_root.string().c_str());
	std::filesystem::path data_root_2(DEFAULT_DATA_DIR);
	data_root_2+="/jpeg/color_2.jpeg";
	ju.write_jpeg_data(data_root_2.string().c_str(), temp_jpeg_7);
	temp_jpeg_7.release();


}


BOOST_AUTO_TEST_CASE(WRITE_JPEG)      // 写入 JPEG
{
	std::cout << "\r\n\r\n****************" << "WRITE_JPEG" << "****************" << std::endl;
	// 创建空白图片并写入
	jpeg_utils ju;
	jpeg_data temp_jpeg_2 = ju.creat_empty_jpeg(300, 300, 3, JCS_RGB);
	std::filesystem::path data_root_3(DEFAULT_DATA_DIR);
	data_root_3+="/jpeg/empty_while.jpeg";
	ju.write_jpeg_data(data_root_3.string().c_str(), temp_jpeg_2);
	temp_jpeg_2.release();

}


BOOST_AUTO_TEST_CASE(SET_PIXEL_COLOR)      // 设置像素点颜色
{
	std::cout << "\r\n\r\n****************" << "SET_PIXEL_COLOR" << "****************" << std::endl;
	// 改变某像素点坐标
	jpeg_utils ju;
	std::filesystem::path data_root_4(DEFAULT_DATA_DIR);
	data_root_4+="/jpeg/empty_while.jpeg";
	jpeg_data temp_jpeg_8 = ju.read_jpeg(data_root_4.string().c_str());

	std::filesystem::path data_root_5(DEFAULT_DATA_DIR);
	data_root_5+="/jpeg/one_blace.jpeg";
	int row = 50; // 第几行
	int col = 30; // 第几列
	jpeg_pixel jp(0,0,0);
	temp_jpeg_8.set_pixel(row, col, jp);
	ju.write_jpeg_data(data_root_5.string().c_str(), temp_jpeg_8);
	temp_jpeg_8.release();
}

BOOST_AUTO_TEST_CASE(MATRIX_TO_JPEG)      // 将二维矩阵转化为jpeg
{
	std::cout << "\r\n\r\n****************" << "MATRIX_TO_JPEG" << "****************" << std::endl;
	jpeg_utils ju;
	// 创建一个unsigned char类型的二维矩阵 并为他虚拟出逐列递增的值
	matrix_2d<unsigned char> matrix;
	int width = 300;
	int height = 400;
	matrix.create(height, width);
	unsigned char** data = matrix.get_data();
	for (int r = 0; r < height; r++)
	{
		for (int c = 0; c < width; c++)
		{
			data[r][c] = (unsigned char)(c / 401.0 * 255);
		}

	}

	std::vector<unsigned char> threshold{ (unsigned char)0,(unsigned char)5,(unsigned char)15,(unsigned char)25,(unsigned char)50,(unsigned char)100 };

	std::vector<jpeg_pixel> pixel_colors{ {jpeg_pixel((char)0xc9, (char)0xec, (char)0xc9)}, {jpeg_pixel((char)0x9b, (char)0xdd, (char)0xa9)}, {jpeg_pixel((char)0x7e, (char)0xcc, (char)0x7b)}, {jpeg_pixel((char)0x89, (char)0x99, (char)0x00)},{ jpeg_pixel((char)0x8f, (char)0x3a, (char)0x00)}, {jpeg_pixel((char)0xFA, (char)0x00, (char)0xFA)} };


	jpeg_data jd;
	std::filesystem::path data_root_6(DEFAULT_DATA_DIR);
	data_root_6+="/jpeg/matrix_to_color_1.jpeg";
	jd.matrix2d_to_rgb_jpeg<unsigned char>(matrix, threshold, pixel_colors);
	ju.write_jpeg_data(data_root_6.string().c_str(), jd);
	jd.release();
	matrix.destroy();
}

BOOST_AUTO_TEST_SUITE_END()


#endif //SILLY_UTILS_TEST_JPEG_HPP
