

#include "silly_geo_utils.h"
// GDAL.
#include "ogr_spatialref.h"
// #include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "ogr_api.h"

#include <polyclipping/clipper.hpp>
#include <encode/silly_encode.h>
#include "su_marco.h"
#include "files/silly_file.h"
#include <proj/silly_proj.h>

using namespace ClipperLib;

silly_point geo_utils::poly_centroid(const silly_poly& poly)
{
    silly_point center_point;
    OGRPolygon orgPloy = geo_utils::silly_poly_to_ogr(poly);
    OGRPoint point;
    int err = orgPloy.Centroid(&point);
    if (0 == err)
    {
        center_point.lgtd = point.getX();
        center_point.lttd = point.getY();
    }
    return center_point;
}

double geo_utils::azimuth(silly_point from, silly_point to)
{
    double theta = atan2(to.lgtd - from.lgtd, to.lttd - from.lttd);
    theta = theta * 180.0 / SU_PI;
    return theta;
}

std::string geo_utils::angle_to_desc(const double& angle)
{
    std::string desc;
    if (angle >= -15.0 && angle <= 15.0)
    {
        desc = "北部";
    }
    else if (angle > 15.0 && angle < 75.0)
    {
        desc = "东北部";
    }
    else if (angle >= 75.0 && angle <= 105.0)
    {
        desc = "东部";
    }
    else if (angle > 105.0 && angle < 165.0)
    {
        desc = "东南部";
    }
    else if ((angle >= 165.0 && angle <= 180.0) || (angle >= -180.0 && angle <= -165.0))
    {
        desc = "南部";
    }
    else if (angle > -165.0 && angle < -105.0)
    {
        desc = "西南部";
    }
    else if (angle >= -105.0 && angle <= -75.0)
    {
        desc = "西部";
    }
    else if (angle > -75.0 && angle < -15.0)
    {
        desc = "西北部";
    }
    else
    {
        desc = "局部";
    }
    return desc;
}

// 将 silly_ring 转换为 OGRPolygon
OGRLinearRing geo_utils::silly_ring_to_ogr(const silly_ring& ring)
{
    OGRLinearRing result;
    std::vector<double> xs(ring.points.size());
    std::vector<double> ys(ring.points.size());
    for (int i = 0; i < ring.points.size(); i++)
    {
        xs[i] = ring.points[i].lgtd;
        ys[i] = ring.points[i].lttd;
    }
    result.setPoints(static_cast<int>(ring.points.size()), &xs[0], &ys[0]);
    result.closeRings();
    return result;
}

// 环OGRLinearRing对象，将其转换为silly_ring对象  (环)
silly_ring geo_utils::silly_ring_from_ogr(const OGRLinearRing* ring)
{
    silly_ring result;
    int pointCount = ring->getNumPoints();
    for (int i = 0; i < pointCount; i++)
    {
        double x = ring->getX(i);
        double y = ring->getY(i);
        result.points.push_back({x, y});
    }
    return result;
}

// 将 OGRPoint(单点) 转换为 silly_point(单点) 类型
silly_point geo_utils::silly_point_from_ogr(const OGRPoint* ogrPoint)
{
    silly_point result(ogrPoint->getX(), ogrPoint->getY());
    return result;
}

// 将 silly_point(单点) 转换为 OGRPoint(单点) 类型
OGRPoint geo_utils::silly_point_to_ogr(const silly_point& point)
{
    OGRPoint ogrPoint(point.lgtd, point.lttd);
    return ogrPoint;
}

// 将 OGRMultiPoint(多点) 转换为 silly_multi_point(多点) 类型
silly_multi_point geo_utils::silly_multi_point_from_ogr(const OGRMultiPoint* ogrMultiPoint)
{
    silly_multi_point multiPoint;
    int pointCount = ogrMultiPoint->getNumGeometries();
    for (int i = 0; i < pointCount; i++)
    {
        silly_point sillyPoint = silly_point_from_ogr(ogrMultiPoint->getGeometryRef(i));
        multiPoint.push_back(sillyPoint);
    }
    return multiPoint;
}

// 将 silly_multi_point(多点) 转换为 OGRMultiPoint(多点) 类型
OGRMultiPoint geo_utils::silly_multi_point_to_ogr(const silly_multi_point& mulitPoint)
{
    OGRMultiPoint orgMultiPoint;
    for (const silly_point& point : mulitPoint)
    {
        OGRPoint ogrPoint = silly_point_to_ogr(point);
        orgMultiPoint.addGeometryDirectly(ogrPoint.clone());
    }
    return orgMultiPoint;
}

// OGRLineString(线)类型转为silly_line(线)类型
silly_line geo_utils::silly_line_from_ogr(const OGRLineString* lineString)
{
    silly_line line;
    int num_points = lineString->getNumPoints();
    for (int j = 0; j < num_points; j++)
    {
        line.push_back({lineString->getX(j), lineString->getY(j)});
    }
    return line;
}

// 将 silly_line(线) 转换为 OGRLineString(线)类型
OGRLineString geo_utils::silly_line_to_ogr(const silly_line& line)
{
    OGRLineString ogrLineString;
    std::vector<double> xs(line.size());
    std::vector<double> ys(line.size());
    for (int i = 0; i < line.size(); i++)
    {
        xs[i] = line[i].lgtd;
        ys[i] = line[i].lttd;
    }
    ogrLineString.setPoints((int)line.size(), &xs[0], &ys[0]);
    return ogrLineString;
}

// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
silly_multi_silly_line geo_utils::silly_multi_line_from_ogr(const OGRMultiLineString* multiLineString)
{
    silly_multi_silly_line multiLine;
    int numLines = multiLineString->getNumGeometries();
    for (int i = 0; i < numLines; i++)
    {
        auto lineString = (OGRLineString*)multiLineString->getGeometryRef(i);
        if (lineString != nullptr)
        {
            silly_line line = silly_line_from_ogr(lineString);
            multiLine.push_back(line);
        }
    }
    return multiLine;
}

// 将 silly_multiline(多线) 转换为 OGRMultiLineString(多线)类型
OGRMultiLineString geo_utils::silly_multi_line_to_ogr(const silly_multi_silly_line& multiLine)
{
    OGRMultiLineString ogrMultiLineString;

    for (const silly_line& line : multiLine)
    {
        OGRLineString ogrLineString = silly_line_to_ogr(line);
        ogrMultiLineString.addGeometryDirectly(ogrLineString.clone());
    }

    return ogrMultiLineString;
}

// OGRPolygon对象转换为silly_poly(多环:外环+内环)对象  (单面)
silly_poly geo_utils::silly_poly_from_ogr(const OGRPolygon* polygon)
{
    silly_poly poly;
    // 处理OGRPolygon外环
    auto outerRing = (OGRLinearRing*)polygon->getExteriorRing();
    poly.outer_ring = silly_ring_from_ogr(outerRing);
    // 处理OGRPolygon内环
    int innerRingCount = polygon->getNumInteriorRings();
    for (int k = 0; k < innerRingCount; k++)
    {
        auto ring = (OGRLinearRing*)polygon->getInteriorRing(k);
        silly_ring innerRing = silly_ring_from_ogr(ring);
        poly.inner_rings.push_back(innerRing);
    }
    return poly;
}

// 将 silly_poly 转换为 OGRPolygon(单面)
OGRPolygon geo_utils::silly_poly_to_ogr(const silly_poly& poly)
{
    OGRPolygon org;
    // 设置外环
    OGRLinearRing outerRing = silly_ring_to_ogr(poly.outer_ring);
    // double a = outerRing.get_Area();
    org.addRing(&outerRing);
    // 设置内环
    for (const silly_ring& innerRing : poly.inner_rings)
    {
        OGRLinearRing innerOGRRing = silly_ring_to_ogr(innerRing);
        org.addRing(&innerOGRRing);
        org.closeRings();
    }
    return org;
}

// 多面的OGRMultiPolygon对象转换为silly_multi_poly(多面)
silly_multi_poly geo_utils::silly_multi_poly_from_ogr(const OGRMultiPolygon* multiPolygon)
{
    silly_multi_poly multi_poly;
    int polygonCount = multiPolygon->getNumGeometries();
    for (int i = 0; i < polygonCount; i++)
    {
        silly_poly tmp_poly;
        auto polygon = (OGRPolygon*)multiPolygon->getGeometryRef(i);
        tmp_poly = silly_poly_from_ogr(polygon);
        multi_poly.push_back(tmp_poly);
    }
    return multi_poly;
}

