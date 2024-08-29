

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
    result.setPoints(ring.points.size(), &xs[0], &ys[0]);
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
    silly_multi_point mulitPoint;
    int pointCount = ogrMultiPoint->getNumGeometries();
    for (int i = 0; i < pointCount; i++)
    {
        silly_point sillyPoint = silly_point_from_ogr(ogrMultiPoint->getGeometryRef(i));
        mulitPoint.push_back(sillyPoint);
    }
    return mulitPoint;
}

// 将 silly_multi_point(多点) 转换为 OGRMultiPoint(多点) 类型
OGRMultiPoint geo_utils::silly_multi_point_to_ogr(const silly_multi_point& mulitPoint)
{
    OGRMultiPoint orgMultiPoint;
    for (const silly_point& point : mulitPoint)
    {
        OGRPoint ogrPoint = silly_point_to_ogr(point);
        orgMultiPoint.addGeometryDirectly(&ogrPoint);
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
    ogrLineString.setPoints(line.size(), &xs[0], &ys[0]);
    return ogrLineString;
}

// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
silly_multi_silly_line geo_utils::silly_multi_line_from_ogr(const OGRMultiLineString* multiLineString)
{
    silly_multi_silly_line multiLine;
    int numLines = multiLineString->getNumGeometries();
    for (int i = 0; i < numLines; i++)
    {
        OGRLineString* lineString = (OGRLineString*)(multiLineString->getGeometryRef(i));
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
        ogrMultiLineString.addGeometryDirectly(&ogrLineString);
    }

    return ogrMultiLineString;
}

// OGRPolygon对象转换为silly_poly(多环:外环+内环)对象  (单面)
silly_poly geo_utils::silly_poly_from_ogr(const OGRPolygon* polygon)
{
    silly_poly poly;
    // 处理OGRPolygon外环
    OGRLinearRing* outerRing = (OGRLinearRing*)polygon->getExteriorRing();
    poly.outer_ring = silly_ring_from_ogr(outerRing);
    // 处理OGRPolygon内环
    int innerRingCount = polygon->getNumInteriorRings();
    for (int k = 0; k < innerRingCount; k++)
    {
        OGRLinearRing* ring = (OGRLinearRing*)polygon->getInteriorRing(k);
        silly_ring innerRing = silly_ring_from_ogr(ring);
        poly.inner_rings.push_back(innerRing);
    }
    return poly;
}

// 将 silly_poly 转换为 OGRPolygon(单面)
OGRPolygon geo_utils::silly_poly_to_ogr(const silly_poly& poly)
{
    OGRPolygon ogrPolygon;
    // 设置外环
    OGRLinearRing outerRing = silly_ring_to_ogr(poly.outer_ring);

    ogrPolygon.addRingDirectly(&outerRing);
    // 设置内环
    for (const silly_ring& innerRing : poly.inner_rings)
    {
        OGRLinearRing innerOGRRing = silly_ring_to_ogr(innerRing);
        ogrPolygon.addRingDirectly(&innerOGRRing);
    }
    ogrPolygon.closeRings();
    return ogrPolygon;
}

// 多面的OGRMultiPolygon对象转换为silly_multi_poly(多面)
silly_multi_poly geo_utils::silly_multi_poly_from_ogr(const OGRMultiPolygon* multiPolygon)
{
    silly_multi_poly multi_poly;
    int polygonCount = multiPolygon->getNumGeometries();
    for (int i = 0; i < polygonCount; i++)
    {
        silly_poly tmp_poly;
        OGRPolygon* polygon = (OGRPolygon*)multiPolygon->getGeometryRef(i);
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

bool geo_utils::check_shp_info(const std::string& shp_file, enum_geometry_type& type, std::map<std::string, silly_geo_prop::enum_prop_type>& properties)
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
        OGRFieldType type = def->GetType();
        std::string field_name = def->GetNameRef();
        if (!silly_encode::check_text_utf8(field_name.c_str(), field_name.size()))
        {
            field_name = silly_encode::gbk_utf8(field_name);
        }
        silly_geo_prop::enum_prop_type field_type{silly_geo_prop::enum_prop_type::eptNone};
        switch (type)
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
#if _DEBUG
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
            auto geo_type = geometry->getGeometryType();
            OGRPoint* point = (OGRPoint*)(geometry);
            geo_coll.m_point = geo_utils::silly_point_from_ogr(point);
            status = true;
        }
        break;
        case enum_geometry_type::egtLineString:  // 单线
        {
            OGRLineString* lineString = (OGRLineString*)(geometry);
            geo_coll.m_line = geo_utils::silly_line_from_ogr(lineString);
            status = true;
        }
        break;
        case enum_geometry_type::egtPolygon:  // 单面
        {
            OGRPolygon* polygon = (OGRPolygon*)(geometry);
            geo_coll.m_poly = geo_utils::silly_poly_from_ogr(polygon);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiPoint:  // 多点
        {
            OGRMultiPoint* multiPoint = (OGRMultiPoint*)(geometry);
            geo_coll.m_m_points = geo_utils::silly_multi_point_from_ogr(multiPoint);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiLineString:  // 多线
        {
            OGRMultiLineString* multiLineString = (OGRMultiLineString*)(geometry);
            geo_coll.m_m_lines = geo_utils::silly_multi_line_from_ogr(multiLineString);
            status = true;
        }
        break;
        case enum_geometry_type::egtMultiPolygon:  // 多面
        {
            OGRMultiPolygon* multiPolygon = (OGRMultiPolygon*)(geometry);
            geo_coll.m_m_polys = geo_utils::silly_multi_poly_from_ogr(multiPolygon);
            status = true;
        }
        break;
        case enum_geometry_type::egtCompositeType:  // 复合数据类型
        {
            OGRGeometryCollection* geomCollection = (OGRGeometryCollection*)(geometry);
            int numGeometries = geomCollection->getNumGeometries();
            SU_DEBUG_PRINT("Number of Geometries in Collection: %d\n", numGeometries);
            for (int j = 0; j < numGeometries; j++)
            {
                OGRGeometry* collGeometry = geomCollection->getGeometryRef(j);
                if (collGeometry != nullptr)
                {
                    enum_geometry_type feature_type_ = (enum_geometry_type)wkbFlatten(collGeometry->getGeometryType());
                    status = read_all_types_data(feature_type_, collGeometry, geo_coll);
                }
            }
        }
        break;
        default:
        {
            SU_ERROR_PRINT("Unprocessable data types: %d\n", feature_type);
        }
        break;
    }

    return status;
}

bool geo_utils::read_geo_coll(const std::string& file, std::vector<silly_geo_coll>& collections, const bool& ignore_prop)
{
    bool status = false;

#if IS_WIN32
    std::string nfpath = silly_encode::utf8_gbk(file);
#else
    std::string nfpath = file;
#endif
    if (!std::filesystem::exists(std::filesystem::path(nfpath)))
    {
        SFP_ERROR("文件[{}]不存在\n", file);
        return status;
    }
    enum_geometry_type type;
    std::map<std::string, silly_geo_prop::enum_prop_type> properties;
    if (!check_shp_info(nfpath, type, properties))
    {
        SFP_ERROR("检查矢量[{}]信息失败\n", file);
        return status;
    }
    // 打开现有 shp 文件
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(nfpath.c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, NULL, NULL, NULL));
    if (dataset == nullptr)
    {
        // 处理文件打开失败的情况
        SU_ERROR_PRINT("Error: Failed to open shapefile\n");
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
            SU_ERROR_PRINT("Error: Failed to get layer\n");
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
            enum_geometry_type feature_type = (enum_geometry_type)wkbFlatten(geometry->getGeometryType());
            temp_geo_coll.m_type = feature_type;                                  // 添加矢量数据类型
            if(ignore_prop)
            {
                read_property(feature, properties, temp_geo_coll.m_props);            // 读取属性数据
            }
            status = read_all_types_data(feature_type, geometry, temp_geo_coll);  // 添加所有数据类型,如果是复合数据类型会递归的调用
            OGRFeature::DestroyFeature(feature);
            collections.push_back(temp_geo_coll);
        }

    }  // 一个图层结束

    return status;
}

