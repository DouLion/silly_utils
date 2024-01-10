

#include "silly_geo_utils.h"
#include <math.h>
#include <filesystem>

// GDAL.
#include "ogr_spatialref.h"
// #include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "ogr_api.h"

#include <polyclipping/clipper.hpp>
#include <encode/silly_encode.h>
#include "su_marco.h"

using namespace ClipperLib;

silly_point geo_utils::ring_to_center(silly_poly poly)
{
    silly_point center_point;
    OGRPolygon* orgPloy = geo_utils::SillyPolyToOGRPoly(poly);
    OGRPoint point;
    int err = orgPloy->Centroid(&point);
    if (0 == err)
    {
        center_point.lgtd = point.getX();
        center_point.lttd = point.getY();
    }
    return center_point;
}

silly_geo_coll geo_utils::intersection_area(silly_poly poly_main, silly_poly poly_deputy)
{
    silly_geo_coll result;

    // 创建 OGRPolygon 对象
    OGRPolygon* org_ploy_main = geo_utils::SillyPolyToOGRPoly(poly_main);
    OGRPolygon* org_ploy_deputy = geo_utils::SillyPolyToOGRPoly(poly_deputy);

    // 判断两个 OGRPolygon 是否相交
    if (!org_ploy_main->Intersects(org_ploy_deputy))
    {
        // 不相交，返回空的vector
        delete org_ploy_main;
        delete org_ploy_deputy;
        SU_INFO_PRINT("Two regions do not intersect \n");
        return result;
    }

    // 计算相交区域
    OGRGeometry* intersection = org_ploy_main->Intersection(org_ploy_deputy);

    // 处理不同几何类型的情况
    OGRwkbGeometryType geometryType = intersection->getGeometryType();
    switch (geometryType)
    {
        // 单面
        case wkbPolygon:
        case wkbPolygon25D:
        {
            OGRPolygon* intersectingPolygon = dynamic_cast<OGRPolygon*>(intersection);
            result.m_poly = geo_utils::OGRPolyToSillyPoly(intersectingPolygon);
            result.m_type = ePolygon;
            delete intersectingPolygon;
            break;
        }
        // 多面
        case wkbMultiPolygon:
        case wkbMultiPolygon25D:
        {
            OGRMultiPolygon* intersectingMultiPolygon = dynamic_cast<OGRMultiPolygon*>(intersection);
            result.m_m_polys = geo_utils::OGRMulPolyToSillyMulPoly(intersectingMultiPolygon);
            result.m_type = eMultiPolygon;
            delete intersectingMultiPolygon;
            break;
        }
        // 处理其他几何类型的情况
        default:
            SU_ERROR_PRINT("Error: Unable to process this type \n");
            break;
    }

    delete org_ploy_main;
    delete org_ploy_deputy;

    return result;
}

double geo_utils::two_point_azimuth(silly_point from, silly_point to)
{
    double theta = atan2(to.lgtd - from.lgtd, to.lttd - from.lttd);
    theta = theta * 180.0 / SU_PI;
    return theta;
}

// 将 silly_ring 转换为 OGRPolygon
OGRPolygon* geo_utils::SillyRingToPolygon(const silly_ring& ring)
{
    OGRPolygon* polygon = new OGRPolygon;
    OGRLinearRing* ringObj = new OGRLinearRing;

    for (const auto& point : ring.points)
    {
        ringObj->addPoint(point.lgtd, point.lttd);
    }

    polygon->addRingDirectly(ringObj);
    polygon->closeRings();
    return polygon;
}

// 环OGRLinearRing对象，将其转换为silly_ring对象  (环)
silly_ring geo_utils::OGRRingToSillyRing(OGRLinearRing* ring)
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
silly_point geo_utils::OGRPointToSillyPoint(OGRPoint* ogrPoint)
{
    silly_point Point;
    Point.lgtd = ogrPoint->getX();
    Point.lttd = ogrPoint->getY();
    return Point;
}

