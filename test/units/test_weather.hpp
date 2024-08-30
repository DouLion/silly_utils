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

#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>
#include "weather/geotiff_utils.h"
#include "weather/grib_utils.h"
#include "image/png_utils.h"
#include "image/silly_color.h"
#include "weather/silly_tzx_grid.h"
#include "render/silly_grid_render.h"

//template <typename T>

//silly_image::png_data generatePNGFromMatrix(matrix_2d<double>& elevation, std::map<int, silly_color, std::greater<int>> colorMap)
//{
//    int height = elevation.row();
//    int width = elevation.col();
//    if (height == 0 || width == 0 )
//    {
//        std::cout << "The size of the elevation matrix and shadow matrix is not equal." << std::endl;
//        return silly_image::png_data();
//    }
//    // 创建空的PNG数据块
//    silly_image::png_data blockImage = silly_image::png_utils::create_empty(height, width, PNG_COLOR_TYPE_RGB_ALPHA);
//
//    // 遍历矩阵，根据阈值映射颜色
//    for (int r = 0; r < height; ++r)
//    {
//        for (int c = 0; c < width; ++c)
//        {
//            double valueInMM = elevation.at(r, c) * 1000;  // 转换为毫米
//            silly_color pixelColor;
//            for (const auto& [thres, pixel] :colorMap)
//            {
//                if (valueInMM >= thres)
//                {
//                    pixelColor = pixel;
//                    blockImage.set_pixel(r, c, pixelColor);  
//                    break;
//                }
//            }
//        }
//    }
//    return blockImage;
//}




// 比较函数，用于在 colorMap 中查找大于或等于 value 的第一个阈值
bool compareThreshold(const std::pair<int, silly_color>& entry, double value)
{
    return entry.first >= value;
}

silly_image::png_data generatePNGFromMatrix(matrix_2d<double>& elevation, std::map<int, silly_color, std::greater<int>> colorMap)
{
    int height = elevation.row();
    int width = elevation.col();
    if (height == 0 || width == 0)
    {
        std::cout << "The size of the elevation matrix and shadow matrix is not equal." << std::endl;
        return silly_image::png_data();
    }
    // 创建空的PNG数据块
    silly_image::png_data blockImage = silly_image::png_utils::create_empty(height, width, PNG_COLOR_TYPE_RGB_ALPHA);

    // 遍历矩阵，根据阈值映射颜色
    for (int r = 0; r < height; ++r)
    {
        for (int c = 0; c < width; ++c)
        {
            double valueInMM = elevation.at(r, c) *1000;  // 转换为毫米
            silly_color pixelColor;
            auto it = std::lower_bound(colorMap.begin(), colorMap.end(), valueInMM, compareThreshold);
            if (it != colorMap.end())
            {
                pixelColor = it->second;
                blockImage.set_pixel(r, c, pixelColor);
            }
        }
    }
    return blockImage;
}