// 根据文件拓展得到对应的存储格式 TODO :
bool geo_utils::get_driver_name(const std::string& file, std::string& driverName)
{
    bool status = true;
    // if (!std::filesystem::exists(std::filesystem::path(file)))
    //{
    //     SU_ERROR_PRINT("Error: file does not exist %s ", file);
    //     return status;
    // }
    //  获取文件扩展名
    std::string lowerExtension = std::filesystem::path(file.c_str()).extension().string();
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
        driverName = "";
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
        case silly_geo_prop::enum_prop_type::eptInt:
            result = OFTInteger;
        case silly_geo_prop::enum_prop_type::eptNumeric:
            result = OFTReal;
        case silly_geo_prop::enum_prop_type::eptString:
            result = OFTString;
        case silly_geo_prop::enum_prop_type::eptBinary:
            result = OFTBinary;
        case silly_geo_prop::enum_prop_type::eptTime:
            result = OFTTime;
        case silly_geo_prop::enum_prop_type::eptDate:
            result = OFTDate;
        case silly_geo_prop::enum_prop_type::eptDateTime:
            result = OFTDateTime;
        case silly_geo_prop::enum_prop_type::eptLong:
            result = OFTInteger64;
        default:
            result = OFTString;
    }
    return result;
}

// 添加属性到shp中
bool writePropertiesToGeometry(OGRFeature* feature, const std::map<std::string, silly_geo_prop>& m_props)
{
    bool status = true;
    for (auto [key, prop] : m_props)
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

bool geo_utils::write_geo_coll(const std::string& file, const std::vector<silly_geo_coll>& collections)
{
    bool status = false;
    if (collections.empty())
    {
        return status;
    }
    // 根据拓展名得到存储格式
    std::string driverName;
    if (!get_driver_name(file.c_str(), driverName))
    {
        SFP_ERROR("Error: Unable to obtain storage method for this type: %s\n", file);
        return status;
    }
    std::string LayerName = std::filesystem::path(file).filename().stem().string();
    if (collections.empty())
    {
        SU_ERROR_PRINT("Error: Vector data is empty\n");
        return status;
    }
    GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName(driverName.c_str());
    GDALDataset* outputData = outDriver->Create(file.c_str(), 0, 0, 0, GDT_Unknown, nullptr);
    if (outputData == nullptr)
    {
        SU_ERROR_PRINT("Error: Failed to create output shapefile\n");
        return false;
    }
    OGRLayer* outputLayer = outputData->CreateLayer(LayerName.c_str(), nullptr, wkbUnknown, nullptr);
    if (outputLayer == nullptr)
    {
        // 处理图层创建失败的情况
        SU_ERROR_PRINT("Failed to create output layer\n")
        GDALClose(outputData);
        return false;
    }
    // TODO :

    for (auto [k, p] : collections.front().m_props)  // 添加属性
    {
        OGRFieldType ogrType = convertToOGRFieldType(p.value_type());
        OGRFieldDefn fieldDef(k.c_str(), ogrType);
        if (outputLayer->CreateField(&fieldDef) != OGRERR_NONE)
        {
            SU_ERROR_PRINT("Error: Failed to create color field\n");
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
                SU_ERROR_PRINT("Add attribute fail\n")
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
            OGRPolygon* intersectingPolygon = (OGRPolygon*)(intersection);
            result.emplace_back(geo_utils::silly_poly_from_ogr(intersectingPolygon));
            break;
        }
        // 多面
        case wkbMultiPolygon:
        case wkbMultiPolygon25D:
        {
            OGRMultiPolygon* intersectingMultiPolygon = (OGRMultiPolygon*)(intersection);
            auto m_polys = geo_utils::silly_multi_poly_from_ogr(intersectingMultiPolygon);
            for (auto poly : m_polys)
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
bool silly_geo_utils::intersect(const silly_multi_poly& mpoly, const silly_point& point)
{
    return false;
}
bool silly_geo_utils::intersect(const silly_multi_poly& mpoly, const silly_line& line)
{
    return false;
}
bool silly_geo_utils::nearby(const silly_point& point, const silly_line& line, const double& dist)
{
    return false;
}
std::vector<silly_line> silly_geo_utils::intersection(const silly_multi_poly& mpoly, const silly_line& line)
{
    return std::vector<silly_line>();
}
double silly_geo_utils::area(const std::vector<silly_point>& points)
{
    size_t n = points.size();
    // 确保至少有3个点才能构成一个多边形
    if (n < 3) {
        return 0.0;
    }

    double area = 0.0;
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        area += points[i].lgtd * points[j].lttd;
        area -= points[j].lgtd * points[i].lttd;
    }
    return std::abs(area) / 2.0;
}
double silly_geo_utils::area_degree(const silly_poly& poly)
{
    double total_area = area(poly.outer_ring.points);
    if(total_area < 1.E-15)
    {
        return total_area;
    }

    for(auto inner_ring : poly.inner_rings)
    {
        total_area -= area(inner_ring.points);
    }

    return total_area;
}
double silly_geo_utils::area_sqkm(const silly_poly& poly)
{
    return 0;
}
double silly_geo_utils::area_degree(const silly_multi_poly& mpoly)
{
    return 0;
}
double silly_geo_utils::area_sqkm(const silly_multi_poly& mpoly)
{
    return 0;
}
