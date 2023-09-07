//#pragma once
//
//#include "silly_geo_operation.h"
//#include <math.h>
//#include <ogr_spatialref.h>
//#include <gdal_priv.h>
//#include <ogrsf_frmts.h>
//#include <gdal_alg.h>
//#include <ogr_api.h>
//#include "marco.h"
//
//silly_point geo_operation::ring_to_center(silly_ring ring)
//{
//    double sum_x = 0.0;
//    double sum_y = 0.0;
//    for (const auto& point : ring.points) 
//    {
//        sum_x += point.lgtd;
//        sum_y += point.lttd;
//    }
//
//    double center_x = sum_x / ring.points.size();
//    double center_y = sum_y / ring.points.size();
//
//    silly_point center_point;
//    center_point.lgtd = center_x;
//    center_point.lttd = center_y;
//
//    return center_point;
//}
//
//
//double geo_operation::two_point_azimuth(silly_point p1, silly_point p2)
//{
//    double theta = atan2(p1.lgtd - p1.lgtd, (p1.lttd + 1) - p1.lttd) - atan2(p2.lgtd - p1.lgtd, p2.lttd - p1.lttd);
//    if (theta > SU_PI)
//    {
//        theta -= 2 * SU_PI;
//    }
//    if (theta < -SU_PI)
//    {
//        theta += 2 * SU_PI;
//    }
//    theta = -theta * 180.0 / SU_PI;
//    return theta;
//}
//
//
//bool geo_operation::shp_to_geojson(const char* shpFile, const char* geojsonFile)
//{
//    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	// 支持中文路径
//    GDALAllRegister();
//
//    GDALDataset* shpDataset = (GDALDataset*)GDALOpenEx(shpFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
//    if (shpDataset == nullptr) 
//    {
//        std::cout << "Unable to open SHP file  " << std::endl;
//        return false;
//    }
//
//    GDALDriver* geojsonDriver = GetGDALDriverManager()->GetDriverByName("GeoJSON");
//    if (geojsonDriver == nullptr) 
//    {
//        std::cout << "Unable to create GeoJSON driver  " << std::endl;
//        GDALClose(shpDataset);
//        return false;
//    }
//
//    GDALDataset* geojsonDataset = geojsonDriver->CreateCopy(geojsonFile, shpDataset, FALSE, NULL, NULL, NULL);
//    if (geojsonDataset == nullptr) 
//    {
//        std::cout << "Unable to create output GeoJSON file  " << std::endl;
//        GDALClose(shpDataset);
//        return false;
//    }
//
//    GDALClose(geojsonDataset);
//    GDALClose(shpDataset);
//
//    std::cout << "SHP file successfully converted to GeoJSON file  " << std::endl;
//    return true;
//}
//
//bool geo_operation::geojson_to_shp(const char* geojsonFile, const char* shpFile)
//{
//    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	// 支持中文路径
//    GDALAllRegister();
//
//    GDALDataset* geojsonDataset = (GDALDataset*)GDALOpenEx(geojsonFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
//    if (geojsonDataset == nullptr) 
//    {
//        std::cout << "Unable to open GeoJSON file " << std::endl;
//        return false;
//    }
//
//    GDALDriver* shpDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
//    if (shpDriver == nullptr) 
//    {
//        std::cout << "Unable to create SHP driver " << std::endl;
//        GDALClose(geojsonDataset);
//        return false;
//    }
//
//    GDALDataset* shpDataset = shpDriver->CreateCopy(shpFile, geojsonDataset, FALSE, NULL, NULL, NULL);
//    if (shpDataset == nullptr) 
//    {
//        std::cout << "Unable to create output SHP file " << std::endl;
//        GDALClose(geojsonDataset);
//        return false;
//    }
//
//    GDALClose(shpDataset);
//    GDALClose(geojsonDataset);
//
//    std::cout << "GeoJSON file successfully converted to SHP file " << std::endl;
//    return true;
//}