// 将silly_multi_poly对象转换为OGRMultiPolygon对象(多面)
OGRMultiPolygon geo_utils::silly_multi_poly_to_ogr(const silly_multi_poly& multiPoly)
{
    OGRMultiPolygon ogrMultiPolygon;
    for (const silly_poly& poly : multiPoly)
    {
        OGRPolygon ogrPolygon = silly_poly_to_ogr(poly);
        ogrMultiPolygon.addGeometry(&ogrPolygon);
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

bool geo_utils::is_valid_shp(const std::string& u8file)
{
    auto poDSr = (GDALDataset*)GDALOpenEx(u8file.c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr);
    if (nullptr == poDSr)
    {
        return false;
    }
    OGRCleanupAll();
    return true;
}

bool geo_utils::check_shp_info(const std::string& u8file, enum_geometry_type& type, std::map<std::string, silly_geo_prop::enum_prop_type>& properties)
{
    bool status = false;
    std::map<std::string, std::string> result;

    auto poDSr = (GDALDataset*)GDALOpenEx(u8file.c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr);
    if (nullptr == poDSr)
    {
        return status;
    }
    OGRLayer* poLayer_r = poDSr->GetLayer(0);
    if (poLayer_r == nullptr)
    {
        GDALClose(poDSr);
        return status;
    }
    OGRFeature* pFeature_r = nullptr;
    // 获取Feature特征
    pFeature_r = poLayer_r->GetNextFeature();
    if (nullptr == pFeature_r)
    {
        GDALClose(poDSr);
        return status;
    }

    // 获取要素的几何形状
    OGRGeometry* poGeometry_r = pFeature_r->GetGeometryRef();
    auto gdal_type = wkbFlatten(poGeometry_r->getGeometryType());
    type = static_cast<enum_geometry_type>(gdal_type);
    if (wkbUnknown == gdal_type)
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
        OGRFieldType type_ = def->GetType();
        std::string field_name = def->GetNameRef();
        if (!silly_encode::check_text_utf8(field_name.c_str(), field_name.size()))
        {
            field_name = silly_encode::gbk_utf8(field_name);
        }
        silly_geo_prop::enum_prop_type field_type{silly_geo_prop::enum_prop_type::eptNone};
        switch (type_)
        {
            case OFTInteger:
                field_type = silly_geo_prop::enum_prop_type::eptInt;
                break;
            case OFTIntegerList:
                break;
            case OFTReal:
                field_type = silly_geo_prop::enum_prop_type::eptNumeric;
                break;
            case OFTRealList:
                break;
            case OFTString:
                field_type = silly_geo_prop::enum_prop_type::eptString;
                break;
            case OFTStringList:
                break;
#ifndef NDEBUG
                // 弃用 deprecated
            case OFTWideString:
                break;
            case OFTWideStringList:
                break;
#endif
            case OFTBinary:
                field_type = silly_geo_prop::enum_prop_type::eptBinary;
                break;
            case OFTTime:
                field_type = silly_geo_prop::enum_prop_type::eptTime;
                break;
            case OFTDate:
                field_type = silly_geo_prop::enum_prop_type::eptDate;
                break;
            case OFTDateTime:
                field_type = silly_geo_prop::enum_prop_type::eptDateTime;
                break;
            case OFTInteger64:
                field_type = silly_geo_prop::enum_prop_type::eptLong;
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
bool read_property(const OGRFeature* feature, const std::map<std::string, silly_geo_prop::enum_prop_type>& properties, std::map<std::string, silly_geo_prop>& props)
{
    for (const auto& [key, p_type] : properties)
    {
        switch (p_type)
        {
            case silly_geo_prop::enum_prop_type::eptNone:
                break;
            case silly_geo_prop::enum_prop_type::eptInt:
            {
                int value = feature->GetFieldAsInteger(key.c_str());
                props[key] = {value};
            }
            break;
            case silly_geo_prop::enum_prop_type::eptNumeric:
            {
                double value = feature->GetFieldAsDouble(key.c_str());
                props[key] = {value};
            }
            break;
            case silly_geo_prop::enum_prop_type::eptString:
            {
                std::string value = feature->GetFieldAsString(key.c_str());
                props[key] = {value};
            }
            break;
            case silly_geo_prop::enum_prop_type::eptTime:
            case silly_geo_prop::enum_prop_type::eptDate:
            case silly_geo_prop::enum_prop_type::eptDateTime:
            {
                int idx = feature->GetFieldIndex(key.c_str());
                int y = 0, m = 0, d = 0, h = 0, M = 0, s = 0, tzFlag;
                if (feature->GetFieldAsDateTime(idx, &y, &m, &d, &h, &M, &s, &tzFlag))
                {
                    // 成功
                    int a = 0;
                }
                else
                {
                    props[key] = {""};
                }
            }
            break;
            case silly_geo_prop::enum_prop_type::eptLong:
            {
                long long value = feature->GetFieldAsInteger64(key.c_str());
                props[key] = {value};
            }
            break;
            default:
                break;
        }
    }
    return true;
}

bool read_all_types_data(const enum_geometry_type& feature_type, const OGRGeometry* geometry, silly_geo_coll& geo_coll)
{
    bool status = false;
    geo_coll.comp_type.push_back(feature_type);
    switch (feature_type)
    {
        case enum_geometry_type::egtPoint:  // 单点
        {
            auto point = (OGRPoint*)(geometry);
            geo_coll.m_point = geo_utils::silly_point_from_ogr(point);
            status = true;
        }
        break;
        case enum_geometry_type::egtLineString:  // 单线
        {
            auto lineString = (OGRLineString*)(geometry);
            geo_coll.m_line = geo_utils::silly_line_from_ogr(lineString);
            status = true;
        }
        break;
        case enum_geometry_type::egtPolygon:  // 单面
        {
            auto polygon = (OGRPolygon*)(geometry);
            geo_coll.m_poly = geo_utils::silly_poly_from_ogr(polygon);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiPoint:  // 多点
        {
            auto multiPoint = (OGRMultiPoint*)(geometry);
            geo_coll.m_m_points = geo_utils::silly_multi_point_from_ogr(multiPoint);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiLineString:  // 多线
        {
            auto multiLineString = (OGRMultiLineString*)(geometry);
            geo_coll.m_m_lines = geo_utils::silly_multi_line_from_ogr(multiLineString);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiPolygon:  // 多面
        {
            auto multiPolygon = (OGRMultiPolygon*)(geometry);
            geo_coll.m_m_polys = geo_utils::silly_multi_poly_from_ogr(multiPolygon);
            status = true;
        }
        break;
        case enum_geometry_type::egtCompositeType:  // 复合数据类型
        {
            auto geomCollection = (OGRGeometryCollection*)(geometry);
            int numGeometries = geomCollection->getNumGeometries();
            SU_DEBUG_PRINT("Number of Geometries in Collection: %d\n", numGeometries);
            for (int j = 0; j < numGeometries; j++)
            {
                OGRGeometry* collGeometry = geomCollection->getGeometryRef(j);
                if (collGeometry != nullptr)
                {
                    auto feature_type_ = (enum_geometry_type)wkbFlatten(collGeometry->getGeometryType());
                    status = read_all_types_data(feature_type_, collGeometry, geo_coll);
                }
            }
        }
        break;
        default:
        {
            SLOG_ERROR("Unprocessable data types: {}\n", static_cast<int>(feature_type));
        }
        break;
    }

    return status;
}

bool geo_utils::read_geo_coll(const std::string& u8file, std::vector<silly_geo_coll>& collections, const bool& ignore_prop)
{
    bool status = false;

    if (!std::filesystem::exists(std::filesystem::path(u8file)))
    {
        SLOG_ERROR("文件[{}]不存在\n", u8file);
        return status;
    }
    enum_geometry_type type;
    std::map<std::string, silly_geo_prop::enum_prop_type> properties;
    if (!check_shp_info(u8file, type, properties))
    {
        SLOG_ERROR("检查矢量[{}]信息失败\n", u8file);
        return status;
    }
    // 打开现有 shp 文件
    auto dataset = static_cast<GDALDataset*>(GDALOpenEx(u8file.c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        // 处理文件打开失败的情况
        SLOG_ERROR("Error: Failed to open shapefile\n");
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
            SLOG_ERROR("Error: Failed to get layer\n");
            GDALClose(dataset);
            return status;
        }
        layer->ResetReading();
        OGRFeature* feature;
        int64_t iEntities = layer->GetFeatureCount();  // 获取属性的个数,即矢量数据的个数
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
            auto feature_type = (enum_geometry_type)wkbFlatten(geometry->getGeometryType());
            temp_geo_coll.m_type = feature_type;  // 添加矢量数据类型
            if (ignore_prop)
            {
                read_property(feature, properties, temp_geo_coll.m_props);  // 读取属性数据
            }
            status = read_all_types_data(feature_type, geometry, temp_geo_coll);  // 添加所有数据类型,如果是复合数据类型会递归的调用
            OGRFeature::DestroyFeature(feature);
            collections.push_back(temp_geo_coll);
        }

    }  // 一个图层结束

    return status;
}

// 根据文件拓展得到对应的存储格式 TODO :
bool geo_utils::get_driver_name(const std::string& u8file, std::string& driver_name)
{
    bool status = true;
    // if (!std::filesystem::exists(std::filesystem::path(u8file)))
    //{
    //     SLOG_ERROR("Error: u8file does not exist %s ", u8file);
    //     return status;
    // }
    //  获取文件扩展名
    std::string lowerExtension = std::filesystem::path(u8file.c_str()).extension().string();
    // 将文件扩展名转换为小写
    std::transform(lowerExtension.begin(), lowerExtension.end(), lowerExtension.begin(), ::tolower);
    if (lowerExtension == SILLY_SHP_SUFFIX)
    {  // shp
        driver_name = SILLY_SHP_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_TAB_SUFFIX)
    {  // tab
        driver_name = SILLY_TAB_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_GEOJSON_SUFFIX)
    {  // geojson
        driver_name = SILLY_GEOJSON_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_SQLITE_SUFFIX)
    {  // sqlite
        driver_name = SILLY_SQLITE_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_CSV_SUFFIX)
    {  // csv
        driver_name = SILLY_CSV_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_KML_SUFFIX)
    {  // kml
        driver_name = SILLY_KML_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_GML_SUFFIX)
    {  // gml
        driver_name = SILLY_GML_DRIVER_NAME;
    }
    else if (lowerExtension == SILLY_XLSX_SUFFIX)
    {  // xlsx
        driver_name = SILLY_XLSX_DRIVER_NAME;
    }
    else
    {
        driver_name = "";
        status = false;
    }
    return status;
}

// 根据silly_geo_prop::enum_prop_type 找gdal中属性的类型
OGRFieldType convertToOGRFieldType(const silly_geo_prop::enum_prop_type& type)
{
    OGRFieldType result = OFTString;
    switch (type)
    {
        case silly_geo_prop::enum_prop_type::eptNone:
            result = OFTString;
            break;
        case silly_geo_prop::enum_prop_type::eptInt:
            result = OFTInteger;
            break;
        case silly_geo_prop::enum_prop_type::eptNumeric:
            result = OFTReal;
            break;
        case silly_geo_prop::enum_prop_type::eptString:
            result = OFTString;
            break;
        case silly_geo_prop::enum_prop_type::eptBinary:
            result = OFTBinary;
            break;
        case silly_geo_prop::enum_prop_type::eptTime:
            result = OFTTime;
            break;
        case silly_geo_prop::enum_prop_type::eptDate:
            result = OFTDate;
            break;
        case silly_geo_prop::enum_prop_type::eptDateTime:
            result = OFTDateTime;
            break;
        case silly_geo_prop::enum_prop_type::eptLong:
            result = OFTInteger64;
            break;
        default:
            result = OFTString;
            break;
    }
    return result;
}

// 添加属性到shp中
bool writePropertiesToGeometry(OGRFeature* feature, const std::map<std::string, silly_geo_prop>& m_props)
{
    bool status = true;
    for (const auto& [key, prop] : m_props)
    {
        int fieldIndex = feature->GetFieldIndex(key.c_str());
        if (fieldIndex >= 0)
        {
            switch (prop.value_type())
            {
                case silly_geo_prop::enum_prop_type::eptInt:
                    feature->SetField(fieldIndex, prop.as_int());
                    break;
                case silly_geo_prop::enum_prop_type::eptNumeric:
                    feature->SetField(fieldIndex, prop.as_double());
                    break;
                case silly_geo_prop::enum_prop_type::eptString:
                    feature->SetField(fieldIndex, prop.as_string().c_str());
                    break;
                case silly_geo_prop::enum_prop_type::eptTime:
                    break;
                case silly_geo_prop::enum_prop_type::eptDate:
                    break;
                case silly_geo_prop::enum_prop_type::eptDateTime:
                    break;
                case silly_geo_prop::enum_prop_type::eptLong:
                    feature->SetField(fieldIndex, prop.as_longlong());
                    break;
                default:
                    status = false;
                    break;
            }
        }
    }
    return status;
}

// 处理复合数据类型的变量
bool process_composite_data(const enum_geometry_type coll_type, OGRGeometry* geometry, OGRGeometryCollection* geomCollection, const silly_geo_coll& geo_coll)
{
    bool status = true;
    switch (coll_type)
    {
        case enum_geometry_type::egtPoint:
        {
            OGRPoint ogrPoint(geo_coll.m_point.lgtd, geo_coll.m_point.lttd);
            geomCollection->addGeometry(&ogrPoint);
        }
        break;
        case enum_geometry_type::egtLineString:
        {
            OGRLineString orgLine = geo_utils::silly_line_to_ogr(geo_coll.m_line);
            geomCollection->addGeometry(&orgLine);
        }
        break;
        case enum_geometry_type::egtPolygon:
        {
            OGRPolygon polygon = geo_utils::silly_poly_to_ogr(geo_coll.m_poly);
            geomCollection->addGeometry(&polygon);
        }
        break;
        case enum_geometry_type::egtMultiPoint:
        {
            OGRMultiPoint multiPoint = geo_utils::silly_multi_point_to_ogr(geo_coll.m_m_points);
            geomCollection->addGeometry(&multiPoint);
        }
        break;
        case enum_geometry_type::egtMultiLineString:
        {
            OGRMultiLineString multiLineString = geo_utils::silly_multi_line_to_ogr(geo_coll.m_m_lines);
            geomCollection->addGeometry(&multiLineString);
        }
        break;
        case enum_geometry_type::egtMultiPolygon:
        {
            OGRMultiPolygon multiPolygon = geo_utils::silly_multi_poly_to_ogr(geo_coll.m_m_polys);
            geomCollection->addGeometry(&multiPolygon);
        }
        break;
        default:
            status = false;
            break;
    }
    return status;
}

// 写入所有类型的数据
static bool wire_all_types_data(const enum_geometry_type coll_type, OGRLayer* outputLayer, OGRFeature* feature, OGRGeometry* geometry, const silly_geo_coll& geo_coll)
{
    bool status = true;
    switch (coll_type)
    {
        case enum_geometry_type::egtPoint:
        {
            OGRPoint ogrPoint(geo_coll.m_point.lgtd, geo_coll.m_point.lttd);
            feature->SetGeometry(&ogrPoint);
        }
        break;
        case enum_geometry_type::egtLineString:
        {
            OGRLineString orgLine = silly_geo_utils::silly_line_to_ogr(geo_coll.m_line);
            feature->SetGeometry(&orgLine);
        }
        break;
        case enum_geometry_type::egtPolygon:
        {
            OGRPolygon polygon = geo_utils::silly_poly_to_ogr(geo_coll.m_poly);
            feature->SetGeometry(&polygon);
        }
        break;
        case enum_geometry_type::egtMultiPoint:
        {
            OGRMultiPoint multiPoint = geo_utils::silly_multi_point_to_ogr(geo_coll.m_m_points);
            feature->SetGeometry(&multiPoint);
        }
        break;
        case enum_geometry_type::egtMultiLineString:
        {
            OGRMultiLineString multiLineString = geo_utils::silly_multi_line_to_ogr(geo_coll.m_m_lines);
            feature->SetGeometry(&multiLineString);
        }
        break;
        case enum_geometry_type::egtMultiPolygon:
        {
            OGRMultiPolygon multiPolygon = geo_utils::silly_multi_poly_to_ogr(geo_coll.m_m_polys);
            feature->SetGeometry(&multiPolygon);
        }
        break;
        case enum_geometry_type::egtCompositeType:
        {
            OGRGeometryCollection geomCollection;
            for (const auto& type : geo_coll.comp_type)
            {
                status = process_composite_data(type, geometry, &geomCollection, geo_coll);
            }
            feature->SetGeometry(&geomCollection);
        }
        break;
        default:
            status = false;
            break;
    }
    if (outputLayer->CreateFeature(feature) != OGRERR_NONE)  // 在图层中创建要素
    {
        status = false;
    }
    return status;
}

bool geo_utils::write_geo_coll(const std::string& u8file, const std::vector<silly_geo_coll>& collections)
{
    bool status = false;
    if (collections.empty())
    {
        return status;
    }
    // 根据拓展名得到存储格式
    std::string driver_name;
    if (!get_driver_name(u8file, driver_name))
    {
        SLOG_ERROR("Error: Unable to obtain storage method for this type: %s\n", u8file);
        return status;
    }
    std::string LayerName = std::filesystem::path(u8file).filename().stem().string();
    if (collections.empty())
    {
        SLOG_ERROR("Error: Vector data is empty\n");
        return status;
    }
    GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName(driver_name.c_str());
#if IS_WIN32
    std::string _file_name = u8file;
    if (silly_encode::is_utf8(_file_name))
    {
        _file_name = silly_encode::utf8_gbk(_file_name);
    }
    GDALDataset* outputData = outDriver->Create(_file_name.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
#else
    GDALDataset* outputData = outDriver->Create(u8file.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
#endif
    if (outputData == nullptr)
    {
        SLOG_ERROR("Error: Failed to create output shapefile\n");
        return false;
    }
    OGRLayer* outputLayer = outputData->CreateLayer(LayerName.c_str(), nullptr, wkbUnknown, nullptr);
    if (outputLayer == nullptr)
    {
        // 处理图层创建失败的情况
        SLOG_ERROR("Failed to create output layer\n")
        GDALClose(outputData);
        return false;
    }
    // TODO :

    for (const auto& [k, p] : collections.front().m_props)  // 添加属性
    {
        OGRFieldType ogrType = convertToOGRFieldType(p.value_type());
        OGRFieldDefn fieldDef(k.c_str(), ogrType);
        if (outputLayer->CreateField(&fieldDef) != OGRERR_NONE)
        {
            SLOG_ERROR("Error: Failed to create color field\n");
        }
    }
    for (const auto& coll : collections)
    {
        OGRFeature* feature = OGRFeature::CreateFeature(outputLayer->GetLayerDefn());
        // 添加矢量
        enum_geometry_type coll_type = coll.m_type;
        OGRGeometry* geometry = OGRGeometryFactory::createGeometry((OGRwkbGeometryType)coll_type);
        if (!coll.m_props.empty())
        {
            if (!writePropertiesToGeometry(feature, coll.m_props))  // 添加属性
            {
                SLOG_ERROR("Add attribute fail\n")
            }
        }
        // 添加矢量数据
        status = wire_all_types_data(coll_type, outputLayer, feature, geometry, coll);

        OGRFeature::DestroyFeature(feature);
        OGRGeometryFactory::destroyGeometry(geometry);
    }

    // 关闭数据集
    GDALClose(outputData);
    SU_INFO_PRINT("Vector data added to shapefile and saved successfully\n");

    return status;
}
bool silly_geo_utils::intersect(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2)
{
    // TODO:
    return false;
}
std::vector<silly_poly> silly_geo_utils::intersection(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2)
{
    std::vector<silly_poly> result;

    // 创建 OGRPolygon 对象
    OGRMultiPolygon org_ploy_1 = geo_utils::silly_multi_poly_to_ogr(mpoly1);
    OGRMultiPolygon org_ploy_2 = geo_utils::silly_multi_poly_to_ogr(mpoly2);

    /*// 判断两个 OGRPolygon 是否相交
    if (!org_ploy_1.Intersects(&org_ploy_2))
    {
        return result;
    }*/

    // 计算相交区域
    OGRGeometry* intersection = org_ploy_1.Intersection(&org_ploy_2);

    // 处理不同几何类型的情况
    OGRwkbGeometryType geometryType = intersection->getGeometryType();
    switch (geometryType)
    {
        // 单面
        case wkbPolygon:
        case wkbPolygon25D:
        {
            auto intersectingPolygon = (OGRPolygon*)(intersection);
            result.emplace_back(geo_utils::silly_poly_from_ogr(intersectingPolygon));
            break;
        }
        // 多面
        case wkbMultiPolygon:
        case wkbMultiPolygon25D:
        {
            auto intersectingMultiPolygon = (OGRMultiPolygon*)(intersection);
            auto m_polys = geo_utils::silly_multi_poly_from_ogr(intersectingMultiPolygon);
            for (const auto& poly : m_polys)
            {
                result.emplace_back(poly);
            }
            break;
        }
        default:
            break;
    }

    return result;
}


// 判断点是否在线段上
bool on_segment(const silly_point& point, const silly_point& l_beg, const silly_point& l_end)
{
    return std::min(l_beg.lgtd, l_end.lgtd) <= point.lgtd && point.lgtd <= std::max(l_beg.lgtd, l_end.lgtd) && std::min(l_beg.lttd, l_end.lttd) <= point.lttd && point.lttd <= std::max(l_beg.lttd, l_end.lttd);
}

// 计算两条线段相交点
silly_point line_intersection(const silly_point& p1, const silly_point& p2, const silly_point& q1, const silly_point& q2)
{
    // 利用直线方程求解交点
    double a1 = p2.lttd - p1.lttd;
    double b1 = p1.lgtd - p2.lgtd;
    double c1 = a1 * p1.lgtd + b1 * p1.lttd;
    double a2 = q2.lttd - q1.lttd;
    double b2 = q1.lgtd - q2.lgtd;
    double c2 = a2 * q1.lgtd + b2 * q1.lttd;
    double determinant = a1 * b2 - a2 * b1;
    if (fabs(determinant) < 1e-6)
    {
        return silly_point();  // 没有交点或线段重叠
    }
    double x = (b2 * c1 - b1 * c2) / determinant;
    double y = (a1 * c2 - a2 * c1) / determinant;
    if (on_segment(silly_point(x, y), p1, p2) && on_segment(silly_point(x, y), q1, q2))
    {
        return silly_point(x, y);
    }
    return silly_point();
}


// 两条线段所在的区域是否有重叠部分
bool segments_overlap(double xa, double xb, double ya, double yb)
{
    return std::max(xa, xb) >= std::min(ya, yb) && std::max(ya, yb) >= std::min(xa, xb);
}

// 计算线段是否相交
bool segments_intersect(silly_point a1, silly_point a2, silly_point b1, silly_point b2)
{
    // 快速排斥测试
    // 两条线段的覆盖入去要有重叠部分, a线段最大的x要大于b线段最小的x, a线段最小的x要小于b线段最大的x,y轴同理
    if (!segments_overlap(a1.lgtd, a2.lgtd, b1.lgtd, b2.lgtd) || !segments_overlap(a1.lttd, a2.lttd, b1.lttd, b2.lttd))
    {
        return false;
    }

    // 计算方向
    //d1 计算了线段 b1-b2 的方向相对于点 a1 的位置
    //d2 计算了线段 b1-b2 的方向相对于点 a2 的位置
    //d3 计算了线段 a1-a2 的方向相对于点 b1 的位置
    //d4 计算了线段 a1-a2 的方向相对于点 b2 的位置
    double d1 = (b2.lttd - b1.lttd) * (a1.lgtd - b1.lgtd) - (b2.lgtd - b1.lgtd) * (a1.lttd - b1.lttd);
    double d2 = (b2.lttd - b1.lttd) * (a2.lgtd - b1.lgtd) - (b2.lgtd - b1.lgtd) * (a2.lttd - b1.lttd);
    double d3 = (a2.lttd - a1.lttd) * (b1.lgtd - a1.lgtd) - (a2.lgtd - a1.lgtd) * (b1.lttd - a1.lttd);
    double d4 = (a2.lttd - a1.lttd) * (b2.lgtd - a1.lgtd) - (a2.lgtd - a1.lgtd) * (b2.lttd - a1.lttd);

    // 当 d1 * d2 < 0 时，意味着点 a1 和 a2 在线段 b1-b2 的两侧
    // 当 d3 * d4 < 0 时，意味着点 b1 和 b2 在线段 a1-a2 的两侧 
    // 如果两个条件都满足，那么线段 a1-a2 和 b1-b2 是相交的,但是如果有一个满足可能是有一点重合,

    // 平行或共线的情况
    if (d1 * d2 == 0 && d3 * d4 == 0)
    {
        // 判断点是否在线段上 a1 是否在 b1-b2 上
        if (on_segment(a1, b1, b2))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if ((d1 * d2 < 0) && (d3 * d4 < 0)) // 相交
    {
        return true;
    }
    else // 线段a上的一个点在线段b上,或者线段b的延长线上
    {
        // 求相交点,如果相交点为线段的起点返回true
        silly_point intersect = line_intersection(a1, a2, b1, b2);
        // 如果交点为线段b的起始点
        if ((std::abs(intersect.lgtd - b1.lgtd) <= 1e-8) && (std::abs(intersect.lttd - b1.lttd) <= 1e-8))
        {
            return true;
        }
        else
        {
            return false;
        }

        int a = 0;

    }
    
}

// 判断点是否在环内
bool isPointInRing(const silly_point& point, const silly_ring& ring)
{
    int intersections = 0;
    silly_point ray_end(point.lgtd + 10000, point.lttd);  // 向右引一条射线  假设地图宽度不超过10000单位

    // 外环
    for (size_t i = 0, n = ring.points.size(); i < n; ++i)
    {
        const silly_point& start = ring.points[i];
        const silly_point& end = ring.points[(i + 1) % n];
        if (start == end)
        {
            continue;
        }
        if (point == start || point == end)
        {
            return true;
        }
        if (segments_intersect(point, ray_end, start, end))
        {
            intersections++;
        }
    }

    return intersections % 2 != 0;
}

bool silly_geo_utils::intersect(const silly_poly& mpoly, const silly_point& point)
{
    int intersections = 0;
    silly_point ray_end(point.lgtd + 1000, point.lttd);  // 向右引一条射线 1000单位

    // 外环
    bool is_in_outer_ring = isPointInRing(point, mpoly.outer_ring);
    if (is_in_outer_ring)
    {
        // 内环
        for (const auto& inner : mpoly.inner_rings)
        {
            if (isPointInRing(point, inner)) // 在内环内
            {
                return false; // 如果这个点在一个内环内就属于在面外
            }
        }
        return true; // 点在外环内,且不在任何一个内环内
    }
    else
    {
        return false;
    }
}

bool silly_geo_utils::intersect(const silly_multi_poly& mpoly, const silly_point& point)
{
    // TODO:
    return false;
}
bool silly_geo_utils::intersect(const silly_multi_poly& mpoly, const silly_line& line)
{
    // TODO:
    return false;
}
bool silly_geo_utils::nearby(const silly_point& point, const silly_line& line, const double& dist)
{
    // TODO:
    return false;
}
std::vector<silly_line> silly_geo_utils::intersection(const silly_multi_poly& mpoly, const silly_line& line)
{
    // TODO:
    return std::vector<silly_line>();
}
double silly_geo_utils::area(const std::vector<silly_point>& points)
{
    double result = 0.0;
    size_t pnum = points.size();
    // 确保至少有3个点才能构成一个多边形
    if (pnum < 3)
    {
        return 0.0;
    }

    for (size_t i = 0; i < pnum; ++i)
    {
        size_t j = (i + 1) % pnum;
        result += points[i].lgtd * points[j].lttd;
        result -= points[j].lgtd * points[i].lttd;
    }
    return std::abs(result) / 2.0;
}
double silly_geo_utils::area(const silly_poly& poly)
{
    double total_area = area(poly.outer_ring.points);
    if (total_area < 1.E-15)
    {
        return total_area;
    }

    for (const auto& inner_ring : poly.inner_rings)
    {
        total_area -= area(inner_ring.points);
    }

    return total_area;
}
double silly_geo_utils::area_sqkm(const silly_poly& poly)
{
    double total_area = area_sqkm(poly.outer_ring.points);
    if (total_area < 1.E-15)
    {
        return total_area;
    }
    for (const auto& inner_ring : poly.inner_rings)
    {
        total_area -= area_sqkm(inner_ring.points);
    }
    return total_area;
}
double silly_geo_utils::area(const silly_multi_poly& mpoly)
{
    double total_area = 0;
    for (const auto& poly : mpoly)
    {
        total_area += area(poly);
    }
    return total_area;
}
double silly_geo_utils::area_sqkm(const silly_multi_poly& mpoly)
{
    double total_area = 0;
    for (const auto& poly : mpoly)
    {
        total_area += area_sqkm(poly);
    }
    return total_area;
}
std::vector<silly_poly> silly_geo_utils::trans_intersection(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2)
{
    std::vector<silly_poly> result;
    // TODO:
    return result;
}
std::vector<silly_line> silly_geo_utils::trans_intersection(const silly_multi_poly& mpoly1, const silly_line& line)
{
    std::vector<silly_line> result;
    // TODO:
    return result;
}
double silly_geo_utils::area_sqkm(const std::vector<silly_point>& points)
{
    double maxx = -1e10, minx = 1e10;
    for (auto p : points)
    {
        maxx = std::max(maxx, p.lgtd);
        minx = std::min(minx, p.lgtd);
    }
    std::vector<silly_point> gpoints;
    auto central = SU_GAUSS6_L0(SU_GAUSS6_NO((minx + maxx) / 2));
    for (auto p : points)
    {
        silly_point tmp;
        silly_proj::lonlat_to_gauss(central, p.lgtd, p.lttd, tmp.lttd, tmp.lgtd);
        gpoints.push_back(tmp);
    }
    return area(gpoints) / 1e6;
}
std::vector<silly_point> silly_geo_utils::smooth_line(const std::vector<silly_point>& line, const int& mod, const int& interp)
{
    return std::vector<silly_point>();
}
std::vector<silly_point> silly_geo_utils::smooth_ring(const std::vector<silly_point>& ring, const int& mod, const int& interp)
{
    return std::vector<silly_point>();
}
std::vector<silly_point> silly_geo_utils::simplify_line(const std::vector<silly_point>& line, const double& dist)
{
    return std::vector<silly_point>();
}
std::vector<silly_point> silly_geo_utils::simplify_ring(const std::vector<silly_point>& ring, const double& dist)
{
    return std::vector<silly_point>();
}
bool silly_geo_utils::read_iso_polygon(const std::string& u8file, silly_multi_poly& mpoly)
{
    bool status = false;
    try
    {
        std::string content;
        if (0 == silly_file::read(u8file, content))
        {
            return status;
        }
        std::stringstream sstream(content);
        int part_num = 0;
        int point_num = 0;
        sstream >> part_num;

        if (part_num <= 0)
        {
            return status;
        }
        std::vector<silly_ring> rings;

        for (int i = 0; i < part_num; ++i)
        {
            sstream >> point_num;
            silly_ring ring;
            while (point_num && !sstream.eof())
            {
                double x, y;
                sstream >> x >> y;
                ring.points.emplace_back(x, y);
                point_num--;
            }

            if (point_num)
            {
                return status;
            }
            rings.push_back(ring);
        }

        if (rings.empty())
        {
            return status;
        }

        // 检查内外环
        silly_multi_poly tmp;
        tmp.resize(200);
        tmp[0].outer_ring = rings[0];
        int poly_num = 1;
        for (int i = 1; i < rings.size(); ++i)
        {
            if (rings[i].points.empty())
            {
                continue;
            }
            bool is_outer = true;
            for (auto& poly : tmp)
            {
                if (intersect(rings[i].points.front(), poly.outer_ring.points))
                {
                    poly.inner_rings.push_back(rings[i]);
                    is_outer = false;
                    break;
                }
            }
            if (is_outer)
            {
                tmp[poly_num].outer_ring = rings[i];
                poly_num++;
            }
        }
        for (int i = 0; i < poly_num; ++i)
        {
            mpoly.push_back(tmp[i]);
        }

        status = true;
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return status;
    }

    return status;
}
bool silly_geo_utils::write_iso_polygon(const std::string& u8file, const silly_multi_poly& mpoly, const int& precision)
{
    bool status = false;
    int part_num = mpoly.size();
    try
    {
        std::ofstream ofs(u8file);
        if (!ofs.is_open())
        {
            return false;
        }
        ofs << part_num;
        for (auto& poly : mpoly)
        {
            ofs << " " << poly.outer_ring.points.size();
            for (auto& p : poly.outer_ring.points)
            {
                ofs << " " << std::fixed << std::setprecision(precision) << p.lgtd;
                ofs << " " << std::fixed << std::setprecision(precision) << p.lttd;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return status;
    }
    return status;
}
bool silly_geo_utils::intersect(const silly_point& point, const std::vector<silly_point>& points)
{
    int i, j;
    double d;
    int num = points.size();
    bool c = false;
    for (i = 0, j = num - 1; i < num; j = i++)
    {
        d = (points[j].lgtd - points[i].lgtd) * (point.lttd - points[i].lttd) / (points[j].lttd - points[i].lttd) + points[i].lgtd;
        if (point.lgtd == d)
        {
            return false;
        }

        if ((((points[i].lttd <= point.lttd) && (point.lttd < points[j].lttd) || ((points[j].lttd <= point.lttd) && (point.lttd < points[i].lttd))) && (point.lgtd < d)))
        {
            c = !c;
        }
    }
    return c;
}
