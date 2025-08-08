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
#define V_FEATURE "Feature"
#define V_FEATURE_COLLECTION "FeatureCollection"

std::vector<silly_geo_coll> geojson::read(const std::filesystem::path& file)
{
    Json::Value jv = jsonpp::loadf(file);
    return load(jv);
}

std::vector<silly_geo_coll> geojson::load(const std::string& geojson)
{
    Json::Value jv = jsonpp::loads(geojson);
    return load(jv);
}

std::vector<silly_geo_coll> geojson::load(const Json::Value& jv)
{
    std::vector<silly_geo_coll> ret;
    std::string type;
    sujson::check_str(jv, K_TYPE, type);
    if (V_FEATURE_COLLECTION == type)
    {
        Json::Value jvFeatures;
        if (jsonpp::check_arr(jv, K_FEATURES, jvFeatures))
        {
            for (auto& jvf : jvFeatures)
            {
                silly_geo_coll gc;
                if (load(jvf, gc))
                {
                    ret.push_back(gc);
                }
            }
        }
    }
    else if (V_FEATURE == type)
    {
        silly_geo_coll gc;
        if (load(jv, gc))
        {
            ret.push_back(gc);
        }
    }
    else if (type.empty())
    {
        silly_geo_coll gc;
        switch (sugeo::str2type(type))
        {
            case eGeometryType::Point:
                gc.m_type = eGeometryType::Point;
                load(jv[K_COORDINATES], gc.m_point);
                break;
            case eGeometryType::MultiPoint:
                gc.m_type = eGeometryType::MultiPoint;
                load(jv[K_COORDINATES], gc.m_m_points);
                break;
            case eGeometryType::LineString:
                gc.m_type = eGeometryType::LineString;
                load(jv[K_COORDINATES], gc.m_line);
                break;
            case eGeometryType::MultiLineString:
                gc.m_type = eGeometryType::MultiLineString;
                load(jv[K_COORDINATES], gc.m_m_lines);
                break;
            case eGeometryType::Polygon:
                gc.m_type = eGeometryType::Polygon;
                load(jv[K_COORDINATES], gc.m_poly);
                break;
            case eGeometryType::MultiPolygon:
                gc.m_type = eGeometryType::MultiPolygon;
                load(jv[K_COORDINATES], gc.m_m_polys);
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

bool geojson::load(const Json::Value& jv, silly_geo_coll& gc)
{
    std::string type;
    Json::Value jvGeo, jvProps;
    if (!jsonpp::check_str(jv, K_TYPE, type))
    {
        return false;
    }
    if (V_FEATURE_COLLECTION == type)
    {
        // 多元集合 在实现上会优先会使用递归,在结构上会也会递归,避免使用这种矢量结构
        throw std::runtime_error("不支持多元集合,有堆栈溢出风险");
    }
    if (V_FEATURE != type)
    {
        return false;
    }
    if (!jsonpp::check_obj(jv, K_GEOMETRY, jvGeo))
    {
        return false;
    }

    switch (sugeo::str2type(jvGeo[K_TYPE].asString()))
    {
        case eGeometryType::Point:
            gc.m_type = eGeometryType::Point;
            load(jvGeo[K_COORDINATES], gc.m_point);
            break;
        case eGeometryType::MultiPoint:
            gc.m_type = eGeometryType::MultiPoint;
            load(jvGeo[K_COORDINATES], gc.m_m_points);
            break;
        case eGeometryType::LineString:
            gc.m_type = eGeometryType::LineString;
            load(jvGeo[K_COORDINATES], gc.m_line);
            break;
        case eGeometryType::MultiLineString:
            gc.m_type = eGeometryType::MultiLineString;
            load(jvGeo[K_COORDINATES], gc.m_m_lines);
            break;
        case eGeometryType::Polygon:
            gc.m_type = eGeometryType::Polygon;
            load(jvGeo[K_COORDINATES], gc.m_poly);
            break;
        case eGeometryType::MultiPolygon:
            gc.m_type = eGeometryType::MultiPolygon;
            load(jvGeo[K_COORDINATES], gc.m_m_polys);
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
        if (!load(jvProps, gc.m_props))
        {
            // 提示?
        }
    }
    return true;
}

bool geojson::load(const Json::Value& jv, silly_point& point)
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

bool geojson::load(const Json::Value& jv, silly_multi_point& mpoint)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        silly_point point;
        if (!load(jv_point, point))
        {
            return false;
        }
        mpoint.push_back(point);
    }
    return true;
}

bool geojson::load(const Json::Value& jv, silly_line& line)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        silly_point point;
        if (!load(jv_point, point))
        {
            return false;
        }
        line.push_back(point);
    }

    return true;
}

