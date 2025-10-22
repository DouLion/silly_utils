//
// Created by dell on 2025/9/23.
//

#include "silly_gdal.h"
#include <system/silly_system.h>
#include <encode/silly_encode.h>
#if SU_THIRD_SUPPORT_GDAL

const static std::map<OGRFieldType, eGeoFieldType> GEO_FIELD_TYPE_OGR2SU = {
    {OFTInteger, eGeoFieldType::Int},
    {OFTReal, eGeoFieldType::Numeric},
    {OFTString, eGeoFieldType::String},
    {OFTBinary, eGeoFieldType::Binary},
    {OFTTime, eGeoFieldType::Time},
    {OFTDate, eGeoFieldType::Date},
    {OFTDateTime, eGeoFieldType::DateTime},
    {OFTInteger64, eGeoFieldType::Long},
};

const static std::map<eGeoFieldType, OGRFieldType> GEO_FIELD_TYPE_SU2OGR = {
    {eGeoFieldType::Int, OFTInteger},
    {eGeoFieldType::Numeric, OFTReal},
    {eGeoFieldType::String, OFTString},
    {eGeoFieldType::Binary, OFTBinary},
    {eGeoFieldType::Time, OFTTime},
    {eGeoFieldType::Date, OFTDate},
    {eGeoFieldType::DateTime, OFTDateTime},
    {eGeoFieldType::Long, OFTInteger64},

};

const static std::string DATE_TIME_FMT = "{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d} TZ{}";
// 将 suRing 转换为 OGRPolygon
OGRLinearRing silly_gdal::silly_ring_to_ogr(const suRing& ring)
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

// 环OGRLinearRing对象，将其转换为suRing对象  (环)
suRing silly_gdal::silly_ring_from_ogr(const OGRLinearRing* ring)
{
    suRing result;
    int pointCount = ring->getNumPoints();
    for (int i = 0; i < pointCount; i++)
    {
        double x = ring->getX(i);
        double y = ring->getY(i);
        result.points.push_back({x, y});
    }
    return result;
}

// 将 OGRPoint(单点) 转换为 suPoint(单点) 类型
suPoint silly_gdal::silly_point_from_ogr(const OGRPoint* ogrPoint)
{
    suPoint result(ogrPoint->getX(), ogrPoint->getY());
    return result;
}

// 将 suPoint(单点) 转换为 OGRPoint(单点) 类型
OGRPoint silly_gdal::silly_point_to_ogr(const suPoint& point)
{
    OGRPoint ogrPoint(point.x, point.y);
    return ogrPoint;
}

// 将 OGRMultiPoint(多点) 转换为 silly_multi_point(多点) 类型
silly_multi_point silly_gdal::silly_multi_point_from_ogr(const OGRMultiPoint* ogrMultiPoint)
{
    silly_multi_point multiPoint;
    int pointCount = ogrMultiPoint->getNumGeometries();
    for (int i = 0; i < pointCount; i++)
    {
        suPoint sillyPoint = silly_point_from_ogr(ogrMultiPoint->getGeometryRef(i));
        multiPoint.push_back(sillyPoint);
    }
    return multiPoint;
}

// 将 silly_multi_point(多点) 转换为 OGRMultiPoint(多点) 类型
OGRMultiPoint silly_gdal::silly_multi_point_to_ogr(const silly_multi_point& mulitPoint)
{
    OGRMultiPoint orgMultiPoint;
    for (const suPoint& point : mulitPoint)
    {
        OGRPoint ogrPoint = silly_point_to_ogr(point);
        // orgMultiPoint.addGeometryDirectly(ogrPoint.clone());
        orgMultiPoint.addGeometry(&ogrPoint);
    }
    return orgMultiPoint;
}

// OGRLineString(线)类型转为suLine(线)类型
suLine silly_gdal::silly_line_from_ogr(const OGRLineString* lineString)
{
    suLine line;
    int num_points = lineString->getNumPoints();
    for (int j = 0; j < num_points; j++)
    {
        line.push_back({lineString->getX(j), lineString->getY(j)});
    }
    return line;
}

// 将 suLine(线) 转换为 OGRLineString(线)类型
OGRLineString silly_gdal::silly_line_to_ogr(const suLine& line)
{
    OGRLineString ogrLineString;
    std::vector<double> xs(line.size());
    std::vector<double> ys(line.size());
    for (int i = 0; i < line.size(); i++)
    {
        xs[i] = line[i].x;
        ys[i] = line[i].y;
    }
    ogrLineString.setPoints(static_cast<int>(line.size()), &xs[0], &ys[0]);
    return ogrLineString;
}

// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
suMultiLine silly_gdal::silly_multi_line_from_ogr(const OGRMultiLineString* multiLineString)
{
    suMultiLine multiLine;
    int numLines = multiLineString->getNumGeometries();
    for (int i = 0; i < numLines; i++)
    {
        auto lineString = (OGRLineString*)multiLineString->getGeometryRef(i);
        if (lineString != nullptr)
        {
            suLine line = silly_line_from_ogr(lineString);
            multiLine.push_back(line);
        }
    }
    return multiLine;
}

// 将 silly_multiline(多线) 转换为 OGRMultiLineString(多线)类型
OGRMultiLineString silly_gdal::silly_multi_line_to_ogr(const suMultiLine& multiLine)
{
    OGRMultiLineString ogrMultiLineString;

    for (const suLine& line : multiLine)
    {
        OGRLineString ogrLineString = silly_line_to_ogr(line);
        ogrMultiLineString.addGeometry(&ogrLineString);
    }

    return ogrMultiLineString;
}

// OGRPolygon对象转换为suPoly(多环:外环+内环)对象  (单面)
suPoly silly_gdal::silly_poly_from_ogr(const OGRPolygon* polygon)
{
    suPoly poly;
    // 处理OGRPolygon外环
    auto outerRing = (OGRLinearRing*)polygon->getExteriorRing();
    poly.outer = silly_ring_from_ogr(outerRing);
    // 处理OGRPolygon内环
    int innerRingCount = polygon->getNumInteriorRings();
    for (int k = 0; k < innerRingCount; k++)
    {
        auto ring = (OGRLinearRing*)polygon->getInteriorRing(k);
        suRing innerRing = silly_ring_from_ogr(ring);
        poly.holes.push_back(innerRing);
    }
    return poly;
}

// 将 suPoly 转换为 OGRPolygon(单面)
OGRPolygon silly_gdal::silly_poly_to_ogr(const suPoly& poly)
{
    OGRPolygon org;
    // 设置外环
    OGRLinearRing outerRing = silly_ring_to_ogr(poly.outer);
    // double a = outerRing.get_Area();
    org.addRing(&outerRing);
    // 设置内环
    for (const suRing& innerRing : poly.holes)
    {
        OGRLinearRing innerOGRRing = silly_ring_to_ogr(innerRing);
        org.addRing(&innerOGRRing);
        org.closeRings();
    }
    return org;
}

// 多面的OGRMultiPolygon对象转换为suMultiPoly(多面)
suMultiPoly silly_gdal::silly_multi_poly_from_ogr(const OGRMultiPolygon* multiPolygon)
{
    suMultiPoly multi_poly;
    int polygonCount = multiPolygon->getNumGeometries();
    for (int i = 0; i < polygonCount; i++)
    {
        suPoly tmp_poly;
        auto polygon = (OGRPolygon*)multiPolygon->getGeometryRef(i);
        tmp_poly = silly_poly_from_ogr(polygon);
        multi_poly.push_back(tmp_poly);
    }
    return multi_poly;
}

// 将suMultiPoly对象转换为OGRMultiPolygon对象(多面)
OGRMultiPolygon silly_gdal::silly_multi_poly_to_ogr(const suMultiPoly& multiPoly)
{
    OGRMultiPolygon ogrMultiPolygon;
    for (const suPoly& poly : multiPoly)
    {
        OGRPolygon ogrPolygon = silly_poly_to_ogr(poly);
        ogrMultiPolygon.addGeometry(&ogrPolygon);
    }

    return ogrMultiPolygon;
}

OGRGeometry* silly_gdal::silly_geo_coll_to_ogr(const silly_geo_coll& coll)
{
    switch (coll.m_type)
    {
        case eGeometryType::Point:
            return new OGRPoint(silly_point_to_ogr(coll.m_point));

        case eGeometryType::MultiPoint:
            return new OGRMultiPoint(silly_multi_point_to_ogr(coll.m_m_points));

        case eGeometryType::LineString:
            return new OGRLineString(silly_line_to_ogr(coll.m_line));

        case eGeometryType::MultiLineString:
            return new OGRMultiLineString(silly_multi_line_to_ogr(coll.m_m_lines));

        case eGeometryType::Polygon:
            return new OGRPolygon(silly_poly_to_ogr(coll.m_poly));

        case eGeometryType::MultiPolygon:
            return new OGRMultiPolygon(silly_multi_poly_to_ogr(coll.m_m_polys));

        default:
            SLOG_ERROR("Error: Unsupported type: {}");
            return nullptr;
    }
}

