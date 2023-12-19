

#include "silly_geo_utils.h"
#include <math.h>
#include <filesystem>

// GDAL.
#include "ogr_spatialref.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "ogr_api.h"

#include <polyclipping/clipper.hpp>
#include <encode/convert.hpp>
#include "su_marco.h"


using namespace ClipperLib;

silly_point geo_utils::ring_to_center(silly_ring ring)
{
    silly_point center_point;
    double sumX = 0;
    double sumY = 0;
    double area = 0;

    // 计算面积和形心横纵坐标的分子部分
    for (size_t i = 0; i < ring.points.size() -1 ; i++)
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

    //silly_point center_point;
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
OGRPolygon* convertPolygonFromSillyRing(const silly_ring& ring)
{
    OGRPolygon* polygon = new OGRPolygon;
    OGRLinearRing* ringObj = new OGRLinearRing;

    for (const auto& point : ring.points) {
        ringObj->addPoint(point.lgtd, point.lttd);
    }

    polygon->addRingDirectly(ringObj);
    return polygon;
}


// 环OGRLinearRing对象，将其转换为silly_ring对象  (环)
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


// 将 OGRPoint(单点) 转换为 silly_point(单点) 类型
silly_point convertSillyPointFromOGRPoint(OGRPoint* ogrPoint)
{
    double x = ogrPoint->getX();
    double y = ogrPoint->getY();
    return silly_point(x, y);
}
bool processPoint(OGRPoint* ogrPoint, silly_point& Point)
{
    Point.lgtd = ogrPoint->getX();
    Point.lttd = ogrPoint->getY();
    return true;
}


// 将 silly_point(单点) 转换为 OGRPoint(单点) 类型
OGRPoint* convertOGRPointFromSillyPoint(const silly_point& point)
{
    OGRPoint* ogrPoint = new OGRPoint(point.lgtd, point.lttd);
    return ogrPoint;
}

// 将 OGRMultiPoint(多点) 转换为 silly_multi_point(多点) 类型
bool processMultiPoint(OGRMultiPoint* ogrMultiPoint, silly_multi_point& mulitPoint)
{
    int pointCount = ogrMultiPoint->getNumGeometries();
    for (int i = 0; i < pointCount; i++)
    {
        OGRPoint* point = dynamic_cast<OGRPoint*>(ogrMultiPoint->getGeometryRef(i));
        if (point != nullptr)
        {
            silly_point sillyPoint = convertSillyPointFromOGRPoint(point);
            mulitPoint.push_back(sillyPoint);
        }
    }
    return true;
}

// 将 silly_multi_point(多点) 转换为 OGRMultiPoint(多点) 类型
OGRMultiPoint* convertOGRMultiPointFromMultiPoint(const silly_multi_point& mulitPoint)
{
    OGRMultiPoint* orgMultiPoint = new OGRMultiPoint;
    for (const silly_point& point : mulitPoint)
    {
        OGRPoint* ogrPoint = convertOGRPointFromSillyPoint(point);
        orgMultiPoint->addGeometryDirectly(ogrPoint);
    }
    return orgMultiPoint;
}

// OGRLineString(线)类型转为silly_line(线)类型
bool processLineString(OGRLineString* lineString, silly_line& line)
{
    int num_points = lineString->getNumPoints();
    for (int j = 0; j < num_points; j++)
    {
        double x = lineString->getX(j);
        double y = lineString->getY(j);
        silly_point point(x, y);
        line.push_back(point);
    }
    return true;
}


// 将 silly_line(线) 转换为 OGRLineString(线)类型
OGRLineString* convertLineStringFromSillyLine(const silly_line& line)
{
    OGRLineString* ogrLineString = new OGRLineString;
    for (const silly_point& point : line) 
    {
        ogrLineString->addPoint(point.lgtd, point.lttd);
    }
    return ogrLineString;
}


// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
bool processMultiLineString(OGRMultiLineString* multiLineString, silly_multi_silly_line& multiLine)
{
    int numLines = multiLineString->getNumGeometries();
    for (int i = 0; i < numLines; i++)
    {
        OGRLineString* lineString = dynamic_cast<OGRLineString*>(multiLineString->getGeometryRef(i));
        if (lineString != nullptr)
        {
            silly_line line;
            processLineString(lineString, line);
            multiLine.push_back(line);
        }
    }
    return true;
}

// 将 silly_multiline(多线) 转换为 OGRMultiLineString(多线)类型
OGRMultiLineString* convertMultiLineStringFromSillyMultiLine(const silly_multi_silly_line& multiLine)
{
    OGRMultiLineString* ogrMultiLineString = new OGRMultiLineString;

    for (const silly_line& line : multiLine)
    {
        OGRLineString* ogrLineString = convertLineStringFromSillyLine(line);
        ogrMultiLineString->addGeometryDirectly(ogrLineString);
    }

    return ogrMultiLineString;
}


// OGRPolygon对象转换为silly_poly(多环:外环+内环)对象  (单面)
bool processPolygon(OGRPolygon* polygon, silly_poly& poly)
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
    return true;
}