bool geojson::load(const Json::Value& jv, silly_multi_line& mline)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_line : jv)
    {
        silly_line line;
        if (!load(jv_line, line))
        {
            return false;
        }
        mline.push_back(line);
    }
    return true;
}

bool geojson::load(const Json::Value& jv, silly_ring& ring)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        silly_point point;
        if (!load(jv_point, point))
        {
            return false;
        }
        ring.points.push_back(point);
    }
    return true;
}

bool geojson::load(const Json::Value& jv, silly_poly& poly)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_ring : jv)
    {
        if (poly.outer.points.empty())
        {
            if (!load(jv_ring, poly.outer))
            {
                return false;
            }
        }
        else
        {
            silly_ring ring;
            if (!load(jv_ring, ring))
            {
                return false;
            }
            poly.holes.push_back(ring);
        }
    }
    return true;
}

bool geojson::load(const Json::Value& jv, silly_multi_poly& mpoly)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_poly : jv)
    {
        silly_poly poly;
        if (!load(jv_poly, poly))
        {
            return false;
        }
        mpoly.push_back(poly);
    }
    return true;
}

bool geojson::load(const Json::Value& jv, std::map<std::string, silly_geo_prop>& props)
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
    return true;
}

std::string geojson::stringify(const std::vector<silly_geo_coll>& geo_colls, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    Json::Value jvObj = Json::objectValue;
    jvObj[K_TYPE] = V_FEATURE_COLLECTION;
    Json::Value jvArr = Json::arrayValue;
    for (const auto& item : geo_colls)
    {
        jvArr.append(jsonify(item));
    }
    jvObj[K_FEATURES] = jvArr;
    return sujson::stringify(jvObj, opt);
}
std::string geojson::stringify(const std::vector<silly_point>& points, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    Json::Value jvArr = Json::arrayValue;
    for (const auto& item : points)
    {
        jvArr.append(jsonify(item));
    }
    return sujson::stringify(jvArr, opt);
}
std::string geojson::stringify(const std::vector<silly_line>& lines, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    Json::Value jvArr = Json::arrayValue;
    for (const auto& item : lines)
    {
        jvArr.append(jsonify(item));
    }
    return sujson::stringify(jvArr, opt);
}
std::string geojson::stringify(const std::vector<silly_poly>& polys, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    Json::Value jvArr = Json::arrayValue;
    for (const auto& item : polys)
    {
        jvArr.append(jsonify(item));
    }
    return sujson::stringify(jvArr, opt);
}
std::string geojson::stringify(const std::vector<silly_multi_point>& mpoints, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    Json::Value jvArr = Json::arrayValue;
    for (const auto& gc : mpoints)
    {
        jvArr.append(jsonify(gc));
    }
    return sujson::stringify(jvArr, opt);
}
std::string geojson::stringify(const std::vector<silly_multi_line>& mlines, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    Json::Value jvArr = Json::arrayValue;
    for (const auto& gc : mlines)
    {
        jvArr.append(jsonify(gc));
    }
    return sujson::stringify(jvArr, opt);
}
std::string geojson::stringify(const std::vector<silly_multi_poly>& mpolys, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    Json::Value jvArr = Json::arrayValue;
    for (const auto& gc : mpolys)
    {
        jvArr.append(jsonify(gc));
    }
    return sujson::stringify(jvArr, opt);
}