// 将 silly_point(单点) 转换为 OGRPoint(单点) 类型
OGRPoint* geo_utils::SillyPointToOGRPoint(const silly_point& point)
{
    OGRPoint* ogrPoint = new OGRPoint(point.lgtd, point.lttd);
    return ogrPoint;
}

// 将 OGRMultiPoint(多点) 转换为 silly_multi_point(多点) 类型
silly_multi_point geo_utils::OGRMulPointToSillyMulPoint(OGRMultiPoint* ogrMultiPoint)
{
    silly_multi_point mulitPoint;
    int pointCount = ogrMultiPoint->getNumGeometries();
    for (int i = 0; i < pointCount; i++)
    {
        OGRPoint* point = dynamic_cast<OGRPoint*>(ogrMultiPoint->getGeometryRef(i));
        if (point != nullptr)
        {
            silly_point sillyPoint = OGRPointToSillyPoint(point);
            mulitPoint.push_back(sillyPoint);
        }
    }
    return mulitPoint;
}

// 将 silly_multi_point(多点) 转换为 OGRMultiPoint(多点) 类型
OGRMultiPoint* geo_utils::SillyMulPointToOGRMulPoint(const silly_multi_point& mulitPoint)
{
    OGRMultiPoint* orgMultiPoint = new OGRMultiPoint;
    for (const silly_point& point : mulitPoint)
    {
        OGRPoint* ogrPoint = SillyPointToOGRPoint(point);
        orgMultiPoint->addGeometryDirectly(ogrPoint);
    }
    return orgMultiPoint;
}

// OGRLineString(线)类型转为silly_line(线)类型
silly_line geo_utils::OGRLineToSillyLine(OGRLineString* lineString)
{
    silly_line line;
    int num_points = lineString->getNumPoints();
    for (int j = 0; j < num_points; j++)
    {
        double x = lineString->getX(j);
        double y = lineString->getY(j);
        silly_point point(x, y);
        line.push_back(point);
    }
    return line;
}

// 将 silly_line(线) 转换为 OGRLineString(线)类型
OGRLineString* geo_utils::SillyLineToOGRLine(const silly_line& line)
{
    OGRLineString* ogrLineString = new OGRLineString;
    for (const silly_point& point : line)
    {
        ogrLineString->addPoint(point.lgtd, point.lttd);
    }
    return ogrLineString;
}

// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
silly_multi_silly_line geo_utils::OGRMulLineToSillyMulLine(OGRMultiLineString* multiLineString)
{
    silly_multi_silly_line multiLine;
    int numLines = multiLineString->getNumGeometries();
    for (int i = 0; i < numLines; i++)
    {
        OGRLineString* lineString = dynamic_cast<OGRLineString*>(multiLineString->getGeometryRef(i));
        if (lineString != nullptr)
        {
            silly_line line = OGRLineToSillyLine(lineString);
            multiLine.push_back(line);
        }
    }
    return multiLine;
}

// 将 silly_multiline(多线) 转换为 OGRMultiLineString(多线)类型
OGRMultiLineString* geo_utils::SillyMulLineToOGRMulLine(const silly_multi_silly_line& multiLine)
{
    OGRMultiLineString* ogrMultiLineString = new OGRMultiLineString;

    for (const silly_line& line : multiLine)
    {
        OGRLineString* ogrLineString = SillyLineToOGRLine(line);
        ogrMultiLineString->addGeometryDirectly(ogrLineString);
    }

    return ogrMultiLineString;
}

// OGRPolygon对象转换为silly_poly(多环:外环+内环)对象  (单面)
silly_poly geo_utils::OGRPolyToSillyPoly(OGRPolygon* polygon)
{
    silly_poly poly;
    // 处理OGRPolygon外环
    OGRLinearRing* outerRing = polygon->getExteriorRing();
    poly.outer_ring = OGRRingToSillyRing(outerRing);
    // 处理OGRPolygon内环
    int innerRingCount = polygon->getNumInteriorRings();
    for (int k = 0; k < innerRingCount; k++)
    {
        OGRLinearRing* ring = polygon->getInteriorRing(k);
        silly_ring innerRing = OGRRingToSillyRing(ring);
        poly.inner_rings.push_back(innerRing);
    }
    return poly;
}

