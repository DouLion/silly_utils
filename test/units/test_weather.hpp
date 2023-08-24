/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/1 10:04
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include "weather/geotiff_utils.h"

BOOST_AUTO_TEST_SUITE(Test)

BOOST_AUTO_TEST_CASE(readGeoTiff)
{
    std::cout << "\r\n\r\n****************" << "读取tif" << "****************" << std::endl;
    std::filesystem::path tif_data_path(DEFAULT_DATA_DIR);
    tif_data_path += "/tif/write2.tif";
    std::string t = "D:/1_wangyingjie/code/2_hill_shading/世界/地图/ETOPO_2022_v1_60s_N90W180_bed.tif";
    //std::string t = "D:/1_wangyingjie/code/2_hill_shading/HillShading/dem_data/ETOPO_2022_v1_15s_N60W030_bed.tif";
    std::filesystem::path word(t);

    matrix_2d<unsigned char> tif_matrix;
    //tif_data ti = geotiff_utils::readGeoTiff(tif_data_path.string().c_str());
    //tif_data ti = geotiff_utils::readGeoTiff(word.string().c_str());
    tif_data ti = geotiff_utils::readGeoTiffTile(word.string().c_str());

    std::cout << "-------- " << static_cast<float>(ti.tif_matrix2.at(1, 1)) << std::endl;


    std::filesystem::path tif_data_w(DEFAULT_DATA_DIR);
    tif_data_w += "/tif/word_1.tif";
    geotiff_utils::writeGeoTiffTile(tif_data_w.string().c_str(), ti);
    std::cout << "---------------------------------------" << std::endl;


    //for (size_t r = 0; r < ti.tif_height; ++r) {
    //    for (size_t c = 0; c < ti.tif_width; ++c) {
    //        std::cout << static_cast<float>(ti.tif_matrix2.at(r, c)) << " ";
    //    }
    //    std::cout << std::endl;
    //}
    //std::cout << "---------------------------------------" << std::endl;

    ti.tif_matrix2.destroy();
    tif_matrix.destroy();

}



BOOST_AUTO_TEST_SUITE_END()