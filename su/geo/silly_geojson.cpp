//
// Created by dell on 2024/1/25.
//

#include <geo/silly_geojson.h>
#include <files/silly_file.h>
using namespace silly;

// geojson中的key
#define K_FEATURES "features"
#define K_GEOMETRY "geometry"
#define K_TYPE "type"
#define K_COORDINATES "coordinates"
#define K_PROPERTIES "properties"

// geojson中的值
static const std::string TYPE_FEATURE = "Feature";
static const std::string TYPE_FEATURE_COLLECTION = "FeatureCollection";

eGeometryType geojson::type(const std::string& str)
{
    if (GEOJSON_GEOMETRY_POINT == str)
    {
        return eGeometryType::Point;
    }
    else if (GEOJSON_GEOMETRY_LINE_STRING == str)
    {
        return eGeometryType::LineString;
    }
    else if (GEOJSON_GEOMETRY_POLYGON == str)
    {
        return eGeometryType::Polygon;
    }
    else if (GEOJSON_GEOMETRY_MULTI_POINT == str)
    {
        return eGeometryType::MultiPoint;
    }
    else if (GEOJSON_GEOMETRY_MULTI_LINE_STRING == str)
    {
        return eGeometryType::MultiLineString;
    }
    else if (GEOJSON_GEOMETRY_MULTI_POLYGON == str)
    {
        return eGeometryType::MultiPolygon;
    }
    return eGeometryType::Invalid;
}

std::vector<silly_geo_coll> geojson::loadf(const std::filesystem::path& file)
{
    Json::Value jv_root = jsonpp::loadf(file);
    return loadj(jv_root);
}

std::vector<silly_geo_coll> geojson::loads(const std::string& geojson)
{
    Json::Value jv_root = jsonpp::loads(geojson);
    return loadj(jv_root);
}

std::vector<silly_geo_coll> geojson::loadj(const Json::Value& jv)
{
    std::vector<silly_geo_coll> ret;
    std::string type;
    silly::jsonpp::check_str(jv, K_TYPE, type);
    if (TYPE_FEATURE_COLLECTION == type)
    {
        Json::Value jvFeatures;
        if (jsonpp::check_arr(jv, K_FEATURES, jvFeatures))
        {
            for (auto& jvf : jvFeatures)
            {
                silly_geo_coll gc;
                if (read(jvf, gc))
                {
                    ret.push_back(gc);
                }
            }
        }
    }
    else if (TYPE_FEATURE == type)
    {
        silly_geo_coll gc;
        if (read(jv, gc))
        {
            ret.push_back(gc);
        }
    }
    else
    {
        silly_geo_coll gc;
        switch (geojson::type(type))
        {
            case eGeometryType::Point:
                gc.m_type = eGeometryType::Point;
                read(jv[K_COORDINATES], gc.m_point);
                break;
            case eGeometryType::MultiPoint:
                gc.m_type = eGeometryType::MultiPoint;
                read(jv[K_COORDINATES], gc.m_m_points);
                break;
            case eGeometryType::LineString:
                gc.m_type = eGeometryType::LineString;
                read(jv[K_COORDINATES], gc.m_line);
                break;
            case eGeometryType::MultiLineString:
                gc.m_type = eGeometryType::MultiLineString;
                read(jv[K_COORDINATES], gc.m_m_lines);
                break;
            case eGeometryType::Polygon:
                gc.m_type = eGeometryType::Polygon;
                read(jv[K_COORDINATES], gc.m_poly);
                break;
            case eGeometryType::MultiPolygon:
                gc.m_type = eGeometryType::MultiPolygon;
                read(jv[K_COORDINATES], gc.m_m_polys);
                break;
            default:
                gc.m_type = eGeometryType::Invalid;
                break;
        }
        if (gc.m_type != eGeometryType::Invalid)
        {
            ret.push_back(gc);
        }
    }

    return ret;
}

