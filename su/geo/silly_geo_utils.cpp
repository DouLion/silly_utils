

#include "silly_geo_utils.h"
#include <math.h>

// GDAL.
#include "ogr_spatialref.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "ogr_api.h"

#include <polyclipping/clipper.hpp>
#include <encode/convert.hpp>
#include "marco.h"
using namespace ClipperLib;

silly_point geo_utils::ring_to_center(silly_ring ring)
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


double geo_utils::two_point_azimuth(silly_point from, silly_point to)
{
    double theta = atan2(to.lgtd - from.lgtd, to.lttd - from.lttd);
    theta = theta * 180.0 / SU_PI;
    return theta;
}




std::vector<silly_ring> geo_utils::read_shp_ring(const char* shp)
{
    std::vector<silly_ring> rings;
#if IS_WIN32

    // 打开 shp 文件
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(shp, GDAL_OF_VECTOR, nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        std::cout << "Failed to open SHP file " << std::endl;
        return rings;
    }

    // 获取图层数量
    int layerCount = dataset->GetLayerCount();

    // 遍历图层
    for (int i = 0; i < layerCount; i++)
    {
        OGRLayer* layer = dataset->GetLayer(i);

        // 遍历要素
        layer->ResetReading();
        OGRFeature* feature;
        while ((feature = layer->GetNextFeature()) != nullptr)
        {
            //// 获取属性值
            //std::string fieldValue = feature->GetFieldAsString("OBJECTID");
            //// 输出属性值
            //std::cout << "value: " << fieldValue << std::endl;
            
            // 获取几何对象
            OGRGeometry* geometry = feature->GetGeometryRef();
            if (geometry == nullptr)
            {
                continue;
            }
            // 创建一个新的 silly_ring 对象
            silly_ring outer_ring;
            OGRPolygon* polygon = static_cast<OGRPolygon*>(geometry);
            // 获取外环
            OGRLinearRing* outerRing = polygon->getExteriorRing();
            if (outerRing == nullptr)
            {
                continue;
            }
            // 遍历外环的点
            int numPoints = outerRing->getNumPoints();
            if (numPoints > 3)
            {
                for (int j = 0; j < numPoints; j++)
                {
                    OGRPoint point;
                    outerRing->getPoint(j, &point);

                    // 创建一个新的 silly_point 对象，存储点的坐标
                    silly_point sp(point.getX(), point.getY());

                    // 将点添加到 silly_ring 的 points 向量中
                    outer_ring.points.push_back(sp);
                }
                rings.push_back(outer_ring);
            }

            silly_ring inner_ring;
            // 获取内环数量
            int numInnerRings = polygon->getNumInteriorRings();
            if (numInnerRings > 3)
            {
                for (int k = 0; k < numInnerRings; k++)
                {
                    OGRLinearRing* innerRing = polygon->getInteriorRing(k);
                    if (innerRing == nullptr)
                    {
                        continue;
                    }

                    // 遍历内环的点
                    int numInnerPoints = innerRing->getNumPoints();
                    for (int l = 0; l < numInnerPoints; l++)
                    {
                        OGRPoint point;
                        innerRing->getPoint(l, &point);

                        // 创建一个新的 silly_point 对象，存储点的坐标
                        silly_point sp(point.getX(), point.getY());

                        // 将点添加到外环的 points 向量中
                        inner_ring.points.push_back(sp);
                    }
                }
                rings.push_back(inner_ring);
            }
            // 释放要素
            OGRFeature::DestroyFeature(feature);
        }
    }

    GDALClose(dataset);

#endif

    return rings;
}

