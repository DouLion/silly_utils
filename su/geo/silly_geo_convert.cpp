//
// Created by dly on 2023/8/3.
//

#include "silly_geo_convert.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"

bool silly_geo_convert::shp_to_geojson(const char* shpFile, const char* geojsonFile)
{
    GDALDataset* shpDataset = (GDALDataset*)GDALOpenEx(shpFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (shpDataset == nullptr)
    {
        std::cout << "Unable to open SHP file  " << std::endl;
        return false;
    }

    GDALDriver* geojsonDriver = GetGDALDriverManager()->GetDriverByName("GeoJSON");
    if (geojsonDriver == nullptr)
    {
        std::cout << "Unable to create GeoJSON driver  " << std::endl;
        GDALClose(shpDataset);
        return false;
    }

    GDALDataset* geojsonDataset = geojsonDriver->CreateCopy(geojsonFile, shpDataset, FALSE, NULL, NULL, NULL);
    if (geojsonDataset == nullptr)
    {
        std::cout << "Unable to create output GeoJSON file  " << std::endl;
        GDALClose(shpDataset);
        return false;
    }

    GDALClose(geojsonDataset);
    GDALClose(shpDataset);
    std::cout << "SHP file successfully converted to GeoJSON file  " << std::endl;
    return true;
}

bool silly_geo_convert::geojson_to_shp(const char* geojsonFile, const char* shpFile)
{
    GDALDataset* geojsonDataset = (GDALDataset*)GDALOpenEx(geojsonFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
    if (geojsonDataset == nullptr)
    {
        std::cout << "Unable to open GeoJSON file " << std::endl;
        return false;
    }

    GDALDriver* shpDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    if (shpDriver == nullptr)
    {
        std::cout << "Unable to create SHP driver " << std::endl;
        GDALClose(geojsonDataset);
        return false;
    }

    GDALDataset* shpDataset = shpDriver->CreateCopy(shpFile, geojsonDataset, FALSE, NULL, NULL, NULL);
    if (shpDataset == nullptr)
    {
        std::cout << "Unable to create output SHP file " << std::endl;
        GDALClose(geojsonDataset);
        return false;
    }

    GDALClose(shpDataset);
    GDALClose(geojsonDataset);
    std::cout << "GeoJSON file successfully converted to SHP file " << std::endl;
    return true;
}