// 将 silly_poly 转换为 OGRPolygon(单面)
OGRPolygon* convertToOGRPolygon(const silly_poly& poly)
{
    OGRPolygon* ogrPolygon = new OGRPolygon;
    // 设置外环
    OGRLinearRing* outerRing = new OGRLinearRing;
    for (const silly_point& point : poly.outer_ring.points) {
        outerRing->addPoint(point.lgtd, point.lttd);
    }
    ogrPolygon->addRingDirectly(outerRing);

    // 设置内环
    for (const silly_ring& innerRing : poly.inner_rings) {
        OGRLinearRing* innerOGRRing = new OGRLinearRing;
        for (const silly_point& point : innerRing.points) {
            innerOGRRing->addPoint(point.lgtd, point.lttd);
        }
        ogrPolygon->addRingDirectly(innerOGRRing);
    }
    return ogrPolygon;
}


//多面的OGRMultiPolygon对象转换为silly_multi_poly(多面)
bool processMultiPolygon(OGRMultiPolygon* multiPolygon, silly_multi_poly& poly)
{
    int polygonCount = multiPolygon->getNumGeometries();
    for (int i = 0; i < polygonCount; i++)
    {
        silly_poly  tmp_poly;
        OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(multiPolygon->getGeometryRef(i));
        processPolygon(polygon, tmp_poly);
        poly.push_back(tmp_poly);
    }
    return true;
}


// 将silly_multi_poly对象转换为OGRMultiPolygon对象(多面)
OGRMultiPolygon* convertToOGRMultiPolygon(const silly_multi_poly& multiPoly)
{
    OGRMultiPolygon* ogrMultiPolygon = new OGRMultiPolygon;
    for (const silly_poly& poly : multiPoly) 
    {
        OGRPolygon* ogrPolygon = new OGRPolygon;

        // 设置外环
        OGRLinearRing* outerRing = new OGRLinearRing;
        for (const silly_point& point : poly.outer_ring.points)
        {
            outerRing->addPoint(point.lgtd, point.lttd);
        }
        ogrPolygon->addRing(outerRing);

        // 设置内环
        for (const silly_ring& innerRing : poly.inner_rings) {
            OGRLinearRing* innerOGRRing = new OGRLinearRing;
            for (const silly_point& point : innerRing.points) {
                innerOGRRing->addPoint(point.lgtd, point.lttd);
            }
            ogrPolygon->addRing(innerOGRRing);
        }
        ogrMultiPolygon->addGeometry(ogrPolygon);
    }

    return ogrMultiPolygon;
}




