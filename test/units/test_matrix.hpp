#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>
#include "math/silly_matrix.h"

using namespace silly_math;


TEST_CASE("TestMatrix")
{
SECTION("convert_matrix")
{
    std::cout << "\r\n\r\n****************" << "矩阵类型转换" << "****************" << std::endl;
   /* std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
    data_root += "/jpeg/short_to_float_1.jpeg";
    matrix_2d<short> ma_2d_short = convertToMatrix(data_root.string());
    matrix_2d<float> ma_2d_float;

    matrix_tools::convert_matrix(ma_2d_short, ma_2d_float);

    std::filesystem::path data_root_2(DEFAULT_SU_DATA_DIR);
    data_root_2 += "/jpeg/short_to_float_2.jpeg";
    saveMatrixAsImage(ma_2d_float, data_root_2.string());
    ma_2d_short.release();
    ma_2d_float.release();*/

}

//SECTION("inter_nearest_resize")
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
//SECTION("bilinear_resize")
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
}
#endif