// 将 silly_poly 转换为 OGRPolygon(单面)
OGRPolygon* geo_utils::SillyPolyToOGRPoly(const silly_poly& poly)
{
    OGRPolygon* ogrPolygon = new OGRPolygon;
    // 设置外环
    OGRLinearRing* outerRing = new OGRLinearRing;
    for (const silly_point& point : poly.outer_ring.points)
    {
        outerRing->addPoint(point.lgtd, point.lttd);
    }
    ogrPolygon->addRingDirectly(outerRing);
    ogrPolygon->closeRings();
    // 设置内环
    for (const silly_ring& innerRing : poly.inner_rings)
    {
        OGRLinearRing* innerOGRRing = new OGRLinearRing;
        for (const silly_point& point : innerRing.points)
        {
            innerOGRRing->addPoint(point.lgtd, point.lttd);
        }
        ogrPolygon->addRingDirectly(innerOGRRing);
        ogrPolygon->closeRings();
    }
    return ogrPolygon;
}

// 多面的OGRMultiPolygon对象转换为silly_multi_poly(多面)
silly_multi_poly geo_utils::OGRMulPolyToSillyMulPoly(OGRMultiPolygon* multiPolygon)
{
    silly_multi_poly multi_poly;
    int polygonCount = multiPolygon->getNumGeometries();
    for (int i = 0; i < polygonCount; i++)
    {
        silly_poly tmp_poly;
        OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(multiPolygon->getGeometryRef(i));
        tmp_poly = OGRPolyToSillyPoly(polygon);
        multi_poly.push_back(tmp_poly);
    }
    return multi_poly;
}

// 将silly_multi_poly对象转换为OGRMultiPolygon对象(多面)
OGRMultiPolygon* geo_utils::SillyMulPolyToOGRMulPoly(const silly_multi_poly& multiPoly)
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
        for (const silly_ring& innerRing : poly.inner_rings)
        {
            OGRLinearRing* innerOGRRing = new OGRLinearRing;
            for (const silly_point& point : innerRing.points)
            {
                innerOGRRing->addPoint(point.lgtd, point.lttd);
            }
            ogrPolygon->addRing(innerOGRRing);
        }
        ogrMultiPolygon->addGeometry(ogrPolygon);
    }

    return ogrMultiPolygon;
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
    // 获取Feature特征
    pFeature_r = poLayer_r->GetNextFeature();
    if (NULL == pFeature_r)
    {
        GDALClose(poDSr);
        return status;
    }

    // 获取要素的几何形状
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

    // 循环特征域
    int fieldCnt = pFeature_r->GetFieldCount();
    for (int i = 0; i < fieldCnt; i++)
    {
        OGRFieldDefn* def = pFeature_r->GetFieldDefnRef(i);
        OGRFieldType type = def->GetType();
        std::string field_name = def->GetNameRef();
        if (!silly_encode::is_utf8(field_name.c_str(), field_name.size()))
        {
            field_name = silly_encode::gbk_utf8(field_name);
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
                // 弃用 deprecated
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
    // 获取Feature特征
    pFeature_r = poLayer_r->GetNextFeature();
    if (NULL == pFeature_r)
    {
        GDALClose(poDSr);
        return status;
    }

    // 获取要素的几何形状
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

    // 循环特征域
    int fieldCnt = pFeature_r->GetFieldCount();
    for (int i = 0; i < fieldCnt; i++)
    {
        OGRFieldDefn* def = pFeature_r->GetFieldDefnRef(i);
        OGRFieldType type = def->GetType();
        std::string field_name = def->GetNameRef();
        if (!silly_encode::is_utf8(field_name.c_str(), field_name.size()))
        {
            field_name = silly_encode::gbk_utf8(field_name);
        }
        enum_geoprop_types field_type{eNone};
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
                // 弃用 deprecated
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
                // double extractedValue;
                // std::memcpy(&extractedValue, props[key].data, temp_size);
                // int a = 0;
            }
            break;
            case eString:
            {
                std::string value = feature->GetFieldAsString(entry.first.c_str());
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
        case ePoint:  // 单点
        {
            auto geo_type = geometry->getGeometryType();
            OGRPoint* point = dynamic_cast<OGRPoint*>(geometry);
            geo_coll.m_point = geo_utils::OGRPointToSillyPoint(point);
            status = true;
        }
        break;
        case eLineString:  // 单线
        {
            OGRLineString* lineString = dynamic_cast<OGRLineString*>(geometry);
            geo_coll.m_line = geo_utils::OGRLineToSillyLine(lineString);
            status = true;
        }
        break;
        case ePolygon:  // 单面
        {
            OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geometry);
            geo_coll.m_poly = geo_utils::OGRPolyToSillyPoly(polygon);
            status = true;
        }
        break;
        case eMultiPoint:  // 多点
        {
            OGRMultiPoint* multiPoint = dynamic_cast<OGRMultiPoint*>(geometry);
            geo_coll.m_m_points = geo_utils::OGRMulPointToSillyMulPoint(multiPoint);
            status = true;
        }
        break;
        case eMultiLineString:  // 多线
        {
            OGRMultiLineString* multiLineString = dynamic_cast<OGRMultiLineString*>(geometry);
            geo_coll.m_m_lines = geo_utils::OGRMulLineToSillyMulLine(multiLineString);
            status = true;
        }
        break;
        case eMultiPolygon:  // 多面
        {
            OGRMultiPolygon* multiPolygon = dynamic_cast<OGRMultiPolygon*>(geometry);
            geo_coll.m_m_polys = geo_utils::OGRMulPolyToSillyMulPoly(multiPolygon);
            status = true;
        }
        break;
        case eCompositeType:  // 复合数据类型
        {
            OGRGeometryCollection* geomCollection = dynamic_cast<OGRGeometryCollection*>(geometry);
            int numGeometries = geomCollection->getNumGeometries();
            SU_DEBUG_PRINT("Number of Geometries in Collection: %d \n", numGeometries);
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
            SU_ERROR_PRINT("Unprocessable data types: %s \n", std::to_string(feature_type).c_str());
        }
        break;
    }

    return status;
}