std::vector<silly_ring> geo_utils::intersect_area(silly_ring ring_1, silly_ring ring_2)
{
    std::vector<silly_ring> intersecting_rings;

    // 创建 OGRPolygon 对象
    OGRPolygon* poly1 = convertPolygonFromSillyRing(ring_1);
    OGRPolygon* poly2 = convertPolygonFromSillyRing(ring_2);

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

void geo_utils::destroy_gdal_env()
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

//bool geo_utils::check_shp_info(const std::string& shp_file, enum_geometry_types& type, std::map<std::string, std::string>& properties)
//{
//    bool status = false;
//    std::map<std::string, std::string> result;
//
//    GDALDataset* poDSr = (GDALDataset*)GDALOpenEx(shp_file.c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, NULL, NULL, NULL);
//    if (NULL == poDSr)
//    {
//        return status;
//    }
//    OGRLayer* poLayer_r = poDSr->GetLayer(0);
//    if (poLayer_r == NULL)
//    {
//        GDALClose(poDSr);
//        return status;
//    }
//    OGRFeature* pFeature_r = NULL;
//    //获取Feature特征
//    pFeature_r = poLayer_r->GetNextFeature();
//    if (NULL == pFeature_r)
//    {
//        GDALClose(poDSr);
//        return status;
//    }
//
//    //获取要素的几何形状
//    OGRGeometry* poGeometry_r = pFeature_r->GetGeometryRef();
//    type = (enum_geometry_types)wkbFlatten(poGeometry_r->getGeometryType());
//    if (wkbUnknown == type)
//    {
//        GDALClose(poDSr);
//        return status;
//    }
//    // 1 wkbPoint 点
//    // 2 wkbLineString 线
//    // 3 wkbPolygon 面
//    // 4 wkbMultiPoint  多点
//    // 5 wkbMultiLineString  多线
//    // 6 wkbMultiPolygon  多面
//
//    //循环特征域
//    int fieldCnt = pFeature_r->GetFieldCount();
//    for (int i = 0; i < fieldCnt; i++)
//    {
//        OGRFieldDefn* def = pFeature_r->GetFieldDefnRef(i);
//        OGRFieldType type = def->GetType();
//        std::string field_name = def->GetNameRef();
//        if (!silly_code::IsUTF8((uint8_t*)field_name.c_str(), field_name.size()))
//        {
//            field_name = silly_conv::GBK2UTF8(field_name);
//        }
//
//        std::string field_type = "";
//        switch (type)
//        {
//        case OFTInteger:
//            field_type = "OFTInteger";
//            break;
//        case OFTIntegerList:
//            field_type = "OFTIntegerList";
//            break;
//        case OFTReal:
//            field_type = "OFTReal";
//            break;
//        case OFTRealList:
//            field_type = "OFTRealList";
//            break;
//        case OFTString:
//            field_type = "OFTString";
//            break;
//        case OFTStringList:
//            field_type = "OFTStringList";
//            break;
//#if _DEBUG
//            //弃用 deprecated
//        case OFTWideString:
//            field_type = "Deprecated:OFTWideString";
//
//            break;
//        case OFTWideStringList:
//            field_type = "Deprecated:OFTWideStringList";
//            break;
//#endif
//        case OFTBinary:
//            field_type = "OFTBinary";
//            break;
//        case OFTDate:
//            field_type = "OFTDate";
//            break;
//        case OFTTime:
//            field_type = "OFTTime";
//            break;
//        case OFTDateTime:
//            field_type = "OFTDateTime";
//            break;
//        case OFTInteger64:
//            field_type = "OFTInteger64";
//            break;
//        case OFTInteger64List:
//            field_type = "OFTInteger64List";
//            break;
//        }
//        properties[field_name] = field_name;
//    }
//
//
//    GDALClose(poDSr);
//    status = true;
//    return status;
//
//}

bool geo_utils::check_shp_info(const std::string& shp_file, enum_geometry_types& type, std::map<std::string, enum_geoprop_types>& properties)
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
        enum_geoprop_types field_type{ eNone };
        switch (type)
        {
        case OFTInteger:
            field_type = eInt;
            break;
        case OFTIntegerList:
            break;
        case OFTReal:
            field_type = eNumeric;
            break;
        case OFTRealList:
            break;
        case OFTString:
            field_type = eString;
            break;
        case OFTStringList:
            break;
#if _DEBUG
            //弃用 deprecated
        case OFTWideString:
            break;
        case OFTWideStringList:
            break;
#endif
        case OFTBinary:
            field_type = eBinary;
            break;
        case OFTTime:
            field_type = eTime;
            break;
        case OFTDate:
            field_type = eDate;
            break;
        case OFTDateTime:
            field_type = eDateTime;
            break;
        case OFTInteger64:
            field_type = eLong;
            break;
        case OFTInteger64List:
            break;
        }
        properties[field_name] = field_type;
    }


    GDALClose(poDSr);
    status = true;
    return status;

}