bool geojson::read(const Json::Value& jv, silly_geo_coll& gc)
{
    std::string type;
    Json::Value jvGeo, jvProps;
    if (!jsonpp::check_str(jv, K_TYPE, type))
    {
        return false;
    }
    if (TYPE_FEATURE_COLLECTION == type)
    {
        // 多元集合 在实现上会优先会使用递归,在结构上会也会递归,避免使用这种矢量结构
        throw std::runtime_error("不支持多元集合,有堆栈溢出风险");
    }
    if (TYPE_FEATURE != type)
    {
        return false;
    }
    if (!jsonpp::check_obj(jv, K_GEOMETRY, jvGeo))
    {
        return false;
    }

    switch (geojson::type(jvGeo[K_TYPE].asString()))
    {
        case eGeometryType::Point:
            gc.m_type = eGeometryType::Point;
            read(jvGeo[K_COORDINATES], gc.m_point);
            break;
        case eGeometryType::MultiPoint:
            gc.m_type = eGeometryType::MultiPoint;
            read(jvGeo[K_COORDINATES], gc.m_m_points);
            break;
        case eGeometryType::LineString:
            gc.m_type = eGeometryType::LineString;
            read(jvGeo[K_COORDINATES], gc.m_line);
            break;
        case eGeometryType::MultiLineString:
            gc.m_type = eGeometryType::MultiLineString;
            read(jvGeo[K_COORDINATES], gc.m_m_lines);
            break;
        case eGeometryType::Polygon:
            gc.m_type = eGeometryType::Polygon;
            read(jvGeo[K_COORDINATES], gc.m_poly);
            break;
        case eGeometryType::MultiPolygon:
            gc.m_type = eGeometryType::MultiPolygon;
            read(jvGeo[K_COORDINATES], gc.m_m_polys);
            break;
        default:
            gc.m_type = eGeometryType::Invalid;
            break;
    }
    if (eGeometryType::Invalid == gc.m_type)
    {
        return false;
    }
    if (jsonpp::check_obj(jv, K_PROPERTIES, jvProps))
    {
        if (!read(jvProps, gc.m_props))
        {
            // 提示?
        }
    }
    return true;
}

bool geojson::read(const Json::Value& jv, silly_point& point)
{
    if (!jv.isArray())
        return false;
    if (jv.size() >= 2)
    {
        point.x = jv[0].asDouble();
        point.y = jv[1].asDouble();
        return true;
    }
    /*else if(jv.size() == 3)
    {
        // 必要时再考虑高程
    }*/
    return false;
}

bool geojson::read(const Json::Value& jv, silly_multi_point& mpoint)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        silly_point point;
        if (!read(jv_point, point))
        {
            return false;
        }
        mpoint.push_back(point);
    }
    return true;
}

bool geojson::read(const Json::Value& jv, silly_line& line)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        silly_point point;
        if (!read(jv_point, point))
        {
            return false;
        }
        line.push_back(point);
    }

    return true;
}

bool geojson::read(const Json::Value& jv, silly_multi_line& mline)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_line : jv)
    {
        silly_line line;
        if (!read(jv_line, line))
        {
            return false;
        }
        mline.push_back(line);
    }
    return true;
}

bool geojson::read(const Json::Value& jv, silly_ring& ring)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        silly_point point;
        if (!read(jv_point, point))
        {
            return false;
        }
        ring.points.push_back(point);
    }
    return true;
}

bool geojson::read(const Json::Value& jv, silly_poly& poly)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_ring : jv)
    {
        if (poly.outer.points.empty())
        {
            if (!read(jv_ring, poly.outer))
            {
                return false;
            }
        }
        else
        {
            silly_ring ring;
            if (!read(jv_ring, ring))
            {
                return false;
            }
            poly.holes.push_back(ring);
        }
    }
    return true;
}

bool geojson::read(const Json::Value& jv, silly_multi_poly& mpoly)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_poly : jv)
    {
        silly_poly poly;
        if (!read(jv_poly, poly))
        {
            return false;
        }
        mpoly.push_back(poly);
    }
    return true;
}