bool geo_utils::read_geo_coll(const char* file, std::vector<silly_geo_coll>& collections)
{
    bool status = false;
    if (!std::filesystem::exists(std::filesystem::path(file)))
    {
        SU_ERROR_PRINT("Error: file does not exist %s  \n", file);
        return status;
    }
    enum_geometry_types type;
    std::map<std::string, enum_geoprop_types> properties;
    if (!check_shp_info(file, type, properties))
    {
        SU_ERROR_PRINT("Error check shp info  \n", file);
        return status;
    }
    // 打开现有 shp 文件
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(file, GDAL_OF_ALL | GDAL_OF_READONLY, NULL, NULL, NULL));
    if (dataset == nullptr)
    {
        // 处理文件打开失败的情况
        SU_ERROR_PRINT("Error: Failed to open shapefile  \n");
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
            SU_ERROR_PRINT("Error: Failed to get layer  \n");
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
            OGRGeometry* geometry = feature->GetGeometryRef();  // 获取矢量数据
            enum_geometry_types feature_type = (enum_geometry_types)wkbFlatten(geometry->getGeometryType());
            temp_geo_coll.m_type = feature_type;                                  // 添加矢量数据类型
            read_property(feature, properties, temp_geo_coll.m_props);            // 读取属性数据
            status = read_all_types_data(feature_type, geometry, temp_geo_coll);  // 添加所有数据类型,如果是复合数据类型会递归的调用
            OGRFeature::DestroyFeature(feature);
            collections.push_back(temp_geo_coll);
        }

    }  // 一个图层结束

    return status;
}

