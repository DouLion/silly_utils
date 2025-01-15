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
#include "image/silly_jpeg.h"

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

#include <ctime>
#include <cstdlib>
void initialize_random_generator()

{
    std::srand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
}

TEST_CASE("TestJPEG")
{
    SECTION("get pixel")  // 根据二维数组value 创建一个jpeg图片
    {
        std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
        data_root += "/jpeg/1.jpeg";
        silly::jpeg::data jped_data_1;
        if (!jped_data_1.read(data_root.string()))
        {
            std::cout << "read jpeg failed" << std::endl;
        }
        for (int index = 0; index < 100; index++)
        {
            silly::jpeg::data jped_data_2;
            if (!jped_data_2.create(jped_data_1.width(), jped_data_1.height(), jped_data_1.type()))
            {
                std::cout << "create jpeg failed" << std::endl;
            }

            for (int i = 0; i < jped_data_1.width(); i++)
            {
                for (int j = 0; j < jped_data_1.height(); j++)
                {
                    silly::color tc = jped_data_1.pixel(i, j);
                    jped_data_2.pixel(i, j, tc);
                }
            }

            std::filesystem::path temp = data_root;
            temp += std::to_string(index) + ".jpeg";
            if (!jped_data_2.write(temp.string()))
            {
                std::cout << "create jpeg failed" << std::endl;
            }
            jped_data_2.release();
        }
        jped_data_1.release();
        int a = 0;
        int b = 0;
        int c = 0;
    }

    SECTION("read")  // 根据二维数组value 创建一个jpeg图片
    {
        initialize_random_generator();
        std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
        data_root += "/jpeg/1.jpeg";
        for (int i = 0; i < 100; i++)
        {
            silly::jpeg::data jped_data_1;
            if (!jped_data_1.read(data_root.string()))
            {
                std::cout << "read jpeg failed" << std::endl;
            }
            std::filesystem::path write_p(DEFAULT_SU_DATA_DIR);
            write_p += "/jpeg/create/";
            write_p += std::to_string(i) + ".jpeg";
            if (!jped_data_1.write(write_p.string()))
            {
                std::cout << "create jpeg failed" << std::endl;
            }
            jped_data_1.release();
        }

        int a = 0;
        int b = 0;
        int c = 0;
    }

    SECTION("create and write")
    {
        initialize_random_generator();
        std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
        data_root += "/jpeg/create/";

        for (int index = 0; index < 100; index++)
        {
            silly::jpeg::data jped_data_1;
            if (!jped_data_1.create(100, 100, silly::color::eptGRAY))
            {
                std::cout << "create jpeg failed" << std::endl;
            }
            for (int i = 0; i < 100; i++)
            {
                for (int j = 0; j < 100; j++)
                {
                    jped_data_1.pixel(i, j, silly::color(std::rand() % 256));
                }
            }
            std::filesystem::path temp = data_root;
            temp += std::to_string(index) + ".jpeg";
            if (jped_data_1.write(temp.string()))
            {
                std::cout << "create jpeg success" << std::endl;
            }
            else
            {
                std::cout << "create jpeg failed" << std::endl;
            }
            jped_data_1.release();
        }

        int a = 0;
        int b = 0;
        int c = 0;
    }

    SECTION("WRITE_JPEG")  // 写入 JPEG
    {
        // std::cout << "\r\n\r\n****************" << "WRITE_JPEG" << "****************" << std::endl;
        //// 创建空白图片并写入
        // jpeg_utils ju;
        // jpeg_data temp_jpeg_2 = ju.creat_empty(300, 300, 3, JCS_RGB);
        // std::filesystem::path data_root_3(DEFAULT_SU_DATA_DIR);
        // data_root_3+="/jpeg/empty_while.jpeg";
        // ju.write(data_root_3.string().c_str(), temp_jpeg_2);
        // temp_jpeg_2.release();
    }

    SECTION("SET_PIXEL_COLOR")  // 设置像素点颜色
    {
        // std::cout << "\r\n\r\n****************" << "SET_PIXEL_COLOR" << "****************" << std::endl;
        //// 改变某像素点坐标
        // jpeg_utils ju;
        // std::filesystem::path data_root_4(DEFAULT_SU_DATA_DIR);
        // data_root_4+="/jpeg/empty_while.jpeg";
        // jpeg_data temp_jpeg_8 = ju.read(data_root_4.string().c_str());

        // std::filesystem::path data_root_5(DEFAULT_SU_DATA_DIR);
        // data_root_5+="/jpeg/one_blace.jpeg";
        // int row = 50; // 第几行
        // int col = 30; // 第几列
        // jpeg_pixel jp(0,0,0);
        // temp_jpeg_8.set_pixel(row, col, jp);
        // ju.write(data_root_5.string().c_str(), temp_jpeg_8);
        // temp_jpeg_8.release();
    }
}

#endif  // SILLY_UTILS_TEST_JPEG_HPP