silly_geo_coll silly_gdal::silly_geo_coll_from_ogr(const OGRGeometry* geometry)
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
std::string silly_gdal::GradDriverName(const std::filesystem::path& file)
{
    static const std::unordered_map<std::string, std::string> DRIVER_NAMES = {{".shp", "ESRI Shapefile"}, {".tab", "Mapinfo File"}, {".geojson", "GeoJSON"}, {".sqlite", "SQLite"}, {".csv", "CSV"}, {".kml", "KML"}, {".gml", "GML"}, {".xlsx", "XLSX"}};
    std::string ext = TO_LOWER(file.extension().string());
    for (const auto& [extension, driver] : DRIVER_NAMES)
    {
        if (std::strcmp(ext.c_str(), extension.c_str()) == 0)
        {
            return driver;
        }
    }
    return "";
}
void* silly_gdal::GdalOpenDataset(const std::filesystem::path& file, const bool& read)
{
    if (read)
    {
        return GDALOpenEx(file.string().c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr);
    }
    std::string gdalDriverName = GradDriverName(file);
    if (gdalDriverName.empty())
    {
        SLOG_ERROR("无法确定写入文件类型{}", file.u8string());
        return nullptr;
    }

    GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName(gdalDriverName.c_str());
    return outDriver->Create(file.string().c_str(), 0, 0, 0, GDT_Unknown, nullptr);
}
bool silly_gdal::check_field_info(const std::filesystem::path& file, std::map<uint16_t, GeoFiledInfo>& properties)
{
    bool status = false;
    std::map<std::string, std::string> result;

    auto poDSr = static_cast<GDALDataset*>(GDALOpenEx(file.string().c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr));
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

    // 循环特征域
    int count = pFeature_r->GetFieldCount();
    for (uint16_t i = 0; i < count; i++)
    {
        GeoFiledInfo tmp;
        OGRFieldDefn* pDef = pFeature_r->GetFieldDefnRef(i);
        OGRFieldType fieldType = pDef->GetType();
        tmp.name = pDef->GetNameRef();
        if (IS_GBK(tmp.name))
        {
            tmp.u8name = silly_encode::gbk_utf8(tmp.name);
        }
        if (GEO_FIELD_TYPE_OGR2SU.find(fieldType) == GEO_FIELD_TYPE_OGR2SU.end())
        {
            continue;
        }
        tmp.type = GEO_FIELD_TYPE_OGR2SU.at(fieldType);
        tmp.index = i;
        properties[i] = tmp;
    }

    GDALClose(poDSr);
    status = true;
    return status;
}
bool silly_gdal::read_property(const OGRFeature* feature, const std::map<uint16_t, GeoFiledInfo>& properties, std::unordered_map<std::string, silly_geo_prop>& props)
{
    for (const auto& [index, info] : properties)
    {
        switch (info.type)
        {
            case eGeoFieldType::None:
                break;
            case eGeoFieldType::Int:
            {
                int value = feature->GetFieldAsInteger(index);
                props[info.u8name] = {value};
            }
            break;
            case eGeoFieldType::Numeric:
            {
                double value = feature->GetFieldAsDouble(index);
                props[info.u8name] = {value};
            }
            break;
            case eGeoFieldType::String:
            {
                std::string value = feature->GetFieldAsString(index);
                if (!IS_UTF8(value))
                {
                    value = silly_encode::gbk_utf8(value);
                }
                props[info.u8name] = {value};
            }
            break;
            case eGeoFieldType::Time:
            case eGeoFieldType::Date:
            case eGeoFieldType::DateTime:
            {
                int y = 0, m = 0, d = 0, h = 0, M = 0, s = 0, tzFlag;
                if (!feature->GetFieldAsDateTime(index, &y, &m, &d, &h, &M, &s, &tzFlag))
                {
                    props[info.u8name] = {SUFMT(DATE_TIME_FMT, y, m, d, h, M, s, tzFlag)};
                }
            }
            break;
            case eGeoFieldType::Long:
            {
                long long value = feature->GetFieldAsInteger64(index);
                props[info.u8name] = {value};
            }
            break;
            default:
                break;
        }
    }

    return true;
}
bool silly_gdal::read_all_types_data(const eGeometryType& feature_type, const OGRGeometry* geometry, silly_geo_coll& geo_coll)

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

