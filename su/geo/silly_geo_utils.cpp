

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


// 将 silly_ring 转换为 OGRPolygon
OGRPolygon* createPolygonFromSillyRing(const silly_ring& ring)
{
    OGRPolygon* polygon = new OGRPolygon;
    OGRLinearRing* ringObj = new OGRLinearRing;

    for (const auto& point : ring.points) {
        ringObj->addPoint(point.lgtd, point.lttd);
    }

    polygon->addRingDirectly(ringObj);
    return polygon;
}


// 环OGRLinearRing对象，将其转换为silly_ring对象
silly_ring processRing(OGRLinearRing* ring)
{
    silly_ring result;
    int pointCount = ring->getNumPoints();
    for (int i = 0; i < pointCount; i++)
    {
        double x = ring->getX(i);
        double y = ring->getY(i);
        silly_point point(x, y);
        result.points.push_back(point);
    }
    return result;
}


// 单面OGRPolygon对象转换为silly_poly(多环:外环+内环)对象
void processPolygon(OGRPolygon* polygon, silly_poly& poly)
{
    // 处理OGRPolygon外环
    OGRLinearRing* outerRing = polygon->getExteriorRing();
    poly.outer_ring = processRing(outerRing);
    // 处理OGRPolygon内环
    int innerRingCount = polygon->getNumInteriorRings();
    for (int k = 0; k < innerRingCount; k++)
    {
        OGRLinearRing* ring = polygon->getInteriorRing(k);
        silly_ring innerRing = processRing(ring);
        poly.inner_rings.push_back(innerRing);
    }
}


//多面的OGRMultiPolygon对象转换为silly_multi_poly(多面)对象
void processMultiPolygon(OGRMultiPolygon* multiPolygon, silly_multi_poly& poly)
{
    int polygonCount = multiPolygon->getNumGeometries();
    for (int i = 0; i < polygonCount; i++)
    {
        silly_poly  tmp_poly;
        OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(multiPolygon->getGeometryRef(i));
        processPolygon(polygon, tmp_poly);
        poly.push_back(tmp_poly);
    }
}


// OGRLineString(线)类型转为silly_line(线)类型
void processLineString(OGRLineString* lineString, silly_line& line)
{
    int num_points = lineString->getNumPoints();
    for (int j = 0; j < num_points; j++)
    {
        double x = lineString->getX(j);
        double y = lineString->getY(j);
        silly_point point(x, y);
        line.push_back(point);
    }
}