Json::Value geojson::jsonify(const silly_geo_coll& gc)
{
    Json::Value ret;
    switch (gc.m_type)
    {
        case eGeometryType::Point:
            ret = jsonify(gc.m_point);
            break;
        case eGeometryType::MultiPoint:
            ret = jsonify(gc.m_m_points);
            break;
        case eGeometryType::LineString:
            ret = jsonify(gc.m_line);
            break;
        case eGeometryType::MultiLineString:
            ret = jsonify(gc.m_m_lines);
            break;
        case eGeometryType::Polygon:
            ret = jsonify(gc.m_poly);
            break;
        case eGeometryType::MultiPolygon:
            ret = jsonify(gc.m_m_polys);
            break;
        default:
            return Json::nullValue;
    }
    Json::Value jvProp = Json::objectValue;
    if (!gc.m_props.empty())
    {
        for (const auto& [k, v] : gc.m_props)
        {
            switch (v.type())
            {
                case eGeoFieldType::Int:
                    jvProp[k] = v.as_int32();
                    break;
                case eGeoFieldType::Long:
                    jvProp[k] = v.as_int32();
                    break;
                case eGeoFieldType::Numeric:
                    jvProp[k] = v.as_double();
                    break;
                case eGeoFieldType::String:
                    jvProp[k] = v.as_string();
                    break;

                default:
                    jvProp[k] = Json::nullValue;
            }
        }
        ret[K_PROPERTIES] = jvProp;
    }
    return ret;
}
Json::Value geojson::jsonify(const silly_point& point)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    ret[K_GEOMETRY][K_TYPE] = sugeo::type2str(eGeometryType::Point);
    Json::Value jvPoint = Json::objectValue;
    jvPoint.append(point.x);
    jvPoint.append(point.y);
    ret[K_GEOMETRY] = jvPoint;
    return ret;
}
Json::Value geojson::jsonify(const silly_line& line)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    ret[K_GEOMETRY][K_TYPE] = sugeo::type2str(eGeometryType::LineString);
    Json::Value jvLine = Json::arrayValue;
    for (auto& pt : line)
    {
        Json::Value jvPoint = Json::arrayValue;
        jvPoint.append(pt.x);
        jvPoint.append(pt.y);
        jvLine.append(jvPoint);
    }
    ret[K_GEOMETRY] = jvLine;
    return ret;
}
Json::Value geojson::jsonify(const silly_poly& poly)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    jvGeo[K_TYPE] = sugeo::type2str(eGeometryType::Polygon);
    Json::Value jvPoly = Json::arrayValue;
    Json::Value jvOuter = Json::arrayValue;
    for (auto& pt : poly.outer.points)
    {
        Json::Value jvPoint = Json::arrayValue;
        jvPoint.append(pt.x);
        jvPoint.append(pt.y);
        jvOuter.append(jvPoint);
    }
    jvPoly.append(jvOuter);
    for (const auto& ring : poly.holes)
    {
        Json::Value jvHole = Json::arrayValue;
        for (const auto pt : ring.points)
        {
            Json::Value jvPoint = Json::arrayValue;
            jvPoint.append(pt.x);
            jvPoint.append(pt.y);
            jvHole.append(jvPoint);
        }
        jvPoly.append(jvHole);
    }

    jvGeo[K_COORDINATES] = jvPoly;
    ret[K_GEOMETRY] = jvGeo;
    return ret;
}
Json::Value geojson::jsonify(const silly_multi_point& mpoint)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    ret[K_GEOMETRY][K_TYPE] = sugeo::type2str(eGeometryType::MultiPoint);
    Json::Value jvPoints = Json::arrayValue;
    for (const auto& point : mpoint)
    {
        Json::Value jvPoint = Json::objectValue;
        jvPoint.append(point.x);
        jvPoint.append(point.y);
        jvPoints.append(jvPoint);
    }

    ret[K_GEOMETRY] = jvPoints;
    return ret;
}
Json::Value geojson::jsonify(const silly_multi_line& mline)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    ret[K_GEOMETRY][K_TYPE] = sugeo::type2str(eGeometryType::MultiLineString);
    Json::Value jvLines = Json::arrayValue;
    for (const auto& line : mline)
    {
        Json::Value jvLine = Json::arrayValue;
        for (auto& pt : line)
        {
            Json::Value jvPoint = Json::arrayValue;
            jvPoint.append(pt.x);
            jvPoint.append(pt.y);
            jvLine.append(jvPoint);
        }
        jvLines.append(jvLine);
    }

    ret[K_GEOMETRY] = jvLines;
    return ret;
}
Json::Value geojson::jsonify(const silly_multi_poly& mpoly)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    jvGeo[K_TYPE] = sugeo::type2str(eGeometryType::MultiPolygon);
    Json::Value jvPolys = Json::arrayValue;
    for (const auto& poly : mpoly)
    {
        Json::Value jvPoly = Json::arrayValue;
        Json::Value jvOuter = Json::arrayValue;
        for (auto& pt : poly.outer.points)
        {
            Json::Value jvPoint = Json::arrayValue;
            jvPoint.append(pt.x);
            jvPoint.append(pt.y);
            jvOuter.append(jvPoint);
        }
        jvPoly.append(jvOuter);
        for (const auto& ring : poly.holes)
        {
            Json::Value jvHole = Json::arrayValue;
            for (const auto pt : ring.points)
            {
                Json::Value jvPoint = Json::arrayValue;
                jvPoint.append(pt.x);
                jvPoint.append(pt.y);
                jvHole.append(jvPoint);
            }
            jvPoly.append(jvHole);
        }
        jvPolys.append(jvPoly);
    }

    jvGeo[K_COORDINATES] = jvPolys;
    ret[K_GEOMETRY] = jvGeo;
    return ret;
}