TEST_CASE("Test")
{

SECTION("READ_RGRIB")
{
    std::cout << "\r\n\r\n****************"
              << "READ_RGRIB"
              << "****************" << std::endl;
    // std::filesystem::path grib_data_path(DEFAULT_SU_DATA_DIR);
    // grib_data_path += "/grib/2024060312_240.grib2";

    std::filesystem::path rgrib_data_path("/usr/local/mnt/grib_data_143/GRID/ECMWF/APCP/2024070920/2024070920_024.rgrid");
    std::filesystem::path grib_png("/usr/local/mnt/grib_data_143/GRID/ECMWF/APCP/2024070920/2024070920_024.png");
    
    //// 欧洲气象局图例
    //std::map<int, silly_color, std::greater<int>> colorMap;
    //colorMap[1] = silly_color(84, 77, 113, 175);
    //colorMap[5] = silly_color(87, 86, 124, 175);
    //colorMap[10] = silly_color(48, 120, 141, 175);
    //colorMap[20] = silly_color(22, 126, 128, 175);
    //colorMap[40] = silly_color(101, 145, 102, 175);
    //colorMap[100] = silly_color(249, 162, 193, 175);
    //std::vector<silly_val2color<float>> value_to_color_map = {{1.0f, colorMap[1]}, {5.0f, colorMap[5]}, {10.0f, colorMap[10]}, {20.0f, colorMap[20]}, {40.0f, colorMap[40]}, {100.0f, colorMap[100]}};


     silly_color color1;
     color1.from_hex_argb("AF544D71");
     silly_color color2;
     color2.from_hex_argb("AF57567C");
     silly_color color3;
     color3.from_hex_argb("AF30788D");
     silly_color color4;
     color4.from_hex_argb("AF167E80");
     silly_color color5;
     color5.from_hex_argb("AF659166");
     silly_color color6;
     color6.from_hex_argb("AFF9A2C1");

     std::vector<silly_val2color<float>> value_to_color_map = {{1.0f, color1}, {5.0f, color2}, {10.0f, color3}, {20.0f, color4}, {40.0f, color5}, {100.0f, color5}};

    // //  tzx 图例
    //silly_color color1;
    //color1.from_hex_argb("afa5f28f");
    //silly_color color2;
    //color2.from_hex_argb("af3da700");
    //silly_color color3;
    //color3.from_hex_argb("af61bbfd");
    //silly_color color4;
    //color4.from_hex_argb("af0001fb");
    //silly_color color5;
    //color5.from_hex_argb("affe00fe");
    //silly_color color6;
    //color6.from_hex_argb("af00006e");
    //silly_color color7;
    //color7.from_hex_argb("af9600b4");
    //silly_color color8;
    //color8.from_hex_argb("afad90f0");
    //std::vector<silly_val2color<float>> value_to_color_map = {{25.0f, color3}, {1.0f, color1}, {10.0f, color2}, {50.0f, color4}, {150.0f, color6}, {250.0f, color7}, {100.0f, color5}, {350.0f, color8}};

    silly_render_param<float> render_param;
    render_param.v2cs = value_to_color_map;
    render_param.sort();
    silly_tzx_grid stg;
    stg.read(rgrib_data_path.string());
    render_param.mtx = stg.grid;
    render_param.rect.left = 74.0;
    render_param.rect.top = 54.0;
    render_param.rect.right = 135.0;
    render_param.rect.bottom = 18.0;
    // 渲染器
    silly_grid_render<float> renderer;
    // 渲染
    renderer.geo_mc_render_greater(render_param);

    // 写入PNG文件（示例路径）

    silly_image::png_utils::write(grib_png.string().c_str(), render_param.pd);

    if (!silly_image::png_utils::write(grib_png.string().c_str(), render_param.pd))
    {
        std::cout << "Failed to write PNG file." << grib_png.string() << std::endl;
    }
    else
    {
        std::cout << "PNG image generated successfully." << grib_png.string() << std::endl;
    }
    render_param.pd.release();
    render_param.mtx.release();
}

SECTION("GRID_TO_RGRID")
{
    std::cout << "\r\n\r\n****************"
              << "GRID_TO_RGRID"
              << "****************" << std::endl;
    // std::filesystem::path grib_data_path(DEFAULT_SU_DATA_DIR);
    // grib_data_path += "/grib/2024060312_240.grib2";
    std::filesystem::path grib_data_path("/usr/local/mnt/grib_data_143/GRID/ECMWF/APCP/2024070420/2024070420_087.rgrid");
    std::filesystem::path rgrib_data_path("/usr/local/mnt/winshare/rgrid/EDMWF/2024062020_006.rgrib");
    int type = 0;
    std::vector<DMatrix> lists;
    grib_data::grib_utils gu;
    gu.read(grib_data_path.string(), lists, type);
    silly_tzx_grid stg;
    for (auto& l : lists)
    {
        stg.grid.cast_from(l);
        stg.row = l.row();
        stg.col = l.col();

        stg.left = gu.m_geo_info.left;
        stg.right = gu.m_geo_info.right;
        stg.top = gu.m_geo_info.top;
        stg.bottom = gu.m_geo_info.bottom;

        stg.xdelta = gu.m_geo_info.xstep;
        stg.ydelta = gu.m_geo_info.ystep;

        break;
    }

    // boost::timer tm1;  // 定义后计时开始
    // tm1.restart();     // 从新从这里开始计时
     for (int i = 0; i < 200; i++)
    {
         // 临时测试
         std::map<int, silly_color, std::greater<int>> colorMap;
         colorMap[1] = silly_color(165, 242, 143, 175);
         colorMap[10] = silly_color(61, 167, 0, 175);
         colorMap[25] = silly_color(97, 187, 253, 175);
         colorMap[50] = silly_color(0, 1, 251, 175);
         colorMap[100] = silly_color(254, 0, 254, 175);
         colorMap[150] = silly_color(0, 0, 110, 175);
         colorMap[250] = silly_color(150, 0, 180, 175);
         colorMap[350] = silly_color(173, 144, 240, 175);




         std::filesystem::path grib_png("/usr/local/mnt/winshare/rgrid/EDMWF/");
         char tempname[50];
         sprintf(tempname, "/%s_.png", grib_data_path.filename().stem().string().c_str());
         grib_png += tempname;
         for (auto& list : lists)
         {
             silly_image::png_data pngImage = generatePNGFromMatrix(list, colorMap);
             // 写入PNG文件（示例路径）
             silly_image::png_utils::write(grib_png.string().c_str(), pngImage);
             std::cout << "png:" << grib_png.string() << std::endl;
             pngImage.release();
             break;
         }
     }

    // std::cout << tm1.elapsed() << std::endl;  // 单位是秒
    // std::cout << "once:" << tm1.elapsed() / 200 << std::endl;

    //if (stg.save(rgrib_data_path.string()))
    //{
    //    std::cout << "success write: " << rgrib_data_path.string() << std::endl;
    //}
    stg.grid.release();

    for (auto& l : lists)
    {
        l.release();
    }
}















//SECTION("GRID_TO_RGRID")
//{
//     std::cout << "\r\n\r\n****************"<< "GRID_TO_RGRID" << "****************" << std::endl;
//    // std::filesystem::path grib_data_path(DEFAULT_SU_DATA_DIR);
//    // grib_data_path += "/grib/2024060312_240.grib2";
//     std::filesystem::path grib_data_path("/usr/local/mnt/winshare/grib2/ECMWF/APCP/2024062020/2024062020_006.grib2");
//     std::filesystem::path rgrib_data_path("/usr/local/mnt/winshare/rgrid/EDMWF/2024062020_006.rgrib");
//     int type = 0;
//     std::vector<DMatrix> lists;
//     grib_data::grib_utils gu;
//     gu.read(grib_data_path.string(), lists, type);
//     silly_tzx_grid stg;
//     for (auto& l : lists)
//    {
//         stg.grid = l;
//         stg.row = l.row();
//         stg.col = l.col();
//
//        stg.left = gu.m_geo_info.left;
//        stg.right = gu.m_geo_info.right;
//        stg.top = gu.m_geo_info.top;
//        stg.bottom = gu.m_geo_info.bottom;
//
//        stg.xdelta = gu.m_geo_info.xstep;
//        stg.ydelta = gu.m_geo_info.ystep;
//
//        break;
//    }
//
//    //// 临时测试
//    //    std::map<int, silly_color, std::greater<int>> colorMap;
//    //    colorMap[1] = silly_color(165, 242, 143, 175);
//    //    colorMap[10] = silly_color(61, 167, 0, 175);
//    //    colorMap[25] = silly_color(97, 187, 253, 175);
//    //    colorMap[50] = silly_color(0, 1, 251, 175);
//    //    colorMap[100] = silly_color(254, 0, 254, 175);
//    //    colorMap[150] = silly_color(0, 0, 110, 175);
//    //    colorMap[250] = silly_color(150, 0, 180, 175);
//    //    colorMap[350] = silly_color(173, 144, 240, 175);
//    //     std::filesystem::path grib_png("/usr/local/mnt/winshare/rgrid/EDMWF/");
//    //    char tempname[50];
//    //    sprintf(tempname, "/%s_.png", grib_data_path.filename().stem().string().c_str());
//    //    grib_png += tempname;
//    //    silly_image::png_data pngImage = generatePNGFromMatrix(stg.grid, colorMap);
//    //
//    //    // 写入PNG文件（示例路径）
//    //    if (!silly_image::png_utils::write(grib_png.string().c_str(), pngImage))
//    //    {
//    //        std::cout << "Failed to write PNG file." << grib_png.string() << std::endl;
//    //    }
//    //    else
//    //    {
//    //        std::cout << "PNG image generated successfully." << grib_png.string() << std::endl;
//    //    }
//    //// 临时测试
//
//     if (stg.save(rgrib_data_path.string()))
//     {
//        std::cout << "success write: " << rgrib_data_path.string() << std::endl;
//     }
//
//     stg.grid.destroy();
//    
//     for (auto& l : lists)
//    {
//         l.destroy();
//     }
//}





//SECTION("READ_GRID")
//{
//    std::cout << "\r\n\r\n****************"
//              << "READ_GRID"
//              << "****************" << std::endl;
//    // std::filesystem::path grib_data_path(DEFAULT_SU_DATA_DIR);
//    // grib_data_path += "/grib/2024060312_240.grib2";
//    std::filesystem::path grib_data_path("/usr/local/mnt/winshare/grib2/NOAA/APCP/2024062020/2024062020_006.grib2");
//    int type = 0;
//    std::vector<DMatrix> lists;
//    grib_data::grib_utils gu;
//    gu.read(grib_data_path.string(), lists, type);
//
//    // 颜色映射初始化
//    std::map<int, silly_color, std::greater<int>> colorMap;
//    colorMap[1] = silly_color(165, 242, 143, 175);
//    colorMap[10] = silly_color(61, 167, 0, 175);
//    colorMap[25] = silly_color(97, 187, 253, 175);
//    colorMap[50] = silly_color(0, 1, 251, 175);
//    colorMap[100] = silly_color(254, 0, 254, 175);
//    colorMap[150] = silly_color(0, 0, 110, 175);
//    colorMap[250] = silly_color(150, 0, 180, 175);
//    colorMap[350] = silly_color(173, 144, 240, 175);
//    int i = 0;
//    for (auto& list : lists)
//    {
//        std::filesystem::path grib_png("/usr/local/mnt/winshare/grib2/NOAA/APCP/2024062020/");
//        char tempname[50];
//        sprintf(tempname, "/%s_%02d.png", grib_data_path.filename().stem().string().c_str(), i);
//        grib_png += tempname;
//        i++;
//        silly_image::png_data pngImage = generatePNGFromMatrix(list, colorMap);
//
//        // 写入PNG文件（示例路径）
//        if (!silly_image::png_utils::write(grib_png.string().c_str(), pngImage))
//        {
//            std::cout << "Failed to write PNG file." << grib_png.string() << std::endl;
//        }
//        else
//        {
//            std::cout << "PNG image generated successfully." << grib_png.string() << std::endl;
//        }
//    }
//    // 生成PNG数据
//    for (auto l : lists)
//    {
//        l.destroy();
//    }
//
//}


//SECTION("writeGeoTiff")
//{
//    std::cout << "\r\n\r\n****************" << "写入一个随机的tif" << "****************" << std::endl;
//    std::string t = "D:/1_wangyingjie/code/2_hill_shading/世界/地图/ETOPO_2022_v1_60s_N90W180_bed.tif";
//    std::filesystem::path word2(t);
//    tif_data res_tif;
//    bool status = true;
//    // 打开影像数据
//    TIFF* tiff = XTIFFOpen(word2.string().c_str(), "r ");
//    if (tiff == nullptr)
//    {
//        std::cout << "打开影像数据失败 " << std::endl;
//    }
//    // 获取tif的参数属性
//    bool get_data = geotiff_utils::get_tif_data(tiff, res_tif);
//    if (!get_data)
//    {
//        XTIFFClose(tiff);
//    }
//    res_tif.tif_width = 20;
//    res_tif.tif_height = 20;
//    res_tif.tif_matrix2.create(res_tif.tif_height, res_tif.tif_width);
//    for (int j = 0; j < res_tif.tif_height; j++) 
//    {
//        // 逐列增加 30
//        for (int i = 0; i < res_tif.tif_width; i++) 
//        {
//            if (0 == i)
//            {
//                res_tif.tif_matrix2.at(j,i) = 0;
//            }
//            else
//            {
//                res_tif.tif_matrix2.at(j, i) = res_tif.tif_matrix2.at(j, i-1) + 10;
//            }
//        }
//    }
//    for (int j = 0; j < res_tif.tif_height; j++)
//    {
//        // 逐列增加 30
//        for (int i = 0; i < res_tif.tif_width; i++)
//        {
//            std::cout << res_tif.tif_matrix2.at(j, i) << " ";
//        }
//        std::cout << std::endl;
//    }
//   
//    std::filesystem::path tif_data_w(DEFAULT_SU_DATA_DIR);
//    tif_data_w += "/tif/word_3.tif";
//    //geotiff_utils::writeGeoTiff(tif_data_w.string().c_str(), res_tif);
//    geotiff_utils::writeFourChannelTiff(tif_data_w.string().c_str(), res_tif);
//    //std::cout << "---------------------------------------" << std::endl;
//
//
//    res_tif.tif_matrix2.destroy();
//
//}
//
//SECTION("readGeoTiff")
//{
//    std::cout << "\r\n\r\n****************" << "读取tif" << "****************" << std::endl;
//    std::filesystem::path tif_data_path(DEFAULT_SU_DATA_DIR);
//    tif_data_path += "/tif/write2.tif";
//    std::string t = "D:/1_wangyingjie/code/2_hill_shading/世界/地图/ETOPO_2022_v1_60s_N90W180_bed.tif";
//    //std::string t = "D:/1_wangyingjie/code/2_hill_shading/HillShading/dem_data/ETOPO_2022_v1_15s_N60W030_bed.tif";
//    std::filesystem::path word(t);
//
//    matrix_2d<unsigned char> tif_matrix;
//    //tif_data ti = geotiff_utils::readGeoTiff(tif_data_path.string().c_str());
//    tif_data ti = geotiff_utils::readGeoTiff(word.string().c_str());
//    //tif_data ti = geotiff_utils::readGeoTiffTile(word.string().c_str());
//
//    std::cout << "-------- " << static_cast<float>(ti.tif_matrix2.at(1, 1)) << std::endl;
//
//    std::cout << "---------------------------------------" << std::endl;
//
//
//    ti.tif_matrix2.destroy();
//    tif_matrix.destroy();
//
//}

//SECTION("READ_GRID2_DATA")
//{
//    std::cout << "\r\n\r\n****************" << "READ_GRID2_DATA" << "****************" << std::endl;
//    std::filesystem::path grib_data_path(DEFAULT_SU_DATA_DIR);
//    grib_data_path.append("Z_NWGD_C_BCWH_20230912103553_P_RFFC_SPCC-ER01_202309120800_02401.GRB2");
//    
//    int type = 0;
//    std::vector<DMatrix> list;
//    grib_data::grib_utils gu;
//    gu.read(grib_data_path.string(), list, type);
//    //tif_data ti = geotiff_utils::readGeoTiffTile(word.string().c_str());
//    for (auto l : list)
//    {
//        l.destroy();
//    }
//    
//    
//
//}
}

#endif