bool silly_gdal::is_valid_shp(const std::filesystem::path& file)
{
    auto poDSr = static_cast<GDALDataset*>(GDALOpenEx(file.string().c_str(), GDAL_OF_ALL | GDAL_OF_READONLY, nullptr, nullptr, nullptr));
    if (nullptr == poDSr)
    {
        return false;
    }
    GDALClose(poDSr);
    return true;
}

std::vector<std::string> silly_gdal::shp_missing_file(const std::filesystem::path& file)
{
    std::vector<std::string> ret;
    static std::set<std::string> necessary_files{".shx", ".dbf"};
    // 默认情况下 GDAL 在加载 Shapefile 时，会 严格检查文件名大小写 ，并按照以下规则查找配套文件
    std::string ext = file.extension().string();
    if (ext != ".shp")
    {
        return ret;
    }
    std::string stem = file.stem().string();
    std::filesystem::path parent = file.parent_path();

    for (const auto& nf : necessary_files)
    {
        auto tmp = std::filesystem::path(parent).append(stem + nf);
        if (!std::filesystem::exists(tmp))
        {
            ret.push_back(tmp.filename().string());
        }
    }

    return ret;
}
bool silly_gdal::AddGeometry(OGRLayer* layer, const silly_geo_coll& geometry, bool writeProp)
{
    if (!layer || geometry.m_type == eGeometryType::Invalid)
    {
        return false;
    }

    OGRFeature* feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
    // 添加矢量
    OGRGeometry* pGeometry = OGRGeometryFactory::createGeometry((OGRwkbGeometryType)geometry.m_type);
    if (!geometry.m_props.empty() && writeProp)
    {
        if (!FillField(feature, geometry.m_props))  // 添加属性
        {
            SLOG_ERROR("写入属性失败")
        }
    }
    // 添加矢量数据
   bool  status = FillGeometry(geometry.m_type, layer, feature, pGeometry, geometry);

    OGRFeature::DestroyFeature(feature);
    OGRGeometryFactory::destroyGeometry(pGeometry);
    return status;
}
bool silly_gdal::FillGeometry(const eGeometryType& type, OGRLayer* outputLayer, OGRFeature* feature, OGRGeometry* geometry, const silly_geo_coll& geo_coll)
{
    bool status = false;
    switch (type)
    {
        case eGeometryType::Point:
            geometry = OGRPoint(geo_coll.m_point.x, geo_coll.m_point.y).clone();
            break;
        case eGeometryType::LineString:
            geometry = silly_line_to_ogr(geo_coll.m_line).clone();
            break;
        case eGeometryType::Polygon:
             geometry = silly_poly_to_ogr(geo_coll.m_poly).clone();
            break;
        case eGeometryType::MultiPoint:
            geometry = silly_multi_point_to_ogr(geo_coll.m_m_points).clone();
            break;
        case eGeometryType::MultiLineString:
           geometry = silly_multi_line_to_ogr(geo_coll.m_m_lines).clone();
            break;
        case eGeometryType::MultiPolygon:
            geometry = silly_multi_poly_to_ogr(geo_coll.m_m_polys).clone();

            break;
        default:
            status = false;
            break;
    }
    if (!geometry->IsEmpty())
    {
        feature->SetGeometry(geometry);
        if (outputLayer->CreateFeature(feature) == OGRERR_NONE)  // 在图层中创建要素
        {
            status = true;
        }
    }

    return status;
}
bool silly_gdal::FillField(OGRFeature* feature, const std::unordered_map<std::string, silly_geo_prop>& m_props)
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
                case eGeoFieldType::Time:
                case eGeoFieldType::Date:
                case eGeoFieldType::DateTime:
                    feature->SetField(fieldIndex, prop.as_string().c_str());
                    break;
                case eGeoFieldType::Long:
                    feature->SetField(fieldIndex, prop.as_int64());
                    break;
                default:
                    status = false;
                    break;
            }
        }
    }
    return status;
}

#endif