// 根据文件拓展得到对应的存储格式
bool geo_utils::get_driver_name(const char* file, std::string& driverName)
{
    bool status = false;
    // if (!std::filesystem::exists(std::filesystem::path(file)))
    //{
    //     SU_ERROR_PRINT("Error: file does not exist %s ", file);
    //     return status;
    // }
    //  获取文件扩展名
    std::string lowerExtension = std::filesystem::path(file).extension().string();
    // 将文件扩展名转换为小写
    std::transform(lowerExtension.begin(), lowerExtension.end(), lowerExtension.begin(), ::tolower);
    if (lowerExtension == SILLY_SHP_SUFFIX)
    {  // shp
        driverName = SILLY_SHP_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_TAB_SUFFIX)
    {  // tab
        driverName = SILLY_TAB_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_GEOJSON_SUFFIX)
    {  // geojson
        driverName = SILLY_GEOJSON_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_SQLITE_SUFFIX)
    {  // sqlite
        driverName = SILLY_SQLITE_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_CSV_SUFFIX)
    {  // csv
        driverName = SILLY_CSV_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_KML_SUFFIX)
    {  // kml
        driverName = SILLY_KML_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_GML_SUFFIX)
    {  // gml
        driverName = SILLY_GML_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_XLSX_SUFFIX)
    {  // xlsx
        driverName = SILLY_XLSX_DRIVER_NAME;
    }
    else
    {
        driverName = "none";
        return status;
    }
    status = true;
    return status;
}

// 根据enum_geoprop_types 找gdal中属性的类型
OGRFieldType convertToOGRFieldType(enum_geoprop_types geopropType)
{
    switch (geopropType)
    {
        case eNone:
            return OFTString;
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
            return OFTString;
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
            orgLine = geo_utils::SillyLineToOGRLine(geo_coll.m_line);
            geomCollection->addGeometry(orgLine);
            status = true;
        }
        break;
        case ePolygon:
        {
            OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geometry);
            polygon = geo_utils::SillyPolyToOGRPoly(geo_coll.m_poly);
            geomCollection->addGeometry(polygon);
            status = true;
        }
        break;
        case eMultiPoint:
        {
            OGRMultiPoint* multiPoint = dynamic_cast<OGRMultiPoint*>(geometry);
            multiPoint = geo_utils::SillyMulPointToOGRMulPoint(geo_coll.m_m_points);
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
            multiPolygon = geo_utils::SillyMulPolyToOGRMulPoly(geo_coll.m_m_polys);
            geomCollection->addGeometry(multiPolygon);
            status = true;
        }
        break;
        default:
        {
            SU_ERROR_PRINT("Composite unprocessable data types: %s \n", std::to_string(coll_type).c_str());
        }
        break;
    }
    return status;
}

