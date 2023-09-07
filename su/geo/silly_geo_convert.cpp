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
//        std::cout << "�޷���SHP�ļ���" << std::endl;
//        return false;
//    }
//
//    GDALDriver* geojsonDriver = GetGDALDriverManager()->GetDriverByName("GeoJSON");
//    if (geojsonDriver == nullptr) {
//        std::cout << "�޷�����GeoJSON������" << std::endl;
//        GDALClose(shpDataset);
//        return false;
//    }
//
//    GDALDataset* geojsonDataset = geojsonDriver->CreateCopy(geojsonFile, shpDataset, FALSE, NULL, NULL, NULL);
//    if (geojsonDataset == nullptr) {
//        std::cout << "�޷��������GeoJSON�ļ���" << std::endl;
//        GDALClose(shpDataset);
//        return false;
//    }
//
//    GDALClose(geojsonDataset);
//    GDALClose(shpDataset);
//
//    std::cout << "SHP�ļ��ɹ�ת��ΪGeoJSON�ļ���" << std::endl;
//    return true;
//}
//
//bool silly_geo_convert::geojson_to_shp(const char* geojsonFile, const char* shpFile)
//{
//    GDALAllRegister();
//
//    GDALDataset* geojsonDataset = (GDALDataset*)GDALOpenEx(geojsonFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
//    if (geojsonDataset == nullptr) {
//        std::cout << "�޷���GeoJSON�ļ���" << std::endl;
//        return false;
//    }
//
//    GDALDriver* shpDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
//    if (shpDriver == nullptr) {
//        std::cout << "�޷�����SHP������" << std::endl;
//        GDALClose(geojsonDataset);
//        return false;
//    }
//
//    GDALDataset* shpDataset = shpDriver->CreateCopy(shpFile, geojsonDataset, FALSE, NULL, NULL, NULL);
//    if (shpDataset == nullptr) {
//        std::cout << "�޷��������SHP�ļ���" << std::endl;
//        GDALClose(geojsonDataset);
//        return false;
//    }
//
//    GDALClose(shpDataset);
//    GDALClose(geojsonDataset);
//
//    std::cout << "GeoJSON�ļ��ɹ�ת��ΪSHP�ļ���" << std::endl;
//    return true;
//}