/// <summary>
/// 读取一个矢量数据的属性
/// </summary>
/// <param name="feature"></param>
/// <param name="properties"></param>
/// <param name="props"></param>
/// <returns></returns>
bool read_property(const OGRFeature* feature, const std::map<std::string, enum_geoprop_types>& properties, std::map<std::string, silly_geo_prop>& props)
{
    for (const auto& entry : properties)
    {
        std::string key = entry.first;
        size_t temp_size = 0;
        enum_geoprop_types th_type = entry.second;
        switch (th_type)
        {
        case eNone:
            break;
        case eInt:
        {
            int value = feature->GetFieldAsInteger(entry.first.c_str());
            temp_size = sizeof(value);
            props[key].data = new unsigned char[temp_size];
            std::memcpy(props[key].data, &value, temp_size);
        }
        break;
        case eNumeric:
        {
            double value = feature->GetFieldAsDouble(entry.first.c_str());
            temp_size = sizeof(value);
            props[key].data = new unsigned char[temp_size];
            std::memcpy(props[key].data, &value, temp_size);
            //double extractedValue;
            //std::memcpy(&extractedValue, props[key].data, temp_size);
            //int a = 0;
        }
        break;
        case eString:
        {
            std::string value = feature->GetFieldAsString(entry.first.c_str());
            //if (silly_code::Encode::eeUTF8 != silly_code::IsUTF8((uint8_t*)value.c_str(), value.size()))
            //{
            //    value = silly_conv::GBK2UTF8(value);
            //}
            temp_size = value.size() + 1;
            props[key].data = new unsigned char[temp_size];
            std::memcpy(props[key].data, value.c_str(), temp_size);

        }
        break;
        case eTime:
            break;
        case eDate:
            break;
        case eDateTime:
        {
            int year, month, day, hour, minute, second, tzFlag;
            int result = feature->GetFieldAsDateTime(0, &year, &month, &day, &hour, &minute, &second, &tzFlag);
        }
        break;
        case eLong:
        {
            long long value = feature->GetFieldAsInteger64(entry.first.c_str());
            temp_size = sizeof(value);
            props[key].data = new unsigned char[temp_size];
            std::memcpy(props[key].data, &value, temp_size);
        }
        break;
        default:
            break;
        }

        props[key].type = entry.second;
        props[key].len = temp_size;
    }
    return true;
}


