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

BOOST_AUTO_TEST_CASE(writeGeoTiff)
{
    std::cout << "\r\n\r\n****************" << "写入一个随机的tif" << "****************" << std::endl;
    std::string t = "D:/1_wangyingjie/code/2_hill_shading/世界/地图/ETOPO_2022_v1_60s_N90W180_bed.tif";
    std::filesystem::path word2(t);
    tif_data res_tif;
    bool status = true;
    // 打开影像数据
    TIFF* tiff = XTIFFOpen(word2.string().c_str(), "r ");
    if (tiff == nullptr)
    {
        std::cout << "打开影像数据失败 " << std::endl;
    }
    // 获取tif的参数属性
    bool get_data = geotiff_utils::get_tif_data(tiff, res_tif);
    if (!get_data)
    {
        XTIFFClose(tiff);
    }
    res_tif.tif_width = 20;
    res_tif.tif_height = 20;
    res_tif.tif_matrix2.create(res_tif.tif_height, res_tif.tif_width);
    for (int j = 0; j < res_tif.tif_height; j++) 
    {
        // 逐列增加 30
        for (int i = 0; i < res_tif.tif_width; i++) 
        {
            if (0 == i)
            {
                res_tif.tif_matrix2.at(j,i) = 0;
            }
            else
            {
                res_tif.tif_matrix2.at(j, i) = res_tif.tif_matrix2.at(j, i-1) + 10;
            }
        }
    }
    for (int j = 0; j < res_tif.tif_height; j++)
    {
        // 逐列增加 30
        for (int i = 0; i < res_tif.tif_width; i++)
        {
            std::cout << res_tif.tif_matrix2.at(j, i) << " ";
        }
        std::cout << std::endl;
    }
   
    std::filesystem::path tif_data_w(DEFAULT_DATA_DIR);
    tif_data_w += "/tif/word_2.tif";
    geotiff_utils::writeGeoTiff(tif_data_w.string().c_str(), res_tif);
    //std::cout << "---------------------------------------" << std::endl;


    res_tif.tif_matrix2.destroy();

}

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
    tif_data ti = geotiff_utils::readGeoTiff(word.string().c_str());
    //tif_data ti = geotiff_utils::readGeoTiffTile(word.string().c_str());

    std::cout << "-------- " << static_cast<float>(ti.tif_matrix2.at(1, 1)) << std::endl;

    std::cout << "---------------------------------------" << std::endl;


    ti.tif_matrix2.destroy();
    tif_matrix.destroy();

}



BOOST_AUTO_TEST_SUITE_END()