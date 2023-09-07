//
// Created by dly on 2023/8/3.
//

#include "silly_geo_convert.h"


//#include <ogrsf_frmts.h>
//
//
//bool silly_geo_convert::shp_to_geojson(const char* shpFile, const char* geojsonFile)
//{
//    GDALAllRegister();
//
//    GDALDataset* shpDataset = (GDALDataset*)GDALOpenEx(shpFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
//    if (shpDataset == nullptr) {
//        std::cout << "无法打开SHP文件！" << std::endl;
//        return false;
//    }
//
//    GDALDriver* geojsonDriver = GetGDALDriverManager()->GetDriverByName("GeoJSON");
//    if (geojsonDriver == nullptr) {
//        std::cout << "无法创建GeoJSON驱动！" << std::endl;
//        GDALClose(shpDataset);
//        return false;
//    }
//
//    GDALDataset* geojsonDataset = geojsonDriver->CreateCopy(geojsonFile, shpDataset, FALSE, NULL, NULL, NULL);
//    if (geojsonDataset == nullptr) {
//        std::cout << "无法创建输出GeoJSON文件！" << std::endl;
//        GDALClose(shpDataset);
//        return false;
//    }
//
//    GDALClose(geojsonDataset);
//    GDALClose(shpDataset);
//
//    std::cout << "SHP文件成功转换为GeoJSON文件！" << std::endl;
//    return true;
//}
//
//bool silly_geo_convert::geojson_to_shp(const char* geojsonFile, const char* shpFile)
//{
//    GDALAllRegister();
//
//    GDALDataset* geojsonDataset = (GDALDataset*)GDALOpenEx(geojsonFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
//    if (geojsonDataset == nullptr) {
//        std::cout << "无法打开GeoJSON文件！" << std::endl;
//        return false;
//    }
//
//    GDALDriver* shpDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
//    if (shpDriver == nullptr) {
//        std::cout << "无法创建SHP驱动！" << std::endl;
//        GDALClose(geojsonDataset);
//        return false;
//    }
//
//    GDALDataset* shpDataset = shpDriver->CreateCopy(shpFile, geojsonDataset, FALSE, NULL, NULL, NULL);
//    if (shpDataset == nullptr) {
//        std::cout << "无法创建输出SHP文件！" << std::endl;
//        GDALClose(geojsonDataset);
//        return false;
//    }
//
//    GDALClose(shpDataset);
//    GDALClose(geojsonDataset);
//
//    std::cout << "GeoJSON文件成功转换为SHP文件！" << std::endl;
//    return true;
//}