std::vector<silly_ring> geo_utils::read_geojson(const char* geojsonFile)
{
    std::vector<silly_ring> rings;
#if IS_WIN32

    GDALDataset* dataset = (GDALDataset*)GDALOpenEx(geojsonFile, GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    if (dataset == nullptr) 
    {
        std::cout << "Failed to open GeoJSON file " << std::endl;
        return rings;
    }
    //获取GeoJSON文件的第一个图层
    OGRLayer* layer = dataset->GetLayer(0);
    if (layer == nullptr) 
    {
        std::cout << "Failed to retrieve layer from GeoJSON file " << std::endl;
        GDALClose(dataset);
        return rings;
    }
    OGRFeature* feature;
    layer->ResetReading();
    while ((feature = layer->GetNextFeature()) != nullptr) 
    {
        OGRGeometry* geometry = feature->GetGeometryRef();
        if (geometry != nullptr && geometry->getGeometryType() == wkbPolygon) 
        {
            OGRPolygon* polygon = (OGRPolygon*)geometry;
            int ringCount = polygon->getNumInteriorRings() + 1;  

            for (int i = 0; i < ringCount; i++) 
            {
                silly_ring ring;
                OGRLinearRing* linearRing = (i == 0) ? polygon->getExteriorRing() : polygon->getInteriorRing(i - 1);
                int pointCount = linearRing->getNumPoints();

                for (int j = 0; j < pointCount; j++) 
                {
                    OGRPoint point;
                    linearRing->getPoint(j, &point);
                    ring.points.push_back({ point.getX(), point.getY() });
                }

                ring.is_outer = (i == 0) ? 1 : 0;
                rings.push_back(ring);
            }
        }

        OGRFeature::DestroyFeature(feature);
    }
    GDALClose(dataset);

#endif
    return rings;
}



bool geo_utils::points_to_shp(std::vector<silly_point>& points, const char* shpFilePath, const char* outputShpFilePath)
{
#if IS_WIN32

    // 打开现有 shp 文件
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(shpFilePath, GDAL_OF_VECTOR | GDAL_OF_UPDATE, nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        // 处理文件打开失败的情况
        std::cout << "Failed to open shapefile." << std::endl;
        return false;
    }

    // 获取第一个图层
    OGRLayer* layer = dataset->GetLayer(0);
    if (layer == nullptr)
    {
        // 处理图层获取失败的情况
        std::cout << "Failed to get layer." << std::endl;
        GDALClose(dataset);
        return false;
    }

    // 创建新的输出 shp 文件
    GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
    GDALDataset* outputDataset = outDriver->Create(outputShpFilePath, 0, 0, 0, GDT_Unknown, nullptr);
    if (outputDataset == nullptr)
    {
        // 处理输出文件创建失败的情况
        std::cout << "Failed to create output shapefile." << std::endl;
        GDALClose(dataset);
        return false;
    }

    // 创建新的图层
    OGRLayer* outputLayer = outputDataset->CreateLayer("points", layer->GetSpatialRef(), wkbPoint, nullptr);
    if (outputLayer == nullptr)
    {
        // 处理图层创建失败的情况
        std::cout << "Failed to create output layer." << std::endl;
        GDALClose(dataset);
        GDALClose(outputDataset);
        return false;
    }

    // 定义并创建字段
    OGRFieldDefn fieldSize("Size", OFTReal);
    if (outputLayer->CreateField(&fieldSize) != OGRERR_NONE)
    {
        // 处理字段创建失败的情况
        std::cout << "Failed to create size field." << std::endl;
        GDALClose(dataset);
        GDALClose(outputDataset);
        return false;
    }

    OGRFieldDefn fieldColor("Color", OFTString);
    if (outputLayer->CreateField(&fieldColor) != OGRERR_NONE)
    {
        // 处理字段创建失败的情况
        std::cout << "Failed to create color field." << std::endl;
        GDALClose(dataset);
        GDALClose(outputDataset);
        return false;
    }

    // 创建要素并进行设置
    for (const auto& point : points)
    {
        OGRFeature* feature = OGRFeature::CreateFeature(outputLayer->GetLayerDefn());

        // 创建点对象
        OGRPoint ogrPoint;
        ogrPoint.setX(point.lgtd);
        ogrPoint.setY(point.lttd);

        // 设置点要素的几何对象
        feature->SetGeometry(&ogrPoint);


        // 将要素添加到图层
        if (outputLayer->CreateFeature(feature) != OGRERR_NONE)
        {
            // 处理要素添加失败的情况
            std::cout << "Failed to add feature." << std::endl;
            OGRFeature::DestroyFeature(feature);
            GDALClose(dataset);
            GDALClose(outputDataset);
            return false;
        }

        // 释放要素
        OGRFeature::DestroyFeature(feature);
    }

    // 关闭数据集
    GDALClose(dataset);
    GDALClose(outputDataset);

    std::cout << "Points added to shapefile and saved successfully." << std::endl;
#endif

    return true;
}

// 将 silly_ring 转换为 OGRPolygon
OGRPolygon* createPolygonFromSillyRing(const silly_ring& ring) 
{
    OGRPolygon* polygon = new OGRPolygon;
#if IS_WIN32
    OGRLinearRing* ringObj = new OGRLinearRing;

    for (const auto& point : ring.points) {
        ringObj->addPoint(point.lgtd, point.lttd);
    }

    polygon->addRingDirectly(ringObj);
#endif
    return polygon;
}

/// OGRPolygon类型转silly_ring
silly_ring convertPolygonToSillyRing(OGRPolygon* polygon) 
{
    silly_ring ring;

    // 获取多边形的外环
    OGRLinearRing* exteriorRing = polygon->getExteriorRing();

    // 获取外环的点数
    int numPoints = exteriorRing->getNumPoints();

    // 将外环的点转换为 silly_point 类型，并添加到 silly_ring 中
    for (int i = 0; i < numPoints; i++) {
        OGRPoint point;
        exteriorRing->getPoint(i, &point);
        ring.points.push_back(silly_point(point.getX(), point.getY()));
    }

    //// 获取多边形的内环（如果存在），并将内环的点转换为 silly_point 类型，并添加到 silly_ring 中
    int numInteriorRings = polygon->getNumInteriorRings();
    for (int j = 0; j < numInteriorRings; j++) 
    {
        OGRLinearRing* interiorRing = polygon->getInteriorRing(j);
        int numInteriorPoints = interiorRing->getNumPoints();
        for (int k = 0; k < numInteriorPoints; k++) 
        {
            OGRPoint interiorPoint;
            interiorRing->getPoint(k, &interiorPoint);
            ring.points.push_back(silly_point(interiorPoint.getX(), interiorPoint.getY()));
        }
    }

    return ring;
}


std::vector<silly_ring> geo_utils::intersect_area(silly_ring ring_1, silly_ring ring_2)
{
    std::vector<silly_ring> intersecting_rings;
#if IS_WIN32
    // 创建 OGRPolygon 对象
    OGRPolygon* poly1 = createPolygonFromSillyRing(ring_1);
    OGRPolygon* poly2 = createPolygonFromSillyRing(ring_2);

    // 判断两个 OGRPolygon 是否相交
    if (!poly1->Intersects(poly2))
    {
        // 不相交，返回空的vector
        delete poly1;
        delete poly2;
        return intersecting_rings;
    }

    // 计算相交区域
    OGRGeometry* intersection = poly1->Intersection(poly2);

    // 处理不同几何类型的情况
    OGRwkbGeometryType geometryType = intersection->getGeometryType();
    switch (geometryType)
    {
        //单环多边形
    case wkbPolygon:
    case wkbPolygon25D:
    {
        OGRPolygon* intersectingPolygon = dynamic_cast<OGRPolygon*>(intersection);

        // 获取多边形的外环
        OGRLinearRing* ring = intersectingPolygon->getExteriorRing();
        int numPoints = ring->getNumPoints();
        silly_ring intersecting_ring;
        for (int i = 0; i < numPoints; i++)
        {
            OGRPoint point;
            ring->getPoint(i, &point);
            intersecting_ring.points.push_back(silly_point(point.getX(), point.getY()));
        }

        // 获取多边形的内环（如果存在）
        int numInteriorRings = intersectingPolygon->getNumInteriorRings();
        for (int j = 0; j < numInteriorRings; j++)
        {
            OGRLinearRing* interiorRing = intersectingPolygon->getInteriorRing(j);
            int numInteriorPoints = interiorRing->getNumPoints();
            for (int k = 0; k < numInteriorPoints; k++)
            {
                OGRPoint interiorPoint;
                interiorRing->getPoint(k, &interiorPoint);
                intersecting_ring.points.push_back(silly_point(interiorPoint.getX(), interiorPoint.getY()));
            }
        }

        intersecting_rings.push_back(intersecting_ring);
        delete intersectingPolygon;
        break;
    }
    // 多个多边形
    case wkbMultiPolygon:
    case wkbMultiPolygon25D:
    {
        OGRMultiPolygon* intersectingMultiPolygon = dynamic_cast<OGRMultiPolygon*>(intersection);

        // 获取多边形集合中的各个多边形
        int numPolygons = intersectingMultiPolygon->getNumGeometries();
        for (int i = 0; i < numPolygons; i++)
        {
            OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(intersectingMultiPolygon->getGeometryRef(i));
            silly_ring intersecting_ring;

            // 获取多边形的外环
            OGRLinearRing* ring = polygon->getExteriorRing();
            int numPoints = ring->getNumPoints();
            for (int j = 0; j < numPoints; j++)
            {
                OGRPoint point;
                ring->getPoint(j, &point);
                intersecting_ring.points.push_back(silly_point(point.getX(), point.getY()));
            }

            // 获取多边形的内环（如果存在）
            int numInteriorRings = polygon->getNumInteriorRings();
            for (int k = 0; k < numInteriorRings; k++)
            {
                OGRLinearRing* interiorRing = polygon->getInteriorRing(k);
                int numInteriorPoints = interiorRing->getNumPoints();
                for (int m = 0; m < numInteriorPoints; m++)
                {
                    OGRPoint interiorPoint;
                    interiorRing->getPoint(m, &interiorPoint);
                    intersecting_ring.points.push_back(silly_point(interiorPoint.getX(), interiorPoint.getY()));
                }
            }

            intersecting_rings.push_back(intersecting_ring);
        }

        delete intersectingMultiPolygon;
        break;
    }
    // 处理其他几何类型的情况
    default:
        break;
    }

    delete poly1;
    delete poly2;

#endif
    return intersecting_rings;
}

void geo_utils::init_gdal_env()
{
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    OGRRegisterAll();
    CPLSetConfigOption("SHAPE_ENCODING", "");
}

void geo_utils::destory_gdal_env()
{
    OGRCleanupAll();
}

bool geo_utils::is_valid_shp(const std::string& shp_file)
{
    GDALDataset* poDSr = (GDALDataset*)GDALOpenEx(shp_file.c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr);
    if (nullptr == poDSr)
    {
        return false;
    }
    GDALClose(poDSr);
    return true;
}

bool geo_utils::check_shp_info(const std::string& shp_file, int& type, std::map<std::string, std::string>& properties)
{
    bool status = false;
    std::map<std::string, std::string> result;

    GDALDataset* poDSr = (GDALDataset*)GDALOpenEx(shp_file.c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, NULL, NULL, NULL);
    if (NULL == poDSr)
    {
        return status;
    }
    OGRLayer* poLayer_r = poDSr->GetLayer(0);
    if (poLayer_r == NULL)
    {
        GDALClose(poDSr);
        return status;
    }
    OGRFeature* pFeature_r = NULL;
    //获取Feature特征
    pFeature_r = poLayer_r->GetNextFeature();
    if (NULL == pFeature_r)
    {
        GDALClose(poDSr);
        return status;
    }

    //获取要素的几何形状
    OGRGeometry* poGeometry_r = pFeature_r->GetGeometryRef();
    type = wkbFlatten(poGeometry_r->getGeometryType());
    if (wkbUnknown == type)
    {
        GDALClose(poDSr);
        return status;
    }
    // 1 wkbPoint 点
    // 2 wkbLineString 线
    // 3 wkbPolygon 面
    // 4 wkbMultiPoint  多点
    // 5 wkbMultiLineString  多线
    // 6 wkbMultiPolygon  多面
  
    //循环特征域
    int fieldCnt = pFeature_r->GetFieldCount();
    for (int i = 0; i < fieldCnt; i++)
    {
        OGRFieldDefn* def = pFeature_r->GetFieldDefnRef(i);
        OGRFieldType type = def->GetType();
        std::string field_name = def->GetNameRef();
        if (!silly_code::IsUTF8((uint8_t*)field_name.c_str(), field_name.size()))
        {
            field_name = silly_conv::GBK2UTF8(field_name);
        }

        std::string field_type = "";
        switch (type)
        {
        case OFTInteger:
            field_type = "OFTInteger";
            break;
        case OFTIntegerList:
            field_type = "OFTIntegerList";
            break;
        case OFTReal:
            field_type = "OFTReal";
            break;
        case OFTRealList:
            field_type = "OFTRealList";
            break;
        case OFTString:
            field_type = "OFTString";
            break;
        case OFTStringList:
            field_type = "OFTStringList";
            break;
#if _DEBUG
            //弃用 deprecated
        case OFTWideString:
            field_type = "Deprecated:OFTWideString";

            break;
        case OFTWideStringList:
            field_type = "Deprecated:OFTWideStringList";
            break;
#endif
        case OFTBinary:
            field_type = "OFTBinary";
            break;
        case OFTDate:
            field_type = "OFTDate";
            break;
        case OFTTime:
            field_type = "OFTTime";
            break;
        case OFTDateTime:
            field_type = "OFTDateTime";
            break;
        case OFTInteger64:
            field_type = "OFTInteger64";
            break;
        case OFTInteger64List:
            field_type = "OFTInteger64List";
            break;
        }
        properties[field_name] = field_name;
    }

   
    GDALClose(poDSr);
    status = true;
    return status;

}
