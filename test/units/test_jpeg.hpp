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

#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>
#include "image/jpeg_utils.h"

std::string readFile(const std::string& filePath)
{
    // 打开文件
    std::ifstream file(filePath, std::ios::binary);

    // 检查文件是否成功打开
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    // 使用 std::string 的构造函数从文件流中读取内容
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // 关闭文件
    file.close();

    return content;
}


TEST_CASE("TestJPEG")
{



SECTION("dencode_to_memory")
{
        std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
        data_root += "/jpeg/1.jpeg";
        std::string jpeg_string = readFile(data_root.string());


        jpeg_data jpeg_info;
        if (jpeg_utils::memory_decode(jpeg_string, jpeg_info))
        {
            std::cout << "Width: " << jpeg_info.jpeg_width << std::endl;
            std::cout << "Height: " << jpeg_info.jpeg_height << std::endl;
            std::cout << "Components: " << jpeg_info.jpeg_components << std::endl;
            std::cout << "Color Space: " << jpeg_info.color_space << std::endl;
            std::cout << "File Size: " << jpeg_info.fileSize << std::endl;
        }
        else
        {
            std::cerr << "Failed to decode JPEG data." << std::endl;
        }

        std::filesystem::path path_root(DEFAULT_SU_DATA_DIR);
        path_root += "/jpeg/22.jpeg";
        jpeg_utils::write(path_root.string().c_str(), jpeg_info);

		int a = 0;

}

SECTION("encode_to_memory")      // 根据二维数组value 创建一个jpeg图片
{
	std::cout << "\r\n\r\n****************" << "encode_to_memory" << "****************" << std::endl;
	// 对边读一张图片，并另存
	jpeg_utils ju;
	std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
	data_root += "/jpeg/color_2.jpeg";

	jpeg_data temp_jpeg_7 = ju.read(data_root.string().c_str());
	
	char* buf = nullptr;
	size_t len = 0;

	if (ju.memory_encode(temp_jpeg_7, &buf, len))
	{
		std::cout << "转换完成" << std::endl;

		std::filesystem::path path_root(DEFAULT_SU_DATA_DIR);
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
	//std::filesystem::path data_root_2(DEFAULT_SU_DATA_DIR);
	//data_root_2 += "/jpeg/color_2.jpeg";
	//ju.write_jpeg_data(data_root_2.string().c_str(), temp_jpeg_7);
	temp_jpeg_7.release();

	int a = 0;


}



SECTION("JPEG_READ")      // 根据二维数组value 创建一个jpeg图片
{
	std::cout << "\r\n\r\n****************" << "JPEG_READ" << "****************" << std::endl;
	// 对边读一张图片，并另存
	jpeg_utils ju;
	std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
	data_root += "/jpeg/color_1.jpeg";
	//std::string ru_1 = "./jpeg/color_1.jpeg";
	jpeg_data temp_jpeg_7 = ju.read(data_root.string().c_str());
	std::filesystem::path data_root_2(DEFAULT_SU_DATA_DIR);
	data_root_2+="/jpeg/color_2.jpeg";
	ju.write(data_root_2.string().c_str(), temp_jpeg_7);
	temp_jpeg_7.release();


}


SECTION("WRITE_JPEG")      // 写入 JPEG
{
	std::cout << "\r\n\r\n****************" << "WRITE_JPEG" << "****************" << std::endl;
	// 创建空白图片并写入
	jpeg_utils ju;
	jpeg_data temp_jpeg_2 = ju.creat_empty(300, 300, 3, JCS_RGB);
	std::filesystem::path data_root_3(DEFAULT_SU_DATA_DIR);
	data_root_3+="/jpeg/empty_while.jpeg";
	ju.write(data_root_3.string().c_str(), temp_jpeg_2);
	temp_jpeg_2.release();

}


SECTION("SET_PIXEL_COLOR")      // 设置像素点颜色
{
	std::cout << "\r\n\r\n****************" << "SET_PIXEL_COLOR" << "****************" << std::endl;
	// 改变某像素点坐标
	jpeg_utils ju;
	std::filesystem::path data_root_4(DEFAULT_SU_DATA_DIR);
	data_root_4+="/jpeg/empty_while.jpeg";
	jpeg_data temp_jpeg_8 = ju.read(data_root_4.string().c_str());

	std::filesystem::path data_root_5(DEFAULT_SU_DATA_DIR);
	data_root_5+="/jpeg/one_blace.jpeg";
	int row = 50; // 第几行
	int col = 30; // 第几列
	jpeg_pixel jp(0,0,0);
	temp_jpeg_8.set_pixel(row, col, jp);
	ju.write(data_root_5.string().c_str(), temp_jpeg_8);
	temp_jpeg_8.release();
}

}


#endif //SILLY_UTILS_TEST_JPEG_HPP