bool read_all_types_data(const enum_geometry_types feature_type, OGRGeometry* geometry, silly_geo_coll& geo_coll)
{
    bool status = false;
    geo_coll.comp_type.push_back(feature_type);
    switch (feature_type)
    {
    case eInvalid:
        SU_INFO_PRINT("Warn: Unable to process data of this type: %s ", "eInvalid");
        break;
    case ePoint:                // 单点
    {
        auto geo_type = geometry->getGeometryType();
        OGRPoint* point = dynamic_cast<OGRPoint*>(geometry);
        status = processPoint(point, geo_coll.m_point);
    }
    break;
    case eLineString:           // 单线        
    {
        OGRLineString* lineString = dynamic_cast<OGRLineString*>(geometry);
        status = processLineString(lineString, geo_coll.m_line);
    }
    break;
    case ePolygon:              // 单面
    {
        OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geometry);
        status = processPolygon(polygon, geo_coll.m_poly);
    }
    break;
    case eMultiPoint:           // 多点
    {
        OGRMultiPoint* multiPoint = dynamic_cast<OGRMultiPoint*>(geometry);
        status = processMultiPoint(multiPoint, geo_coll.m_m_points);
    }
    break;
    case eMultiLineString:      // 多线
    {
        OGRMultiLineString* multiLineString = dynamic_cast<OGRMultiLineString*>(geometry);
        status = processMultiLineString(multiLineString, geo_coll.m_m_lines);
    }
    break;
    case eMultiPolygon:         // 多面
    {
        OGRMultiPolygon* multiPolygon = dynamic_cast<OGRMultiPolygon*>(geometry);
        status = processMultiPolygon(multiPolygon, geo_coll.m_m_polys);
    }
    break;
    case eCompositeType:        // 复合数据类型  
    {
        OGRGeometryCollection* geomCollection = dynamic_cast<OGRGeometryCollection*>(geometry);
        int numGeometries = geomCollection->getNumGeometries();
        SU_DEBUG_PRINT("Number of Geometries in Collection: %d", numGeometries);
        for (int j = 0; j < numGeometries; j++)
        {
            OGRGeometry* collGeometry = geomCollection->getGeometryRef(j);
            if (collGeometry != nullptr)
            {
                enum_geometry_types feature_type = (enum_geometry_types)wkbFlatten(collGeometry->getGeometryType());
                status = read_all_types_data(feature_type, collGeometry, geo_coll);
            }
        }
    }
    break;
    default:
    {
        SU_ERROR_PRINT("Unprocessable data types: %s", std::to_string(feature_type).c_str());
    }
    break;
    }

    return status;
}


bool geo_utils::read_geo_coll(const char* file, std::vector<silly_geo_coll>& collections)
{
    bool status = false;
    enum_geometry_types type;
    std::map<std::string, enum_geoprop_types> properties;
    if (!check_shp_info(file, type, properties))
    {
        std::cout << "Error check shp info " << std::endl;
        return status;
    }
    // 打开现有 shp 文件
	GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(file, GDAL_OF_ALL | GDAL_OF_READONLY, NULL, NULL, NULL));
	if (dataset == nullptr)
	{
		// 处理文件打开失败的情况
        SU_ERROR_PRINT("Error: Failed to open shapefile ");
		return status;
	}
    // 获得数据集中图层数
    int layerCount = dataset->GetLayerCount();
    for (int i = 0; i < layerCount; i++)
    {
        OGRLayer* layer = dataset->GetLayer(i);
        if (layer == nullptr)
        {
	        // 处理图层获取失败的情况
            SU_ERROR_PRINT("Error: Failed to get layer ");
	        GDALClose(dataset);
            return status;
        }
        layer->ResetReading();
        OGRFeature* feature;
        int iEntities = layer->GetFeatureCount();  // 获取属性的个数,即矢量数据的个数
        if (iEntities <= 0)
        {
            GDALClose(dataset);
            return status;
        }
        int n = 0;
        while ((feature = layer->GetNextFeature()) != nullptr)  // 遍历 矢量数据 
        {
            silly_geo_coll temp_geo_coll; 
            OGRGeometry* geometry = feature->GetGeometryRef();          // 获取矢量数据
            enum_geometry_types feature_type = (enum_geometry_types)wkbFlatten(geometry->getGeometryType());
            temp_geo_coll.m_type = feature_type;                        // 添加矢量数据类型   
            read_property(feature, properties, temp_geo_coll.m_props);  // 读取属性数据
            status = read_all_types_data(feature_type, geometry, temp_geo_coll);  // 添加所有数据类型,如果是复合数据类型会递归的调用
            OGRFeature::DestroyFeature(feature);
            collections.push_back(temp_geo_coll);
        }


    }  // 一个图层结束

    return status;
}


