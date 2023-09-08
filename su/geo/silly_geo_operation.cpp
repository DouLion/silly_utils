

#include "silly_geo_operation.h"
#include <math.h>
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"


silly_point geo_operation::ring_to_center(silly_ring ring)
{

    double sumX = 0;
    double sumY = 0;
    double area = 0;

    // 计算面积和形心横纵坐标的分子部分
    for (size_t i = 0; i < ring.points.size(); i++)
    {
        silly_point currentPoint = ring.points[i];
        silly_point nextPoint = ring.points[(i + 1) % ring.points.size()];

        double product = (currentPoint.lgtd * nextPoint.lttd) - (nextPoint.lgtd * currentPoint.lttd);
        area += product;

        sumX += (currentPoint.lgtd + nextPoint.lgtd) * product;
        sumY += (currentPoint.lttd + nextPoint.lttd) * product;
    }

    area /= 2;
    area < 0 ? -area : area;
    // 形心坐标的分母部分
    double denominator = 1 / (6 * area);

    // 计算形心点的横纵坐标
    double center_x = sumX * denominator;
    double center_y = sumY * denominator;

    silly_point center_point;
    center_point.lgtd = center_x;
    center_point.lttd = center_y;

    return center_point;
}


double geo_operation::two_point_azimuth(silly_point p1, silly_point p2)
{
    double theta = atan2(p1.lgtd - p1.lgtd, (p1.lttd + 1) - p1.lttd) - atan2(p2.lgtd - p1.lgtd, p2.lttd - p1.lttd);
    if (theta > UM_PI)
    {
        theta -= 2 * UM_PI;
    }
    if (theta < -UM_PI)
    {
        theta += 2 * UM_PI;
    }
    theta = -theta * 180.0 / UM_PI;
    return theta;
}


bool geo_operation::shp_to_geojson(const char* shpFile, const char* geojsonFile)
{
    //GDALAllRegister();
    //// 支持中文路径
    //CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	
    //// 属性表字段支持中文
    //CPLSetConfigOption("SHAPE_ENCODING", "");
    //GDALDataset* shpDataset = (GDALDataset*)GDALOpenEx(shpFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
    //if (shpDataset == nullptr)
    //{
    //    std::cout << "Unable to open SHP file  " << std::endl;
    //    return false;
    //}

    //GDALDriver* geojsonDriver = GetGDALDriverManager()->GetDriverByName("GeoJSON");
    //if (geojsonDriver == nullptr)
    //{
    //    std::cout << "Unable to create GeoJSON driver  " << std::endl;
    //    GDALClose(shpDataset);
    //    return false;
    //}

    //GDALDataset* geojsonDataset = geojsonDriver->CreateCopy(geojsonFile, shpDataset, FALSE, NULL, NULL, NULL);
    //if (geojsonDataset == nullptr)
    //{
    //    std::cout << "Unable to create output GeoJSON file  " << std::endl;
    //    GDALClose(shpDataset);
    //    return false;
    //}

    //GDALClose(geojsonDataset);
    //GDALClose(shpDataset);

    std::cout << "SHP file successfully converted to GeoJSON file  " << std::endl;
    return true;
}

bool geo_operation::geojson_to_shp(const char* geojsonFile, const char* shpFile)
{
    //GDALAllRegister();
    //// 支持中文路径
    //CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    //// 属性表字段支持中文
    //CPLSetConfigOption("SHAPE_ENCODING", "");

    //GDALDataset* geojsonDataset = (GDALDataset*)GDALOpenEx(geojsonFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
    //if (geojsonDataset == nullptr)
    //{
    //    std::cout << "Unable to open GeoJSON file " << std::endl;
    //    return false;
    //}

    //GDALDriver* shpDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    //if (shpDriver == nullptr)
    //{
    //    std::cout << "Unable to create SHP driver " << std::endl;
    //    GDALClose(geojsonDataset);
    //    return false;
    //}

    //GDALDataset* shpDataset = shpDriver->CreateCopy(shpFile, geojsonDataset, FALSE, NULL, NULL, NULL);
    //if (shpDataset == nullptr)
    //{
    //    std::cout << "Unable to create output SHP file " << std::endl;
    //    GDALClose(geojsonDataset);
    //    return false;
    //}

    //GDALClose(shpDataset);
    //GDALClose(geojsonDataset);

    std::cout << "GeoJSON file successfully converted to SHP file " << std::endl;
    return true;
}