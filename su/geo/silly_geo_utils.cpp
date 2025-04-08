

#include "silly_geo_utils.h"
#if ENABLE_GDAL
// GDAL.
#include "ogr_spatialref.h"
// #include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "gdal_alg.h"
#include "ogr_api.h"
#endif

#include <polyclipping/clipper.hpp>
#include <encode/silly_encode.h>
#include <files/silly_file.h>
#include <geo/proj/silly_proj.h>
using namespace silly::geo;
using namespace ClipperLib;

silly_point utils::poly_centroid(const silly_poly& poly)
{
    silly_point center_point;
#if ENABLE_GDAL
    OGRPolygon orgPloy = utils::silly_poly_to_ogr(poly);
    OGRPoint point;
    int err = orgPloy.Centroid(&point);
    if (0 == err)
    {
        center_point.x = point.getX();
        center_point.y = point.getY();
    }
#endif
    return center_point;
}

double utils::azimuth(silly_point from, silly_point to)
{
    double theta = atan2(to.x - from.x, to.y - from.y);
    theta = theta * 180.0 / SU_PI;
    return theta;
}

std::string utils::angle_to_desc(const double& angle)
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
#if ENABLE_GDAL
// 将 silly_ring 转换为 OGRPolygon
OGRLinearRing utils::silly_ring_to_ogr(const silly_ring& ring)
{
    OGRLinearRing result;
    std::vector<double> xs(ring.points.size());
    std::vector<double> ys(ring.points.size());
    for (int i = 0; i < ring.points.size(); i++)
    {
        xs[i] = ring.points[i].x;
        ys[i] = ring.points[i].y;
    }
    result.setPoints(static_cast<int>(ring.points.size()), &xs[0], &ys[0]);
    result.closeRings();
    return result;
}

// 环OGRLinearRing对象，将其转换为silly_ring对象  (环)
silly_ring utils::silly_ring_from_ogr(const OGRLinearRing* ring)
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
silly_point utils::silly_point_from_ogr(const OGRPoint* ogrPoint)
{
    silly_point result(ogrPoint->getX(), ogrPoint->getY());
    return result;
}

// 将 silly_point(单点) 转换为 OGRPoint(单点) 类型
OGRPoint utils::silly_point_to_ogr(const silly_point& point)
{
    OGRPoint ogrPoint(point.x, point.y);
    return ogrPoint;
}

// 将 OGRMultiPoint(多点) 转换为 silly_multi_point(多点) 类型
silly_multi_point utils::silly_multi_point_from_ogr(const OGRMultiPoint* ogrMultiPoint)
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
OGRMultiPoint utils::silly_multi_point_to_ogr(const silly_multi_point& mulitPoint)
{
    OGRMultiPoint orgMultiPoint;
    for (const silly_point& point : mulitPoint)
    {
        OGRPoint ogrPoint = silly_point_to_ogr(point);
        // orgMultiPoint.addGeometryDirectly(ogrPoint.clone());
        orgMultiPoint.addGeometry(&ogrPoint);
    }
    return orgMultiPoint;
}

// OGRLineString(线)类型转为silly_line(线)类型
silly_line utils::silly_line_from_ogr(const OGRLineString* lineString)
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
OGRLineString utils::silly_line_to_ogr(const silly_line& line)
{
    OGRLineString ogrLineString;
    std::vector<double> xs(line.size());
    std::vector<double> ys(line.size());
    for (int i = 0; i < line.size(); i++)
    {
        xs[i] = line[i].x;
        ys[i] = line[i].y;
    }
    ogrLineString.setPoints((int)line.size(), &xs[0], &ys[0]);
    return ogrLineString;
}

// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
silly_multi_line utils::silly_multi_line_from_ogr(const OGRMultiLineString* multiLineString)
{
    silly_multi_line multiLine;
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
OGRMultiLineString utils::silly_multi_line_to_ogr(const silly_multi_line& multiLine)
{
    OGRMultiLineString ogrMultiLineString;

    for (const silly_line& line : multiLine)
    {
        OGRLineString ogrLineString = silly_line_to_ogr(line);
        ogrMultiLineString.addGeometry(&ogrLineString);
    }

    return ogrMultiLineString;
}

// OGRPolygon对象转换为silly_poly(多环:外环+内环)对象  (单面)
silly_poly utils::silly_poly_from_ogr(const OGRPolygon* polygon)
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
OGRPolygon utils::silly_poly_to_ogr(const silly_poly& poly)
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
silly_multi_poly utils::silly_multi_poly_from_ogr(const OGRMultiPolygon* multiPolygon)
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
OGRMultiPolygon utils::silly_multi_poly_to_ogr(const silly_multi_poly& multiPoly)
{
    OGRMultiPolygon ogrMultiPolygon;
    for (const silly_poly& poly : multiPoly)
    {
        OGRPolygon ogrPolygon = silly_poly_to_ogr(poly);
        ogrMultiPolygon.addGeometry(&ogrPolygon);
    }

    return ogrMultiPolygon;
}

OGRGeometry* utils::silly_geo_coll_to_ogr(const silly_geo_coll& coll)
{
    switch (coll.m_type)
    {
        case enum_geometry_type::egtPoint:
            return new OGRPoint(utils::silly_point_to_ogr(coll.m_point));

        case enum_geometry_type::egtMultiPoint:
            return new OGRMultiPoint(utils::silly_multi_point_to_ogr(coll.m_m_points));

        case enum_geometry_type::egtLineString:
            return new OGRLineString(utils::silly_line_to_ogr(coll.m_line));

        case enum_geometry_type::egtMultiLineString:
            return new OGRMultiLineString(utils::silly_multi_line_to_ogr(coll.m_m_lines));

        case enum_geometry_type::egtPolygon:
            return new OGRPolygon(utils::silly_poly_to_ogr(coll.m_poly));

        case enum_geometry_type::egtMultiPolygon:
            return new OGRMultiPolygon(utils::silly_multi_poly_to_ogr(coll.m_m_polys));

        default:
            SLOG_ERROR("Error: Unsupported type: {}");
            return nullptr;
    }
}

silly_geo_coll utils::silly_geo_coll_from_ogr(const OGRGeometry* geometry)
{
    switch (geometry->getGeometryType())
    {
        case wkbPoint:
            return silly_point_from_ogr(static_cast<const OGRPoint*>(geometry));
        case wkbMultiPoint:
            return silly_multi_point_from_ogr(static_cast<const OGRMultiPoint*>(geometry));
        case wkbLineString:
            return silly_line_from_ogr(static_cast<const OGRLineString*>(geometry));
        case wkbMultiLineString:
            return silly_multi_line_from_ogr(static_cast<const OGRMultiLineString*>(geometry));
        case wkbPolygon:
            return silly_poly_from_ogr(static_cast<const OGRPolygon*>(geometry));
        case wkbMultiPolygon:
            return silly_multi_poly_from_ogr(static_cast<const OGRMultiPolygon*>(geometry));
        default:
            SLOG_ERROR("Error: Unsupported type: {}");
            return silly_geo_coll();
    }
}
#endif

void utils::init_gdal_env()
{
#if ENABLE_GDAL
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    OGRRegisterAll();
    CPLSetConfigOption("SHAPE_ENCODING", "");
#endif
}

void utils::destroy_gdal_env()
{
#if ENABLE_GDAL
    OGRCleanupAll();
#endif
}

bool utils::is_valid_shp(const std::string& u8file)
{
#if ENABLE_GDAL
    auto poDSr = (GDALDataset*)GDALOpenEx(u8file.c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr);
    if (nullptr == poDSr)
    {
        return false;
    }
    GDALClose(poDSr);
    return true;
#endif
    return false;
}

std::vector<std::string> utils::shp_missing_file(const std::string& u8file)
{
    std::vector<std::string> ret;
    // 获取文件的父级目录和文件名不包含后缀名
    std::filesystem::path sfp_shp(u8file);
    if (!std::filesystem::exists(sfp_shp) || sfp_shp.extension() != ".shp")
    {
        return ret;
    }
    // std::filesystem::path shp_parent_dir = sfp_shp.parent_path();
    std::string name = sfp_shp.stem().string();

    // 判断同级目录是否有 还存在 同名的 .dbf .shx 文件
    std::filesystem::path shx = sfp_shp.parent_path().append(name + ".shx");  // 集合索引
    std::filesystem::path dbf = sfp_shp.parent_path().append(name + ".dbf");  // 属性信息
    if (!std::filesystem::exists(shx))
    {
        ret.push_back(shx.filename().string());
    }
    if (!std::filesystem::exists(dbf))
    {
        ret.push_back(dbf.filename().string());
    }

    return ret;
}

bool utils::check_shp_info(const std::string& u8file, enum_geometry_type& type, std::map<std::string, silly_geo_prop::enum_prop_type>& properties)
{
    bool status = false;
#if ENABLE_GDAL
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
#endif
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
#if ENABLE_GDAL
    for (const auto& [key, p_type] : properties)
    {
        std::string utf8_key = key;
        if (!silly_encode::is_utf8(utf8_key))
        {
            utf8_key = silly_encode::gbk_utf8(utf8_key);
        }
        switch (p_type)
        {
            case silly_geo_prop::enum_prop_type::eptNone:
                break;
            case silly_geo_prop::enum_prop_type::eptInt:
            {
                int value = feature->GetFieldAsInteger(key.c_str());
                props[utf8_key] = {value};
            }
            break;
            case silly_geo_prop::enum_prop_type::eptNumeric:
            {
                double value = feature->GetFieldAsDouble(key.c_str());
                props[utf8_key] = {value};
            }
            break;
            case silly_geo_prop::enum_prop_type::eptString:
            {
                std::string value = feature->GetFieldAsString(key.c_str());
                if (!silly_encode::is_utf8(value))
                {
                    value = silly_encode::gbk_utf8(value);
                }
                props[utf8_key] = {value};
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
                    props[utf8_key] = {""};
                }
            }
            break;
            case silly_geo_prop::enum_prop_type::eptLong:
            {
                long long value = feature->GetFieldAsInteger64(key.c_str());
                props[utf8_key] = {value};
            }
            break;
            default:
                break;
        }
    }
#endif
    return true;
}

