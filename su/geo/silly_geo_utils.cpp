

#include "silly_geo_utils.h"
#ifndef NDEBUG
#include <log/silly_log.h>
#endif
#if SU_THIRD_SUPPORT_GDAL
// GDAL.
#include <ogr_spatialref.h>
#include <ogrsf_frmts.h>
#include <gdal_alg.h>
#include <ogr_api.h>
#endif

#include <polyclipping/clipper.hpp>
#include <encode/silly_encode.h>
#include <files/silly_file.h>
#include <geo/proj/silly_proj.h>
using namespace silly::geo;
using namespace ClipperLib;

silly_point utils::centroid(const silly_poly& poly)
{
    silly_point center_point;
#if SU_THIRD_SUPPORT_GDAL
    OGRPolygon orgPloy = silly_gdal::silly_poly_to_ogr(poly);
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


void utils::init_gdal_env()
{
#if SU_THIRD_SUPPORT_GDAL
    GDALAllRegister();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
    OGRRegisterAll();
    CPLSetConfigOption("SHAPE_ENCODING", "");
#endif
}

void utils::destroy_gdal_env()
{
#if SU_THIRD_SUPPORT_GDAL
    OGRCleanupAll();
#endif
}

bool utils::is_valid_shp(const std::filesystem::path& file)
{
#if SU_THIRD_SUPPORT_GDAL
    auto poDSr = (GDALDataset*)GDALOpenEx(sufile::realpath(file).string().c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr);
    if (nullptr == poDSr)
    {
        return false;
    }
    GDALClose(poDSr);
    return true;
#endif
    return false;
}

std::vector<std::string> utils::shp_missing_file(const std::filesystem::path& file)
{
    std::vector<std::string> ret;
    // 获取文件的父级目录和文件名不包含后缀名
    std::filesystem::path sfp_shp = sufile::realpath(file);
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

bool utils::check_shp_info(const std::filesystem::path& file, eGeometryType& geoType, std::map<std::string, eGeoFieldType>& properties)
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    std::map<std::string, std::string> result;

    auto poDSr = (GDALDataset*)GDALOpenEx(sufile::realpath(file).string().c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr);
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
    geoType = static_cast<eGeometryType>(gdal_type);
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
        OGRFieldType fieldType = def->GetType();
        std::string field_name = def->GetNameRef();
        if (!silly_encode::check_text_utf8(field_name.c_str(), field_name.size()))
        {
            field_name = silly_encode::gbk_utf8(field_name);
        }
        eGeoFieldType field_type{eGeoFieldType::None};
        switch (fieldType)
        {
            case OFTInteger:
                field_type = eGeoFieldType::Int;
                break;
            case OFTIntegerList:
                break;
            case OFTReal:
                field_type = eGeoFieldType::Numeric;
                break;
            case OFTRealList:
                break;
            case OFTString:
                field_type = eGeoFieldType::String;
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
                field_type = eGeoFieldType::Binary;
                break;
            case OFTTime:
                field_type = eGeoFieldType::Time;
                break;
            case OFTDate:
                field_type = eGeoFieldType::Date;
                break;
            case OFTDateTime:
                field_type = eGeoFieldType::DateTime;
                break;
            case OFTInteger64:
                field_type = eGeoFieldType::Long;
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
#if SU_THIRD_SUPPORT_GDAL
/// <summary>
/// 读取一个矢量数据的属性
/// </summary>
/// <param name="feature"></param>
/// <param name="properties"></param>
/// <param name="props"></param>
/// <returns></returns>
bool read_property(const OGRFeature* feature, const std::map<std::string, eGeoFieldType>& properties, std::map<std::string, silly_geo_prop>& props)
{

    for (const auto& [key, p_type] : properties)
    {
        std::string utf8_key = key;
        if (!silly_encode::is_utf8(utf8_key))
        {
            utf8_key = silly_encode::gbk_utf8(utf8_key);
        }
        switch (p_type)
        {
            case eGeoFieldType::None:
                break;
            case eGeoFieldType::Int:
            {
                int value = feature->GetFieldAsInteger(key.c_str());
                props[utf8_key] = {value};
            }
            break;
            case eGeoFieldType::Numeric:
            {
                double value = feature->GetFieldAsDouble(key.c_str());
                props[utf8_key] = {value};
            }
            break;
            case eGeoFieldType::String:
            {
                std::string value = feature->GetFieldAsString(key.c_str());
                if (!silly_encode::is_utf8(value))
                {
                    value = silly_encode::gbk_utf8(value);
                }
                props[utf8_key] = {value};
            }
            break;
            case eGeoFieldType::Time:
            case eGeoFieldType::Date:
            case eGeoFieldType::DateTime:
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
            case eGeoFieldType::Long:
            {
                long long value = feature->GetFieldAsInteger64(key.c_str());
                props[utf8_key] = {value};
            }
            break;
            default:
                break;
        }
    }

    return true;
}

bool read_all_types_data(const eGeometryType& feature_type, const OGRGeometry* geometry, silly_geo_coll& geo_coll)
{
    bool status = false;
    switch (feature_type)
    {
        case eGeometryType::Point:  // 单点
        {
            auto point = (OGRPoint*)(geometry);
            geo_coll.m_point = silly_gdal::silly_point_from_ogr(point);
            status = true;
        }
        break;
        case eGeometryType::LineString:  // 单线
        {
            auto lineString = (OGRLineString*)(geometry);
            geo_coll.m_line = silly_gdal::silly_line_from_ogr(lineString);
            status = true;
        }
        break;
        case eGeometryType::Polygon:  // 单面
        {
            auto polygon = (OGRPolygon*)(geometry);
            geo_coll.m_poly = silly_gdal::silly_poly_from_ogr(polygon);
            status = true;
        }
        break;
        case eGeometryType::MultiPoint:  // 多点
        {
            auto multiPoint = (OGRMultiPoint*)(geometry);
            geo_coll.m_m_points = silly_gdal::silly_multi_point_from_ogr(multiPoint);
            status = true;
        }
        break;
        case eGeometryType::MultiLineString:  // 多线
        {
            auto multiLineString = (OGRMultiLineString*)(geometry);
            geo_coll.m_m_lines = silly_gdal::silly_multi_line_from_ogr(multiLineString);
            status = true;
        }
        break;
        case eGeometryType::MultiPolygon:  // 多面
        {
            auto multiPolygon = (OGRMultiPolygon*)(geometry);
            geo_coll.m_m_polys = silly_gdal::silly_multi_poly_from_ogr(multiPolygon);
            status = true;
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
#endif
std::vector<silly_geo_coll> utils::read(const std::filesystem::path& file, const bool& ignore_prop)
{
    std::vector<silly_geo_coll> ret;
    utils::read(file, ret, ignore_prop);
    return ret;
}

bool utils::read(const std::filesystem::path& file, std::vector<silly_geo_coll>& collections, const bool& ignore_prop)
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    std::filesystem::path realPath = sufile::realpath(file);
    eGeometryType type;
    std::map<std::string, eGeoFieldType> properties;
    if (!check_shp_info(realPath.string(), type, properties))
    {
        SLOG_ERROR("检查矢量[{}]信息失败\n", realPath.u8string());
        return status;
    }
    // 打开现有 shp 文件
    auto dataset = static_cast<GDALDataset*>(GDALOpenEx(realPath.string().c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr));
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
            auto feature_type = (eGeometryType)wkbFlatten(geometry->getGeometryType());
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

std::string utils::gdal_driver_name(const std::filesystem::path& file)
{
    static const std::unordered_map<std::string, std::string>
    DRIVER_NAMES = {
        {".shp", "ESRI Shapefile"},
        {".tab", "Mapinfo File"},
        {".geojson","GeoJSON"},
        {".sqlite","SQLite"},
        {".csv", "CSV"},
        {".kml","KML"},
        {".gml","GML"},
        {".xlsx","XLSX"}
    };
    std::string ext = LowerStr(file.extension().string());
    for (const auto& [extension, driver] : DRIVER_NAMES)
    {
        if (std::strcmp(ext.c_str(), extension.c_str()) == 0)
        {
            return driver;
        }
    }
    return "";
}
#if SU_THIRD_SUPPORT_GDAL
// 根据eGeoFieldType 找gdal中属性的类型
OGRFieldType convertToOGRFieldType(const eGeoFieldType& type)
{
    OGRFieldType result = OFTString;
    switch (type)
    {
        case eGeoFieldType::None:
            result = OFTString;
            break;
        case eGeoFieldType::Int:
            result = OFTInteger;
            break;
        case eGeoFieldType::Numeric:
            result = OFTReal;
            break;
        case eGeoFieldType::String:
            result = OFTString;
            break;
        case eGeoFieldType::Binary:
            result = OFTBinary;
            break;
        case eGeoFieldType::Time:
            result = OFTTime;
            break;
        case eGeoFieldType::Date:
            result = OFTDate;
            break;
        case eGeoFieldType::DateTime:
            result = OFTDateTime;
            break;
        case eGeoFieldType::Long:
            result = OFTInteger64;
            break;
        default:
            result = OFTString;
            break;
    }
    return result;
}

#endif
#if SU_THIRD_SUPPORT_GDAL
// 添加属性到shp中
bool writePropertiesToGeometry(OGRFeature* feature, const std::map<std::string, silly_geo_prop>& m_props)
{

    bool status = true;
    for (const auto& [key, prop] : m_props)
    {
        int fieldIndex = feature->GetFieldIndex(key.c_str());
        if (fieldIndex >= 0)
        {
            switch (prop.type())
            {
                case eGeoFieldType::Int:
                    feature->SetField(fieldIndex, prop.as_int32());
                    break;
                case eGeoFieldType::Numeric:
                    feature->SetField(fieldIndex, prop.as_double());
                    break;
                case eGeoFieldType::String:
                    feature->SetField(fieldIndex, prop.as_string().c_str());
                    break;
                case eGeoFieldType::Time:
                    break;
                case eGeoFieldType::Date:
                    break;
                case eGeoFieldType::DateTime:
                    break;
                case eGeoFieldType::Long:
                    feature->SetField(fieldIndex, static_cast<long long>(prop.as_int64()));
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
bool process_composite_data(const eGeometryType coll_type, OGRGeometry* geometry, OGRGeometryCollection* geomCollection, const silly_geo_coll& geo_coll)
{

    bool status = true;
    switch (coll_type)
    {
        case eGeometryType::Point:
        {
            OGRPoint ogrPoint(geo_coll.m_point.x, geo_coll.m_point.y);
            geomCollection->addGeometry(&ogrPoint);
        }
        break;
        case eGeometryType::LineString:
        {
            OGRLineString orgLine = silly_gdal::silly_line_to_ogr(geo_coll.m_line);
            geomCollection->addGeometry(&orgLine);
        }
        break;
        case eGeometryType::Polygon:
        {
            OGRPolygon polygon = silly_gdal::silly_poly_to_ogr(geo_coll.m_poly);
            geomCollection->addGeometry(&polygon);
        }
        break;
        case eGeometryType::MultiPoint:
        {
            OGRMultiPoint multiPoint = silly_gdal::silly_multi_point_to_ogr(geo_coll.m_m_points);
            geomCollection->addGeometry(&multiPoint);
        }
        break;
        case eGeometryType::MultiLineString:
        {
            OGRMultiLineString multiLineString = silly_gdal::silly_multi_line_to_ogr(geo_coll.m_m_lines);
            geomCollection->addGeometry(&multiLineString);
        }
        break;
        case eGeometryType::MultiPolygon:
        {
            OGRMultiPolygon multiPolygon = silly_gdal::silly_multi_poly_to_ogr(geo_coll.m_m_polys);
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
static bool wire_all_types_data(const eGeometryType coll_type, OGRLayer* outputLayer, OGRFeature* feature, OGRGeometry* geometry, const silly_geo_coll& geo_coll)
{
    bool status = true;
    switch (coll_type)
    {
        case eGeometryType::Point:
        {
            OGRPoint ogrPoint(geo_coll.m_point.x, geo_coll.m_point.y);
            feature->SetGeometry(&ogrPoint);
        }
        break;
        case eGeometryType::LineString:
        {
            OGRLineString orgLine = silly_gdal::silly_line_to_ogr(geo_coll.m_line);
            feature->SetGeometry(&orgLine);
        }
        break;
        case eGeometryType::Polygon:
        {
            OGRPolygon polygon = silly_gdal::silly_poly_to_ogr(geo_coll.m_poly);
            feature->SetGeometry(&polygon);
        }
        break;
        case eGeometryType::MultiPoint:
        {
            OGRMultiPoint multiPoint = silly_gdal::silly_multi_point_to_ogr(geo_coll.m_m_points);
            feature->SetGeometry(&multiPoint);
        }
        break;
        case eGeometryType::MultiLineString:
        {
            OGRMultiLineString multiLineString = silly_gdal::silly_multi_line_to_ogr(geo_coll.m_m_lines);
            feature->SetGeometry(&multiLineString);
        }
        break;
        case eGeometryType::MultiPolygon:
        {
            OGRMultiPolygon multiPolygon = silly_gdal::silly_multi_poly_to_ogr(geo_coll.m_m_polys);
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
}
#endif
bool utils::write(const std::filesystem::path& file, const std::vector<silly_geo_coll>& collection, const eCrsEpsgCode& prj, const std::string& encode)
{
    bool status = false;
#if SU_THIRD_SUPPORT_GDAL
    if (collection.empty())
    {
        SLOG_ERROR("矢量为空");
        return status;
    }
    std::filesystem::path realPath = sufile::realpath(file);
    // 根据拓展名得到存储格式
    std::string gdalDriverName = gdal_driver_name(realPath);
    if (gdalDriverName.empty())
    {
        SLOG_ERROR("无法确定写入文件类型{}", realPath.u8string());
        return status;
    }
    std::string LayerName = realPath.filename().stem().string();

    GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName(gdalDriverName.c_str());
    GDALDataset* outputData = outDriver->Create(realPath.string().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
    if (outputData == nullptr)
    {
        SLOG_ERROR("创建输出文件失败");
        return false;
    }
    OGRSpatialReference ref = ORG_SP_REF(prj);
    OGRLayer* outputLayer = outputData->CreateLayer(LayerName.c_str(), &ref, wkbUnknown, nullptr);
    if (outputLayer == nullptr)
    {
        // 处理图层创建失败的情况
        SLOG_ERROR("创建图层失败")
        GDALClose(outputData);
        return false;
    }
    // TODO :

    for (const auto& [k, p] : collection.front().m_props)  // 添加属性
    {
        OGRFieldType ogrType = convertToOGRFieldType(p.type());
        OGRFieldDefn fieldDef(k.c_str(), ogrType);
        if (outputLayer->CreateField(&fieldDef) != OGRERR_NONE)
        {
            SLOG_ERROR("创建属性段:{}失败", k);
            return false;
        }
    }
    for (const auto& coll : collection)
    {
        OGRFeature* feature = OGRFeature::CreateFeature(outputLayer->GetLayerDefn());
        // 添加矢量
        eGeometryType coll_type = coll.m_type;
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
    //手动创建.cpg文件
    std::filesystem::path cpgFile = realPath.parent_path();
    cpgFile.append(realPath.stem().string().append(".cpg"));
    sufile::write(cpgFile, encode);
    // 关闭数据集
    GDALClose(outputData);
    SLOG_DEBUG("写入矢量至{}成功", realPath.u8string());
#endif
    return status;
}

bool utils::intersect(const silly_geo_coll& gc1, const silly_geo_coll& gc2)
{
    return false;
}

bool utils::intersect(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2)
{
    // TODO:
#if SU_THIRD_SUPPORT_GDAL
    // 创建 OGRPolygon 对象
    OGRMultiPolygon p1 = silly_gdal::silly_multi_poly_to_ogr(mpoly1);
    OGRMultiPolygon p2 = silly_gdal::silly_multi_poly_to_ogr(mpoly2);

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
#if SU_THIRD_SUPPORT_GDAL
    // 创建 OGRPolygon 对象
    OGRMultiPolygon org_ploy_1 = silly_gdal::silly_multi_poly_to_ogr(mpoly1);
    OGRMultiPolygon org_ploy_2 = silly_gdal::silly_multi_poly_to_ogr(mpoly2);

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
            result.emplace_back(silly_gdal::silly_poly_from_ogr(intersectingPolygon));
            break;
        }
        // 多面
        case wkbMultiPolygon:
        case wkbMultiPolygon25D:
        {
            auto intersectingMultiPolygon = (OGRMultiPolygon*)(intersection);
            auto m_polys = silly_gdal::silly_multi_poly_from_ogr(intersectingMultiPolygon);
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

std::optional<silly_pointZ> utils::intersection(const silly_segmentZ& s1, const silly_segmentZ& s2)
{
    double x1 = s1.p0.x;
    double y1 = s1.p0.y;
    double z1 = s1.p0.z;

    double x2 = s1.p1.x;
    double y2 = s1.p1.y;
    double z2 = s1.p1.z;

    double x3 = s2.p0.x;
    double y3 = s2.p0.y;
    double z3 = s2.p0.z;

    double x4 = s2.p1.x;
    double y4 = s2.p1.y;
    double z4 = s2.p1.z;

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
        double intersectZ = ((z1 + t * (z2 - z1)) + (z3 + t * (z4 - z3))) / 2.0;
        return silly_pointZ(intersectX, intersectY, intersectZ);
    }

    // 交点不在两条线段上
    return std::nullopt;
}

bool utils::intersect(const silly_poly& mpoly, const silly_point& point)
{
    int intersections = 0;
    silly_point ray_end(point.x + 1000, point.y);  // 向右引一条射线 1000单位

    // 外环
    bool is_in_outer = intersect(point, mpoly.outer.points);
    if (is_in_outer)
    {
        // 内环
        for (const auto& inner : mpoly.holes)
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
    double total_area = area(poly.outer.points);
    if (total_area < 1.E-15)
    {
        return total_area;
    }

    for (const auto& inner_ring : poly.holes)
    {
        total_area -= area(inner_ring.points);
    }

    return total_area;
}
double utils::area_sqkm(const silly_poly& poly, const double& l0)
{
    double total_area = area_sqkm(poly.outer.points, l0);
    if (total_area < 1.E-15)
    {
        return total_area;
    }
    for (const auto& inner_ring : poly.holes)
    {
        total_area -= area_sqkm(inner_ring.points, l0);
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
double utils::area_sqkm(const silly_multi_poly& mpoly, const double& l0)
{
    double total_area = 0;
    for (const auto& poly : mpoly)
    {
        total_area += area_sqkm(poly, l0);
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
double utils::area_sqkm(const std::vector<silly_point>& points, const double& l0)
{
    double maxx = -1e10, minx = 1e10;
    for (auto p : points)
    {
        maxx = std::max(maxx, p.x);
        minx = std::min(minx, p.x);
    }
    std::vector<silly_point> gpoints;
    for (auto p : points)
    {
        silly_point tmp;
        proj::convert::lonlat_to_gauss(l0, p.x, p.y, tmp.y, tmp.x);
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
    bool is_inside = false;
    const size_t count = points.size();
    if (count < 2)
        return false;  // 至少需要两个点构成线段

    for (size_t i = 0, j = count - 1; i < count; j = i++)
    {
        const auto& v1 = points[j];
        const auto& v2 = points[i];

        // 检查点是否在线段上（避免浮点精度问题）
        const double dx = v2.x - v1.x;
        const double dy = v2.y - v1.y;
        if (std::abs(dy) > std::numeric_limits<double>::epsilon())
        {
            const double intersection_x = (dx * (point.y - v1.y) / dy) + v1.x;
            if (std::abs(point.x - intersection_x) < std::numeric_limits<double>::epsilon())
            {
                return false;  // 点在边界上
            }
        }

        // 射线法判断内外
        if (((v1.y <= point.y && point.y < v2.y) || (v2.y <= point.y && point.y < v1.y)) && (point.x < (dx * (point.y - v1.y) / dy + v1.x)))
        {
            is_inside = !is_inside;
        }
    }
    return is_inside;
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
#if SU_THIRD_SUPPORT_GDAL
    OGRGeometry* resOGRGeom = silly_gdal::silly_geo_coll_to_ogr(coll);
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
    ret = silly_gdal::silly_geo_coll_from_ogr(bufferedGeom);
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
