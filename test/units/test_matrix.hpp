#define BOOST_TEST_INCLUDED
#include "math/silly_matrix.h"
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <iostream>
#include <random>



using namespace silly_math;
//
//matrix_2d<short> convertToMatrix(const std::string& imagePath)
//{
//    // 读取彩色图像
//    cv::Mat image = cv::imread(imagePath);
//
//    if (image.empty())
//    {
//        std::cout << "Failed to read image" << std::endl;
//        return matrix_2d<short>();
//    }
//
//    // 将彩色图像转换为灰度图像
//    cv::Mat grayImage;
//    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
//
//    // 将灰度图像数据存储到 matrix_2d<short> 对象中
//    int rows = grayImage.rows;
//    int cols = grayImage.cols;
//    matrix_2d<short> ma_2d_short;
//    ma_2d_short.create(rows, cols, true);
//
//    for (int i = 0; i < rows; i++)
//    {
//        for (int j = 0; j < cols; j++)
//        {
//            ma_2d_short.at(i, j) = grayImage.at<uchar>(i, j);
//        }
//    }
//
//    return ma_2d_short;
//}
//template<typename T>
//void saveMatrixAsImage(matrix_2d<T>& ma_2d_short, const std::string& outputPath)
//{
//    int rows = ma_2d_short.row();
//    int cols = ma_2d_short.col();
//    cv::Mat grayImage(rows, cols, CV_8U);
//
//    for (int i = 0; i < rows; i++)
//    {
//        for (int j = 0; j < cols; j++)
//        {
//            grayImage.at<uchar>(i, j) = static_cast<uchar>(ma_2d_short.at(i, j));
//        }
//    }
//
//    cv::imwrite(outputPath, grayImage);
//}

BOOST_AUTO_TEST_SUITE(Test)

BOOST_AUTO_TEST_CASE(convert_matrix)
{
    std::cout << "\r\n\r\n****************" << "矩阵类型转换" << "****************" << std::endl;
    std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
    data_root += "/jpeg/short_to_float_1.jpeg";
    matrix_2d<short> ma_2d_short = convertToMatrix(data_root.string());
    matrix_2d<float> ma_2d_float;

    matrix_tools::convert_matrix(ma_2d_short, ma_2d_float);

    std::filesystem::path data_root_2(DEFAULT_SU_DATA_DIR);
    data_root_2 += "/jpeg/short_to_float_2.jpeg";
    saveMatrixAsImage(ma_2d_float, data_root_2.string());
    ma_2d_short.destroy();
    ma_2d_float.destroy();

}

//BOOST_AUTO_TEST_CASE(inter_nearest_resize)
//{
//	std::cout << "\r\n\r\n****************" << "最临近插值" << "****************" << std::endl;
//    std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
//    std::filesystem::path data_root_2(DEFAULT_SU_DATA_DIR);
//    data_root += "/jpeg/color_1.jpeg";
//
//    //matrix_2d<short> ma_2d_short = convertToMatrix(data_root.string());
//    //int dst_row = ma_2d_short.row()*2;
//    //int dst_col = ma_2d_short.col()*2;
//    //matrix_2d<short> res;
//
//    matrix_2d<short> ma_2d_short;
//    ma_2d_short.create(2, 2, true);
//    ma_2d_short.at(0, 0) = 10;
//    ma_2d_short.at(0, 1) = 60;
//    ma_2d_short.at(1, 0) = 110;
//    ma_2d_short.at(1, 1) = 160;
//
//    //matrix_2d<short> ma_2d_short;
//    //ma_2d_short.create(2, 2, true);
//    //float value = 10.0;
//    //for (int i = 0; i < ma_2d_short.row(); i++)
//    //{
//    //    for (int j = 0; j < ma_2d_short.col(); j++)
//    //    {
//    //        if (j == 0) {
//    //            ma_2d_short.at(i, j) = value;
//    //        }
//    //        else {
//    //            ma_2d_short.at(i, j) = ma_2d_short.at(i, j - 1) + 100;
//    //        }
//    //    }
//    //}
//    int dst_row = ma_2d_short.row() * 100;
//    int dst_col = ma_2d_short.col() * 100;
//    matrix_2d<short> res;
//
//    matrix_tools mt;
//    silly_math::InterpolationFlags constant = silly_math::INTER_NEAREST;
//    mt.resize(ma_2d_short, res, dst_row, dst_col, constant);
//    data_root_2 += "/jpeg/s_INTER_NEAREST_8.jpeg";
//    saveMatrixAsImage(res, data_root_2.string());
//
//    ma_2d_short.destroy();
//    res.destroy();
//
//}
//
//
//
//BOOST_AUTO_TEST_CASE(bilinear_resize)
//{
//	std::cout << "\r\n\r\n****************" << "二次性插值" << "****************" << std::endl;
//    std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
//    std::filesystem::path data_root_2(DEFAULT_SU_DATA_DIR);
//
//    data_root += "/jpeg/color_1.jpeg";
//
//    //matrix_2d<short> ma_2d_short = convertToMatrix(data_root.string());
//    //int dst_row = ma_2d_short.row() * 3;
//    //int dst_col = ma_2d_short.col() * 3;
//    //matrix_2d<short> res;
//
//    matrix_2d<float> ma_2d_short;
//
//    ma_2d_short.create(2, 2, true);
//    ma_2d_short.at(0, 0) = 10;
//    ma_2d_short.at(0, 1) = 60;
//    ma_2d_short.at(1, 0) = 110;
//    ma_2d_short.at(1, 1) = 160;
//
//
//    //ma_2d_short.create(2, 2, true);
//    //float value = 10.0;
//    //for (int i = 0; i < ma_2d_short.row(); i++)
//    //{
//    //    for (int j = 0; j < ma_2d_short.col(); j++)
//    //    {
//    //        if (j == 0) {
//    //            ma_2d_short.at(i, j) = value;
//    //        }
//    //        else {
//    //            ma_2d_short.at(i, j) = ma_2d_short.at(i, j - 1) + 100;
//    //        }
//    //    }
//    //}
//    int dst_row = ma_2d_short.row() * 100;
//    int dst_col = ma_2d_short.col() * 100;
//    matrix_2d<float> res;
//
//    matrix_tools mt;
//    silly_math::InterpolationFlags my_var = silly_math::INTER_LINEAR;
//    mt.resize(ma_2d_short, res, dst_row, dst_col, my_var);
//    data_root_2 += "/jpeg/s_INTER_LINEAR_8.jpeg";
//    saveMatrixAsImage(res, data_root_2.string());
//
//    ma_2d_short.destroy();
//    res.destroy();
//}

BOOST_AUTO_TEST_SUITE_END()