bool read_all_types_data(const enum_geometry_type& feature_type, const OGRGeometry* geometry, silly_geo_coll& geo_coll)
{
    bool status = false;
#if ENABLE_GDAL
    geo_coll.comp_type.push_back(feature_type);
    switch (feature_type)
    {
        case enum_geometry_type::egtPoint:  // 单点
        {
            auto point = (OGRPoint*)(geometry);
            geo_coll.m_point = utils::silly_point_from_ogr(point);
            status = true;
        }
        break;
        case enum_geometry_type::egtLineString:  // 单线
        {
            auto lineString = (OGRLineString*)(geometry);
            geo_coll.m_line = utils::silly_line_from_ogr(lineString);
            status = true;
        }
        break;
        case enum_geometry_type::egtPolygon:  // 单面
        {
            auto polygon = (OGRPolygon*)(geometry);
            geo_coll.m_poly = utils::silly_poly_from_ogr(polygon);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiPoint:  // 多点
        {
            auto multiPoint = (OGRMultiPoint*)(geometry);
            geo_coll.m_m_points = utils::silly_multi_point_from_ogr(multiPoint);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiLineString:  // 多线
        {
            auto multiLineString = (OGRMultiLineString*)(geometry);
            geo_coll.m_m_lines = utils::silly_multi_line_from_ogr(multiLineString);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiPolygon:  // 多面
        {
            auto multiPolygon = (OGRMultiPolygon*)(geometry);
            geo_coll.m_m_polys = utils::silly_multi_poly_from_ogr(multiPolygon);
            status = true;
        }
        break;
        default:
        {
            SLOG_ERROR("Unprocessable data types: {}\n", static_cast<int>(feature_type));
        }
        break;
    }
#endif
    return status;
}

bool utils::read_geo_coll(const std::string& u8file, std::vector<silly_geo_coll>& collections, const bool& ignore_prop)
{
    bool status = false;
#if ENABLE_GDAL
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
            if (geometry == nullptr)
            {
                OGRFeature::DestroyFeature(feature);
                continue;
            }
            auto feature_type = (enum_geometry_type)wkbFlatten(geometry->getGeometryType());
            temp_geo_coll.m_type = feature_type;  // 添加矢量数据类型
            if (!ignore_prop)
            {
                read_property(feature, properties, temp_geo_coll.m_props);  // 读取属性数据
            }
            status = read_all_types_data(feature_type, geometry, temp_geo_coll);  // 添加所有数据类型,如果是复合数据类型会递归的调用
            OGRFeature::DestroyFeature(feature);
            collections.push_back(temp_geo_coll);
        }

    }  // 一个图层结束
    GDALClose(dataset);
#endif
    return status;
}

// 根据文件拓展得到对应的存储格式 TODO :
bool utils::get_driver_name(const std::string& u8file, std::string& driver_name)
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
#if ENABLE_GDAL
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

#endif
// 添加属性到shp中
bool writePropertiesToGeometry(OGRFeature* feature, const std::map<std::string, silly_geo_prop>& m_props)
{
#if ENABLE_GDAL
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
#endif
    return false;
}

// 处理复合数据类型的变量
bool process_composite_data(const enum_geometry_type coll_type, OGRGeometry* geometry, OGRGeometryCollection* geomCollection, const silly_geo_coll& geo_coll)
{
#if ENABLE_GDAL
    bool status = true;
    switch (coll_type)
    {
        case enum_geometry_type::egtPoint:
        {
            OGRPoint ogrPoint(geo_coll.m_point.x, geo_coll.m_point.y);
            geomCollection->addGeometry(&ogrPoint);
        }
        break;
        case enum_geometry_type::egtLineString:
        {
            OGRLineString orgLine = utils::silly_line_to_ogr(geo_coll.m_line);
            geomCollection->addGeometry(&orgLine);
        }
        break;
        case enum_geometry_type::egtPolygon:
        {
            OGRPolygon polygon = utils::silly_poly_to_ogr(geo_coll.m_poly);
            geomCollection->addGeometry(&polygon);
        }
        break;
        case enum_geometry_type::egtMultiPoint:
        {
            OGRMultiPoint multiPoint = utils::silly_multi_point_to_ogr(geo_coll.m_m_points);
            geomCollection->addGeometry(&multiPoint);
        }
        break;
        case enum_geometry_type::egtMultiLineString:
        {
            OGRMultiLineString multiLineString = utils::silly_multi_line_to_ogr(geo_coll.m_m_lines);
            geomCollection->addGeometry(&multiLineString);
        }
        break;
        case enum_geometry_type::egtMultiPolygon:
        {
            OGRMultiPolygon multiPolygon = utils::silly_multi_poly_to_ogr(geo_coll.m_m_polys);
            geomCollection->addGeometry(&multiPolygon);
        }
        break;
        default:
            status = false;
            break;
    }

    return status;
#endif
    return false;
}

// 写入所有类型的数据
static bool wire_all_types_data(const enum_geometry_type coll_type, OGRLayer* outputLayer, OGRFeature* feature, OGRGeometry* geometry, const silly_geo_coll& geo_coll)
{
    bool status = true;
#if ENABLE_GDAL
    switch (coll_type)
    {
        case enum_geometry_type::egtPoint:
        {
            OGRPoint ogrPoint(geo_coll.m_point.x, geo_coll.m_point.y);
            feature->SetGeometry(&ogrPoint);
        }
        break;
        case enum_geometry_type::egtLineString:
        {
            OGRLineString orgLine = utils::silly_line_to_ogr(geo_coll.m_line);
            feature->SetGeometry(&orgLine);
        }
        break;
        case enum_geometry_type::egtPolygon:
        {
            OGRPolygon polygon = utils::silly_poly_to_ogr(geo_coll.m_poly);
            feature->SetGeometry(&polygon);
        }
        break;
        case enum_geometry_type::egtMultiPoint:
        {
            OGRMultiPoint multiPoint = utils::silly_multi_point_to_ogr(geo_coll.m_m_points);
            feature->SetGeometry(&multiPoint);
        }
        break;
        case enum_geometry_type::egtMultiLineString:
        {
            OGRMultiLineString multiLineString = utils::silly_multi_line_to_ogr(geo_coll.m_m_lines);
            feature->SetGeometry(&multiLineString);
        }
        break;
        case enum_geometry_type::egtMultiPolygon:
        {
            OGRMultiPolygon multiPolygon = utils::silly_multi_poly_to_ogr(geo_coll.m_m_polys);
            feature->SetGeometry(&multiPolygon);
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
#endif
    return false;
}

bool utils::write_geo_coll(const std::string& u8file, const std::vector<silly_geo_coll>& collections, const proj::CRS::type& prj)
{
    bool status = false;
#if ENABLE_GDAL
    if (collections.empty())
    {
        return status;
    }
    // 根据拓展名得到存储格式
    std::string driver_name;
    if (!get_driver_name(u8file, driver_name))
    {
        SLOG_ERROR("无法确定写入文件类型{}", u8file);
        return status;
    }
    std::string LayerName = std::filesystem::path(u8file).filename().stem().string();
    if (collections.empty())
    {
        SLOG_ERROR("矢量为空");
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
        SLOG_ERROR("创建输出文件失败");
        return false;
    }
    OGRSpatialReference ref = proj::CRS::reference(prj);
    OGRLayer* outputLayer = outputData->CreateLayer(LayerName.c_str(), &ref, wkbUnknown, nullptr);
    if (outputLayer == nullptr)
    {
        // 处理图层创建失败的情况
        SLOG_ERROR("创建图层失败")
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
            SLOG_ERROR("创建属性段:{}失败", k);
            return false;
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
                SLOG_ERROR("写入属性失败")
            }
        }
        // 添加矢量数据
        status = wire_all_types_data(coll_type, outputLayer, feature, geometry, coll);

        OGRFeature::DestroyFeature(feature);
        OGRGeometryFactory::destroyGeometry(geometry);
    }

    // 关闭数据集
    GDALClose(outputData);
    SLOG_DEBUG("写入矢量至{}成功", u8file);
#endif
    return status;
}
bool utils::intersect(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2)
{
    // TODO:
#if ENABLE_GDAL
    // 创建 OGRPolygon 对象
    OGRMultiPolygon p1 = utils::silly_multi_poly_to_ogr(mpoly1);
    OGRMultiPolygon p2 = utils::silly_multi_poly_to_ogr(mpoly2);

    // 判断两个 OGRPolygon 是否相交
    if (p1.Intersects(&p2))
    {
        return true;
    }
#endif
    return false;
}
std::vector<silly_poly> utils::intersection(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2)
{
    std::vector<silly_poly> result;
#if ENABLE_GDAL
    // 创建 OGRPolygon 对象
    OGRMultiPolygon org_ploy_1 = utils::silly_multi_poly_to_ogr(mpoly1);
    OGRMultiPolygon org_ploy_2 = utils::silly_multi_poly_to_ogr(mpoly2);

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
            result.emplace_back(utils::silly_poly_from_ogr(intersectingPolygon));
            break;
        }
        // 多面
        case wkbMultiPolygon:
        case wkbMultiPolygon25D:
        {
            auto intersectingMultiPolygon = (OGRMultiPolygon*)(intersection);
            auto m_polys = utils::silly_multi_poly_from_ogr(intersectingMultiPolygon);
            for (const auto& poly : m_polys)
            {
                result.emplace_back(poly);
            }
            break;
        }
        default:
            break;
    }