// 根据文件拓展得到对应的存储格式
bool GetDriverName(const char* file, std::string& driverName)
{
    // 获取文件扩展名
    std::string lowerExtension = std::filesystem::path(file).extension().string();
    // 将文件扩展名转换为小写
    std::transform(lowerExtension.begin(), lowerExtension.end(), lowerExtension.begin(), ::tolower);
    if (lowerExtension == ".shp") {
        driverName = "ESRI Shapefile";
    }
    else if (lowerExtension == ".tab") {
        driverName = "Mapinfo File";
    }
    else if (lowerExtension == ".geojson") {
        driverName = "GeoJSON";
    }
    else if (lowerExtension == ".sqlite") {
        driverName = "SQLite";
    }
    else if (lowerExtension == ".csv") {
        driverName = "CSV";
    }
    else if (lowerExtension == ".kml") {
        driverName = "KML";
    }
    else if (lowerExtension == ".gml") {
        driverName = "GML";
    }
    else if (lowerExtension == ".xlsx") {
        driverName = "XLSX";
    }
    else {
        driverName = "none";
        return false;
    }

    return true;
}


//根据enum_geoprop_types 找gdal中属性的类型
OGRFieldType convertToOGRFieldType(enum_geoprop_types geopropType)  
{
    switch (geopropType)
    {
    case eNone:
        return OFTInteger;
    case eInt:
        return OFTInteger;
    case eNumeric:
        return OFTReal;
    case eString:
        return OFTString;
    case eBinary:
        return OFTBinary;
    case eTime:
        return OFTTime;
    case eDate:
        return OFTDate;
    case eDateTime:
        return OFTDateTime;
    case eLong:
        return OFTInteger64;
    default:
        return OFTInteger;
    }
}


// 添加属性到shp中
bool writePropertiesToGeometry(OGRFeature* feature, const std::map<std::string, silly_geo_prop>& m_props)
{
    bool status = false;
    for (const auto& prop : m_props)
    {
        std::string key = prop.first;
        silly_geo_prop propValue = prop.second;
        int fieldIndex = feature->GetFieldIndex(key.c_str());
        if (fieldIndex >= 0)
        {
            switch (propValue.type)
            {
            case eInt:
            {
                int value;
                std::memcpy(&value, prop.second.data, prop.second.len);
                feature->SetField(fieldIndex, value);
                status = true;
            }
            break;
            case eNumeric:
            {
                double value;
                std::memcpy(&value, prop.second.data, prop.second.len);
                feature->SetField(fieldIndex, value);
                status = true;
            }
            break;
            case eString:
            {
                std::string value(reinterpret_cast<char*>(prop.second.data), prop.second.len);
                feature->SetField(fieldIndex, value.c_str());
                status = true;
            }
            break;
            case eTime:
                break;
            case eDate:
                break;
            case eDateTime:
                break;
            case eLong:
            {
                long long value;
                std::memcpy(&value, prop.second.data, prop.second.len);
                feature->SetField(fieldIndex, value);
                status = true;
            }
            break;
            default:
                break;
            }

        }
    }
    return status;
}