// 写入所有类型的数据
bool wire_all_types_data(const enum_geometry_types coll_type, OGRLayer* outputLayer, OGRFeature* feature, OGRGeometry* geometry, const silly_geo_coll& geo_coll)
{
    bool status = false;
    switch (coll_type)
    {
        case ePoint:
        {
            OGRPoint ogrPoint(geo_coll.m_point.lgtd, geo_coll.m_point.lttd);
            feature->SetGeometry(&ogrPoint);
            if (outputLayer->CreateFeature(feature) != OGRERR_NONE)  // 在图层中创建要素
            {
                SU_ERROR_PRINT("Failed to create feature in shapefile. \n");
            }
            status = true;
        }
        break;
        case eLineString:
        {
            OGRLineString* orgLine = dynamic_cast<OGRLineString*>(geometry);
            orgLine = geo_utils::SillyLineToOGRLine(geo_coll.m_line);
            feature->SetGeometry(orgLine);
            if (outputLayer->CreateFeature(feature) != OGRERR_NONE)  // 在图层中创建要素
            {
                SU_ERROR_PRINT("Failed to create feature in shapefile. \n");
            }
            status = true;
        }
        break;
        case ePolygon:
        {
            OGRPolygon* polygon = dynamic_cast<OGRPolygon*>(geometry);
            polygon = geo_utils::SillyPolyToOGRPoly(geo_coll.m_poly);
            feature->SetGeometry(polygon);
            if (outputLayer->CreateFeature(feature) != OGRERR_NONE)  // 在图层中创建要素
            {
                SU_ERROR_PRINT("Failed to create feature in shapefile. \n");
            }
            status = true;
        }
        break;
        case eMultiPoint:
        {
            OGRMultiPoint* multiPoint = dynamic_cast<OGRMultiPoint*>(geometry);
            multiPoint = geo_utils::SillyMulPointToOGRMulPoint(geo_coll.m_m_points);
            feature->SetGeometry(multiPoint);
            if (outputLayer->CreateFeature(feature) != OGRERR_NONE)  // 在图层中创建要素
            {
                SU_ERROR_PRINT("Failed to create feature in shapefile. \n");
            }
            status = true;
        }
        break;
        case eMultiLineString:
        {
            OGRMultiLineString* multiLineString = dynamic_cast<OGRMultiLineString*>(geometry);
            multiLineString = geo_utils::SillyMulLineToOGRMulLine(geo_coll.m_m_lines);
            feature->SetGeometry(multiLineString);
            if (outputLayer->CreateFeature(feature) != OGRERR_NONE)  // 在图层中创建要素
            {
                SU_ERROR_PRINT("Failed to create feature in shapefile. \n");
            }
            status = true;
        }
        break;
        case eMultiPolygon:
        {
            OGRMultiPolygon* multiPolygon = dynamic_cast<OGRMultiPolygon*>(geometry);
            multiPolygon = geo_utils::SillyMulPolyToOGRMulPoly(geo_coll.m_m_polys);
            feature->SetGeometry(multiPolygon);
            if (outputLayer->CreateFeature(feature) != OGRERR_NONE)  // 在图层中创建要素
            {
                SU_ERROR_PRINT("Failed to create feature in shapefile. \n");
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
            if (outputLayer->CreateFeature(feature) != OGRERR_NONE)  // 在图层中创建要素
            {
                SU_ERROR_PRINT("Failed to create feature in shapefile. \n");
            }
        }
        break;
        default:
        {
            SU_ERROR_PRINT("Unprocessable data types: %s \n", std::to_string(coll_type).c_str());
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
    if (!get_driver_name(file, driverName))
    {
        SU_ERROR_PRINT("Error: Unable to obtain storage method for this type: %s  \n", file);
        return status;
    }
    std::string LayerName = std::filesystem::path(file).filename().stem().string();
    if (collections.empty())
    {
        SU_ERROR_PRINT("Error: Vector data is empty  \n");
        return status;
    }
    GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName(driverName.c_str());
    GDALDataset* outputData = outDriver->Create(file, 0, 0, 0, GDT_Unknown, nullptr);
    if (outputData == nullptr)
    {
        SU_ERROR_PRINT("Error: Failed to create output shapefile  \n");
        return false;
    }
    OGRLayer* outputLayer = outputData->CreateLayer(LayerName.c_str(), nullptr, wkbUnknown, nullptr);
    if (outputLayer == nullptr)
    {
        // 处理图层创建失败的情况
        SU_ERROR_PRINT("Failed to create output layer  \n")
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
            SU_ERROR_PRINT("Error: Failed to create color field  \n");
        }
    }
    for (const auto& coll : collections)
    {
        OGRFeature* feature = OGRFeature::CreateFeature(outputLayer->GetLayerDefn());
        // 添加矢量
        enum_geometry_types coll_type = coll.m_type;
        OGRGeometry* geometry = OGRGeometryFactory::createGeometry((OGRwkbGeometryType)coll_type);
        if (!coll.m_props.empty())
        {
            if (!writePropertiesToGeometry(feature, coll.m_props))  // 添加属性
            {
                SU_ERROR_PRINT("Add attribute fail  \n")
            }
        }
        // 添加矢量数据
        status = wire_all_types_data(coll_type, outputLayer, feature, geometry, coll);

        OGRFeature::DestroyFeature(feature);
        OGRGeometryFactory::destroyGeometry(geometry);
    }

    // 关闭数据集
    GDALClose(outputData);
    SU_INFO_PRINT("Vector data added to shapefile and saved successfully  \n");

    return status;
}