#endif

    return result;
}
std::optional<silly_point> utils::intersection(const silly_segment& s1, const silly_segment& s2)
{
    double x1 = s1.p0.x;
    double y1 = s1.p0.y;
    double x2 = s1.p1.x;
    double y2 = s1.p1.y;

    double x3 = s2.p0.x;
    double y3 = s2.p0.y;
    double x4 = s2.p1.x;
    double y4 = s2.p1.y;

    // 计算分母
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    // 平行或重合
    if (std::fabs(denominator) < SU_TINY)
    {
        return std::nullopt;
    }

    // 计算交点的参数 t 和 u
    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denominator;
    double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denominator;

    // 检查交点是否在线段内
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
    {
        double intersectX = x1 + t * (x2 - x1);
        double intersectY = y1 + t * (y2 - y1);
        return silly_point(intersectX, intersectY);
    }

    // 交点不在两条线段上
    return std::nullopt;
}

bool utils::intersect(const silly_poly& mpoly, const silly_point& point)
{
    int intersections = 0;
    silly_point ray_end(point.x + 1000, point.y);  // 向右引一条射线 1000单位

    // 外环
    bool is_in_outer_ring = intersect(point, mpoly.outer_ring.points);
    if (is_in_outer_ring)
    {
        // 内环
        for (const auto& inner : mpoly.inner_rings)
        {
            if (intersect(point, inner.points))  // 在内环内
            {
                return false;  // 如果这个点在一个内环内就属于在面外
            }
        }
        return true;  // 点在外环内,且不在任何一个内环内
    }
    else
    {
        return false;
    }
}