std::vector<silly_point> geo_utils::read_vector_points(const char* File)
{
    std::vector<silly_point> points;
    GDALDataset* dataset = (GDALDataset*)GDALOpenEx(File, GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    if (dataset == nullptr)
    {
        std::cout << "Failed to open file! " << std::endl;
        return points;
    }
    // 获得数据集中图层数
    int layerCount = dataset->GetLayerCount();
    for (int i = 0; i < layerCount; i++)
    {
        OGRLayer* layer = dataset->GetLayer(i);
        layer->ResetReading();
        OGRFeature* feature;
        while ((feature = layer->GetNextFeature()) != nullptr)
        {
            OGRGeometry* geometry = feature->GetGeometryRef();
            if (geometry != nullptr)
            {
                OGRwkbGeometryType geomType = wkbFlatten(geometry->getGeometryType());
                // 判断一下类型
                if (geomType == wkbPoint || geomType == wkbPoint25D)
                {
                    OGRPoint* point = dynamic_cast<OGRPoint*>(geometry);
                    double x = point->getX();
                    double y = point->getY();
                    silly_point pt(x, y);
                    // 获取属性信息，这里属性名为 "adnm"
                    //const char* label = feature->GetFieldAsString("adnm");
                    //std::string name = label;
                    points.push_back(pt);
                }
                else
                {
                    std::cout << "The file data type cannot be processed temporarily: " << geomType << std::endl;
                }
            }
            OGRFeature::DestroyFeature(feature);
        }
    }

    GDALClose(dataset);
    return points;
}


std::vector<silly_line> geo_utils::read_vector_lines(const char* File)
{
    std::vector<silly_line> lines;
    GDALDataset* dataset = (GDALDataset*)GDALOpenEx(File, GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    if (dataset == nullptr)
    {
        std::cout << "Failed to open file! " << std::endl;
        return lines;
    }
    int layerCount = dataset->GetLayerCount();
    for (int i = 0; i < layerCount; i++)
    {
        OGRLayer* layer = dataset->GetLayer(i);
        layer->ResetReading();
        OGRFeature* feature;
        while ((feature = layer->GetNextFeature()) != nullptr)
        {
            silly_line line;
            OGRGeometry* geometry = feature->GetGeometryRef();
            if (geometry != nullptr)
            {
                OGRwkbGeometryType geomType = wkbFlatten(geometry->getGeometryType());
                // 单线
                if (geomType == wkbLineString || geomType == wkbLineString25D)
                {
                    OGRLineString* lineString = dynamic_cast<OGRLineString*>(geometry);
                    processLineString(lineString, line);
                    
                    // 获取属性信息，这里属性名为 "RVNM"，
                    //const char* label = feature->GetFieldAsString("RVNM");
                    //std::string name = label;
                    lines.push_back(line);
                }
                // 多线
                else if (geomType == wkbMultiLineString || geomType == wkbMultiLineString25D)
                {
                    OGRMultiLineString* multiLineString = dynamic_cast<OGRMultiLineString*>(geometry);
                    int num_multiLine = multiLineString->getNumGeometries();
                    for (int j = 0; j < num_multiLine; j++)
                    {
                        OGRLineString* lineString = dynamic_cast<OGRLineString*>(multiLineString->getGeometryRef(j));
                        silly_line subLine;
                        processLineString(lineString, subLine);
                        // 获取属性信息，这里属性名为 "RVNM"，
                        //const char* label = feature->GetFieldAsString("RVNM");
                        //std::string name = label;
                        lines.push_back(subLine);
                    }
                }
                else
                {
                    std::cout << "The file data type cannot be processed temporarily: " << geomType << std::endl;
                }
            }
            OGRFeature::DestroyFeature(feature);
        }
    }

    GDALClose(dataset);

    return lines;
}


std::vector<silly_multi_poly> geo_utils::read_vector_polys(const char* File)
{
    std::vector<silly_multi_poly> polygons;  // 存放读取的环数据

    // 打开GeoJSON文件
    GDALDataset* dataset = (GDALDataset*)GDALOpenEx(File, GDAL_OF_VECTOR, nullptr, nullptr, nullptr);
    if (dataset == nullptr)
    {
        // 处理文件打开失败的情况
        std::cout << "Failed to open GeoJson file! " << std::endl;
        return polygons;
    }
    // 获取图层数量
    int layerCount = dataset->GetLayerCount();
    for (int i = 0; i < layerCount; i++)
    {
        OGRLayer* layer = dataset->GetLayer(i);
        // 遍历图层中的要素
        layer->ResetReading();
        OGRFeature* feature;
        while ((feature = layer->GetNextFeature()) != nullptr)
        {
            silly_multi_poly m_poly;
            // 读取环数据
            OGRGeometry* geometry = feature->GetGeometryRef();
            auto type = geometry->getGeometryType();
            if (geometry != nullptr)
            {
                // 处理单面
                OGRwkbGeometryType geomType = wkbFlatten(geometry->getGeometryType());
                if (geomType == wkbPolygon || geomType == wkbPolygon25D)
                {
                    silly_poly tmp_poly;
                    OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geometry);
                    processPolygon(polygon, tmp_poly);
                    m_poly.push_back(tmp_poly);
                }
                // 处理多面
                else if (geomType == wkbMultiPolygon || geomType == wkbMultiPolygon25D)
                {
                    OGRMultiPolygon* multiPolygon = dynamic_cast<OGRMultiPolygon*>(geometry);
                    processMultiPolygon(multiPolygon, m_poly);
                }
                else
                {
                    std::cout << "The file data type cannot be processed temporarily: " << geomType << std::endl;
                }
                polygons.push_back(m_poly);

            }
            OGRFeature::DestroyFeature(feature);
        }
    }

    GDALClose(dataset);  // 关闭数据集
    return polygons;
}




std::vector<silly_ring> geo_utils::intersect_area(silly_ring ring_1, silly_ring ring_2)
{
    std::vector<silly_ring> intersecting_rings;

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

        // 获取多边形的内环
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

            // 获取多边形的内环
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
    OGRCleanupAll();
    return true;
}

bool geo_utils::check_shp_info(const std::string& shp_file, enum_geometry_types& type, std::map<std::string, std::string>& properties)
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
    type = (enum_geometry_types)wkbFlatten(poGeometry_r->getGeometryType());
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