bool geojson::read(const Json::Value& jv, std::map<std::string, silly_geo_prop>& props)
{
    if (!jv.isObject())
        return false;
    for (auto name : jv.getMemberNames())
    {
        if (jv[name].isString())
        {
            props[name] = silly_geo_prop(jv[name].asString());
        }
        else if (jv[name].isInt64())
        {
            props[name] = silly_geo_prop(static_cast<long long>(jv[name].asInt64()));
        }
        else if (jv[name].isDouble())
        {
            props[name] = silly_geo_prop(jv[name].asDouble());
        }
        else if (jv[name].isInt())
        {
            props[name] = silly_geo_prop(jv[name].asInt());
        }
    }
    return false;
}
std::string geojson::stringify(const std::vector<silly_point> points, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const std::vector<silly_line> lines, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const std::vector<silly_poly> polys, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const std::vector<silly_multi_point> mpoints, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const std::vector<silly_multi_line> mlines, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const std::vector<silly_multi_poly> mpolys, const int& precision)
{
    return std::string();
}
Json::Value geojson::jsonify(const silly_point point)
{
    return Json::Value();
}
Json::Value geojson::jsonify(const silly_line line)
{
    return Json::Value();
}
Json::Value geojson::jsonify(const silly_poly poly)
{
    Json::Value jv = Json::objectValue;
    jv[K_TYPE] = "Feature";
    Json::Value jvGeo = Json::objectValue;
    jvGeo[K_TYPE] = "Polygon";
    Json::Value jvCoords = Json::arrayValue;
    Json::Value jvOutRing = Json::arrayValue;
    for (auto& pt : poly.outer.points)
    {
        Json::Value jvPoint = Json::arrayValue;
        jvPoint.append(pt.x);
        jvPoint.append(pt.y);
        jvOutRing.append(jvPoint);
    }
    jvCoords.append(jvOutRing);
    for (auto& ring : poly.holes)
    {
        Json::Value jvInRing = Json::arrayValue;
        for (auto pt : ring.points)
        {
            Json::Value jvPoint = Json::arrayValue;
            jvPoint.append(pt.x);
            jvPoint.append(pt.y);
            jvInRing.append(jvPoint);
        }
        jvCoords.append(jvInRing);
    }

    jvGeo[K_COORDINATES] = jvCoords;
    jv[K_GEOMETRY] = jvGeo;
    return jv;
}
Json::Value geojson::jsonify(const silly_multi_point mpoint)
{
    return Json::Value();
}
Json::Value geojson::jsonify(const silly_multi_line mline)
{
    return Json::Value();
}
Json::Value geojson::jsonify(const silly_multi_poly mpoly)
{
    return Json::Value();
}
std::string geojson::stringify(const silly_point point, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const silly_line line, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const silly_poly poly, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return silly::jsonpp::stringify(jsonify(poly), opt);
}
std::string geojson::stringify(const silly_multi_point mpoint, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const silly_multi_line mline, const int& precision)
{
    return std::string();
}
std::string geojson::stringify(const silly_multi_poly mpoly, const int& precision)
{
    return std::string();
}

bool geojson::check(const Json::Value& jv, silly_point& point)
{
    if (GEOJSON_GEOMETRY_POINT == jv[K_TYPE].asString())
    {
        return read(jv[K_COORDINATES], point);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_multi_point& mpoint)
{
    if (GEOJSON_GEOMETRY_MULTI_POINT == jv[K_TYPE].asString())
    {
        return read(jv[K_COORDINATES], mpoint);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_line& line)
{
    if (GEOJSON_GEOMETRY_LINE_STRING == jv[K_TYPE].asString())
    {
        return read(jv[K_COORDINATES], line);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_multi_line& mline)
{
    if (GEOJSON_GEOMETRY_MULTI_LINE_STRING == jv[K_TYPE].asString())
    {
        return read(jv[K_COORDINATES], mline);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_poly& poly)
{
    if (GEOJSON_GEOMETRY_POLYGON == jv[K_TYPE].asString())
    {
        return read(jv[K_COORDINATES], poly);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_multi_poly& mpoly)
{
    if (GEOJSON_GEOMETRY_MULTI_POLYGON == jv[K_TYPE].asString())
    {
        return read(jv[K_COORDINATES], mpoly);
    }
    return false;
}