bool utils::intersect(const silly_multi_poly& mpoly, const silly_point& point)
{
    bool is_in = false;
    for (const auto& poly : mpoly)
    {
        if (intersect(poly, point))
        {
            is_in = true;  // 如果点在任何一个多边形内,则认为在面内,即相交
            break;
        }
    }
    return is_in;
}
bool utils::intersect(const silly_multi_poly& mpoly, const silly_line& line)
{
    // TODO:
    return false;
}
bool utils::nearby(const silly_point& point, const silly_line& line, const double& dist)
{
    // TODO:
    return false;
}
std::vector<silly_line> utils::intersection(const silly_multi_poly& mpoly, const silly_line& line)
{
    // TODO:
    return std::vector<silly_line>();
}
double utils::area(const std::vector<silly_point>& points)
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
        result += points[i].x * points[j].y;
        result -= points[j].x * points[i].y;
    }
    return std::abs(result) / 2.0;
}
double utils::area(const silly_poly& poly)
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
double utils::area_sqkm(const silly_poly& poly)
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
double utils::area(const silly_multi_poly& mpoly)
{
    double total_area = 0;
    for (const auto& poly : mpoly)
    {
        total_area += area(poly);
    }
    return total_area;
}
double utils::area_sqkm(const silly_multi_poly& mpoly)
{
    double total_area = 0;
    for (const auto& poly : mpoly)
    {
        total_area += area_sqkm(poly);
    }
    return total_area;
}
std::vector<silly_poly> utils::trans_intersection(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2)
{
    std::vector<silly_poly> result;
    // TODO:
    return result;
}
std::vector<silly_line> utils::trans_intersection(const silly_multi_poly& mpoly1, const silly_line& line)
{
    std::vector<silly_line> result;
    // TODO:
    return result;
}
double utils::area_sqkm(const std::vector<silly_point>& points)
{
    double maxx = -1e10, minx = 1e10;
    for (auto p : points)
    {
        maxx = std::max(maxx, p.x);
        minx = std::min(minx, p.x);
    }
    std::vector<silly_point> gpoints;
    auto central = SU_GAUSS6_L0(SU_GAUSS6_NO((minx + maxx) / 2));
    for (auto p : points)
    {
        silly_point tmp;
        proj::convert::lonlat_to_gauss(central, p.x, p.y, tmp.y, tmp.x);
        gpoints.push_back(tmp);
    }
    return area(gpoints) / 1e6;
}
std::vector<silly_point> utils::smooth_line(const std::vector<silly_point>& line, const int& mod, const int& interp)
{
    return std::vector<silly_point>();
}
std::vector<silly_point> utils::smooth_ring(const std::vector<silly_point>& ring, const int& mod, const int& interp)
{
    return std::vector<silly_point>();
}
std::vector<silly_point> utils::simplify_line(const std::vector<silly_point>& line, const double& dist)
{
    return std::vector<silly_point>();
}
std::vector<silly_point> utils::simplify_ring(const std::vector<silly_point>& ring, const double& dist)
{
    return std::vector<silly_point>();
}