// 处理复合数据类型的变量
bool process_composite_data(const enum_geometry_types coll_type, OGRGeometry* geometry, OGRGeometryCollection* geomCollection, const silly_geo_coll& geo_coll)
{
    bool status = false;
    switch (coll_type)
    {
    case eInvalid:
        break;
    case ePoint:
    {
        OGRPoint ogrPoint(geo_coll.m_point.lgtd, geo_coll.m_point.lttd);
        geomCollection->addGeometry(&ogrPoint);
        status = true;
    }
    break;
    case eLineString:
    {
        OGRLineString* orgLine = dynamic_cast<OGRLineString*>(geometry);
        orgLine = convertLineStringFromSillyLine(geo_coll.m_line);
        geomCollection->addGeometry(orgLine);
        status = true;
    }
    break;
    case ePolygon:
    {
        OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geometry);
        polygon = convertToOGRPolygon(geo_coll.m_poly);
        geomCollection->addGeometry(polygon);
        status = true;
    }
    break;
    case eMultiPoint:
    {
        OGRMultiPoint* multiPoint = dynamic_cast<OGRMultiPoint*>(geometry);
        multiPoint = convertOGRMultiPointFromMultiPoint(geo_coll.m_m_points);
        geomCollection->addGeometry(multiPoint);
        status = true;
    }
    break;
    case eMultiLineString:
    {
        OGRMultiLineString* multiLineString = dynamic_cast<OGRMultiLineString*>(geometry);
        geomCollection->addGeometry(multiLineString);
        status = true;
    }
    break;
    case eMultiPolygon:
    {
        OGRMultiPolygon* multiPolygon = dynamic_cast<OGRMultiPolygon*>(geometry);
        multiPolygon = convertToOGRMultiPolygon(geo_coll.m_m_polys);
        geomCollection->addGeometry(multiPolygon);
        status = true;
    }
    break;
    default:
    {
        SU_ERROR_PRINT("Composite unprocessable data types: %s", std::to_string(coll_type).c_str());
    }
    break;
    }
    return status;
}


// 写入所有类型的数据
bool wire_all_types_data(const enum_geometry_types coll_type, OGRLayer* outputLayer,OGRFeature* feature, OGRGeometry* geometry, const silly_geo_coll& geo_coll)
{
    bool status = false;
    switch (coll_type)
    {
    case eInvalid:
        break;
    case ePoint:
    {
        OGRPoint ogrPoint(geo_coll.m_point.lgtd, geo_coll.m_point.lttd);
        feature->SetGeometry(&ogrPoint);
        if (outputLayer->CreateFeature(feature) != OGRERR_NONE)            // 在图层中创建要素
        {
            SU_ERROR_PRINT("Failed to create feature in shapefile.");
        }
        status = true;
    }
    break;
    case eLineString:
    {
        OGRLineString* orgLine = dynamic_cast<OGRLineString*>(geometry);
        orgLine = convertLineStringFromSillyLine(geo_coll.m_line);
        feature->SetGeometry(orgLine);
        if (outputLayer->CreateFeature(feature) != OGRERR_NONE)            // 在图层中创建要素
        {
            SU_ERROR_PRINT("Failed to create feature in shapefile.");
        }
        status = true;
    }
    break;
    case ePolygon:
    {
        OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geometry);
        polygon = convertToOGRPolygon(geo_coll.m_poly);
        feature->SetGeometry(polygon);
        if (outputLayer->CreateFeature(feature) != OGRERR_NONE)            // 在图层中创建要素
        {
            SU_ERROR_PRINT("Failed to create feature in shapefile.");
        }
        status = true;
    }
    break;
    case eMultiPoint:
    {
        OGRMultiPoint* multiPoint = dynamic_cast<OGRMultiPoint*>(geometry);
        multiPoint = convertOGRMultiPointFromMultiPoint(geo_coll.m_m_points);
        feature->SetGeometry(multiPoint);
        if (outputLayer->CreateFeature(feature) != OGRERR_NONE)            // 在图层中创建要素
        {
            SU_ERROR_PRINT("Failed to create feature in shapefile.");
        }
        status = true;
    }
    break;
    case eMultiLineString:
    {
        OGRMultiLineString* multiLineString = dynamic_cast<OGRMultiLineString*>(geometry);
        multiLineString = convertMultiLineStringFromSillyMultiLine(geo_coll.m_m_lines);
        status = true;
    }
    break;
    case eMultiPolygon:
    {
        OGRMultiPolygon* multiPolygon = dynamic_cast<OGRMultiPolygon*>(geometry);
        multiPolygon = convertToOGRMultiPolygon(geo_coll.m_m_polys);
        feature->SetGeometry(multiPolygon);
        if (outputLayer->CreateFeature(feature) != OGRERR_NONE)             // 在图层中创建要素
        {
            SU_ERROR_PRINT("Failed to create feature in shapefile.");
        }
        status = true;
    }
    break;
    case eCompositeType:
    {
        OGRGeometryCollection* geomCollection = dynamic_cast<OGRGeometryCollection*>(geometry);
        for (const auto& type : geo_coll.comp_type)
        {
            status = process_composite_data(type, geometry, geomCollection, geo_coll);
        }
        feature->SetGeometry(geomCollection);
        if (outputLayer->CreateFeature(feature) != OGRERR_NONE)             // 在图层中创建要素
        {
            SU_ERROR_PRINT("Failed to create feature in shapefile.");
        }
    }
        break;
    default:
    {
        SU_ERROR_PRINT("Unprocessable data types: %s", std::to_string(coll_type).c_str());
    }
    break;
    }
    return status;
}