Json::Value geojson::stringify(const silly_geo_coll& geo_coll, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(geo_coll), opt);
}
std::string geojson::stringify(const silly_point& point, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(point), opt);
}
std::string geojson::stringify(const silly_line& line, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(line), opt);
}
std::string geojson::stringify(const silly_poly& poly, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(poly), opt);
}
std::string geojson::stringify(const silly_multi_point& mpoint, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(mpoint), opt);
}
std::string geojson::stringify(const silly_multi_line& mline, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(mline), opt);
}
std::string geojson::stringify(const silly_multi_poly& mpoly, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(mpoly), opt);
}

bool geojson::check(const Json::Value& jv, silly_point& point)
{
    if (sugeo::str2type(jv[K_TYPE].asString()) == eGeometryType::Point)
    {
        return load(jv[K_COORDINATES], point);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_multi_point& mpoint)
{
    if (sugeo::str2type(jv[K_TYPE].asString()) == eGeometryType::MultiPoint)
    {
        return load(jv[K_COORDINATES], mpoint);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_line& line)
{
    if (sugeo::str2type(jv[K_TYPE].asString()) == eGeometryType::LineString)
    {
        return load(jv[K_COORDINATES], line);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_multi_line& mline)
{
    if (sugeo::str2type(jv[K_TYPE].asString()) == eGeometryType::MultiLineString)
    {
        return load(jv[K_COORDINATES], mline);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_poly& poly)
{
    if (sugeo::str2type(jv[K_TYPE].asString()) == eGeometryType::Polygon)
    {
        return load(jv[K_COORDINATES], poly);
    }
    return false;
}
bool geojson::check(const Json::Value& jv, silly_multi_poly& mpoly)
{
    if (sugeo::str2type(jv[K_TYPE].asString()) == eGeometryType::MultiPolygon)
    {
        return load(jv[K_COORDINATES], mpoly);
    }
    return false;
}