bool utils::intersect(const silly_point& point, const std::vector<silly_point>& points)
{
    int i, j;
    double d;
    int num = points.size();
    bool c = false;
    for (i = 0, j = num - 1; i < num; j = i++)
    {
        d = (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x;
        if (point.x == d)
        {
            return false;
        }

        if ((((points[i].y <= point.y) && (point.y < points[j].y) || ((points[j].y <= point.y) && (point.y < points[i].y))) && (point.x < d)))
        {
            c = !c;
        }
    }
    return c;
}
double utils::distance(const silly_point& p1, const silly_point& p2)
{
    return std::sqrt(distance_sq(p1, p2));
}
double utils::distance_km(const silly_point& p1, const silly_point& p2)
{
    /// https://github.com/atychang/geo-distance/blob/master/vincenty/cpp/CalcDistance.cc
    const double a = 6378137.0;            // WGS-84 Earth semi-major axis (m)
    const double f = 1.0 / 298.257223563;  // WGS-84 flattening factor of the Earth
    const double b = a * (1 - f);          // WGS-84 Earth semi-minor axis (m)

    double lat1 = p1.y * SU_PI / 180;
    double lon1 = p1.x * SU_PI / 180;
    double lat2 = p2.y * SU_PI / 180;
    double lon2 = p2.x * SU_PI / 180;
    double U1 = atan((1 - f) * tan(lat1));
    double U2 = atan((1 - f) * tan(lat2));
    double sinU1 = sin(U1), cosU1 = cos(U1);
    double sinU2 = sin(U2), cosU2 = cos(U2);
    double L = (lon2 - lon1) * SU_PI / 180;

    double sinLambda;
    double cosLambda;
    double sinSigma;
    double cosSigma;
    double sigma;
    double cosSqAlpha;
    double cos2SigmaM;

    double lambda = L, prevLambda;
    int iterationLimit = 100;

    do
    {
        sinLambda = sin(lambda);
        cosLambda = cos(lambda);
        ;
        sinSigma = sqrt(pow(cosU2 * sinLambda, 2) + pow(cosU1 * sinU2 - sinU1 * cosU2 * cosLambda, 2));

        if (sinSigma == 0)  // coincident points
            return 0;

        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan(sinSigma / cosSigma);
        double sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - pow(sinAlpha, 2);

        if (cosSqAlpha == 0)  // equatorial line: cosSqAlpha = 0
            cos2SigmaM = 0;
        else
            cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;

        double C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        prevLambda = lambda;
        lambda = L + (1 - C) * f * sinAlpha * (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * pow(cos2SigmaM, 2))));
    } while (std::abs(lambda - prevLambda) > 1e-12 && --iterationLimit > 0);

    if (iterationLimit == 0)  // formula failed to converge
        return 0;

    double uSq = cosSqAlpha * (pow(a, 2) - pow(b, 2)) / pow(b, 2);
    double A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    double B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    double deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 * (cosSigma * (-1 + 2 * pow(cos2SigmaM, 2)) - B / 6 * cos2SigmaM * (-3 + 4 * pow(sinSigma, 2)) * (-3 + 4 * pow(cos2SigmaM, 2))));

    double s = b * A * (sigma - deltaSigma);  // in the same units as a and b

    // bearing (direction) in radius
    // degree = radius * 180 / pi
    double revAz = atan2(cosU1 * sinLambda, -sinU1 * cosU2 + cosU1 * sinU2 * cosLambda);

    return s / 1000.0;
}
double utils::distance_sq(const silly_point& p1, const silly_point& p2)
{
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

silly_geo_coll utils::buffer(const silly_geo_coll& coll, const double& distance)
{
    silly_geo_coll ret;
#if ENABLE_GDAL
    OGRGeometry* resOGRGeom = silly_geo_coll_to_ogr(coll);
    if (resOGRGeom == nullptr)
    {
        SLOG_ERROR("Failed to convert silly_geo_coll to OGRGeometry");
        return ret;
    }
    OGRGeometry* bufferedGeom = resOGRGeom->Buffer(distance);  // 创建缓冲区
    if (bufferedGeom == nullptr)
    {
        OGRGeometryFactory::destroyGeometry(resOGRGeom);
        resOGRGeom = nullptr;
        SLOG_ERROR("Failed to buffer OGRGeometry");
        return ret;
    }
    ret = silly_geo_coll_from_ogr(bufferedGeom);
    if (resOGRGeom != nullptr)
    {
        OGRGeometryFactory::destroyGeometry(resOGRGeom);
        resOGRGeom = nullptr;
    }
    if (bufferedGeom != nullptr)
    {
        OGRGeometryFactory::destroyGeometry(bufferedGeom);
        bufferedGeom = nullptr;
    }
#endif
    return ret;
}
std::vector<std::pair<silly_point, double>> utils::adjust(const std::vector<std::pair<silly_point, double>>& linez, const double& bz, const double& ez)
{
    std::vector<std::pair<silly_point, double>> ret;
    double dzB = (bz - linez.front().second);
    double dzE = (ez - linez.back().second);
    double totalDist = 0;
    for (int i = 1; i < linez.size(); i++)
    {
        totalDist += distance(linez[i].first, linez[i - 1].first);
    }
    double dist = 0;
    ret.push_back(std::make_pair(linez.front().first, bz));
    for (int i = 1; i < linez.size(); i++)
    {
        dist += distance(linez[i].first, linez[i - 1].first);
        double percent = dist / totalDist;
        double z = linez[i].second - ((1 - percent) * dzB + percent * dzE);
        ret.push_back(std::make_pair(linez[i].first, z));
    }

    return ret;
}