bool geo_utils::write_geo_coll(const char* file, const std::vector<silly_geo_coll>& collections) 
{
    bool status = false;
    // 根据拓展名得到存储格式
    std::string driverName;
    if (!GetDriverName(file, driverName))
    {
        SU_ERROR_PRINT("Error: Unable to obtain storage method for this type: %s ", file);
        return status;
    }
    std::string LayerName = std::filesystem::path(file).filename().stem().string();
    if (collections.empty())
    {
        SU_ERROR_PRINT("Error: Vector data is empty ");
        return status;
    }
    GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName(driverName.c_str());
    GDALDataset* outputData = outDriver->Create(file, 0, 0, 0, GDT_Unknown, nullptr);
    if (outputData == nullptr)
    {
        SU_ERROR_PRINT("Error: Failed to create output shapefile ");
        return false;
    }
    OGRLayer* outputLayer = outputData->CreateLayer(LayerName.c_str(), nullptr, wkbUnknown, nullptr);
    if (outputLayer == nullptr)
    {
        // 处理图层创建失败的情况
        SU_ERROR_PRINT("Failed to create output layer " )
        GDALClose(outputData);
        return false;
    }
    auto oneColl = collections.begin();
    for (const auto& prop : oneColl->m_props)  // 添加属性
    {
        std::string key = prop.first;
        silly_geo_prop propValue = prop.second;
        OGRFieldType ogrType = convertToOGRFieldType(propValue.type);
        OGRFieldDefn fieldDef(key.c_str(), ogrType);
        if (outputLayer->CreateField(&fieldDef) != OGRERR_NONE)
        {
            SU_ERROR_PRINT("Error: Failed to create color field ");
        }
    }
    for (const auto& coll : collections)
    {
        OGRFeature* feature = OGRFeature::CreateFeature(outputLayer->GetLayerDefn());
        // 添加矢量
        enum_geometry_types coll_type = coll.m_type;
        OGRGeometry* geometry = OGRGeometryFactory::createGeometry((OGRwkbGeometryType)coll_type);
        if (!writePropertiesToGeometry(feature, coll.m_props))  // 添加属性
        {
            SU_ERROR_PRINT("Add attribute fail ")
        }
        // 添加矢量数据
        status = wire_all_types_data(coll_type, outputLayer, feature, geometry, coll);
 
        OGRFeature::DestroyFeature(feature);
        OGRGeometryFactory::destroyGeometry(geometry);
    }

    // 关闭数据集
    GDALClose(outputData);
    SU_INFO_PRINT("Vector data added to shapefile and saved successfully ");

    return status;
}