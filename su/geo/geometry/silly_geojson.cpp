//
// Created by dell on 2024/1/25.
//

#include <geo/geometry/silly_geojson.h>
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

std::vector<suGeoColl> suGeoJson::read(const suPath& file)
{
    Json::Value jv = jsonpp::loadf(file);
    return load(jv);
}

std::vector<suGeoColl> suGeoJson::load(const std::string& geojson)
{
    Json::Value jv = jsonpp::loads(geojson);
    return load(jv);
}

std::vector<suGeoColl> suGeoJson::load(const Json::Value& jv)
{
    std::vector<suGeoColl> ret;
    std::string type;
    sujson::check_str(jv, K_TYPE, type);
    if (V_FEATURE_COLLECTION == type)
    {
        Json::Value jvFeatures;
        if (jsonpp::check_arr(jv, K_FEATURES, jvFeatures))
        {
            for (auto& jvf : jvFeatures)
            {
                suGeoColl gc;
                if (load(jvf, gc))
                {
                    ret.push_back(gc);
                }
            }
        }
    }
    else if (V_FEATURE == type)
    {
        suGeoColl gc;
        if (load(jv, gc))
        {
            ret.push_back(gc);
        }
    }
    else if (type.empty())
    {
        suGeoColl gc;
        suPoint _p;
        suMultiPoint _mp;
        suLine _l;
        suMultiLine _ml;
        suPoly _pl;
        suMultiPoly _mpl;
        switch (suGeoStr2Type(type))
        {
            case eGeometryType::Point:
                load(jv[K_COORDINATES], _p);
                gc.set(_p);
                break;
            case eGeometryType::MultiPoint:
                load(jv[K_COORDINATES], _mp);
                gc.set(_mp);
                break;
            case eGeometryType::LineString:
                load(jv[K_COORDINATES], _l);
                gc.set(_l);
                break;
            case eGeometryType::MultiLineString:
                load(jv[K_COORDINATES], _ml);
                gc.set(_ml);
                break;
            case eGeometryType::Polygon:
                load(jv[K_COORDINATES], _pl);
                gc.set(_pl);
                break;
            case eGeometryType::MultiPolygon:
                load(jv[K_COORDINATES], _mpl);
                gc.set(_mpl);
                break;
            default:
                break;
        }
        if (gc.type() != eGeometryType::Invalid)
        {
            ret.push_back(gc);
        }
    }

    return ret;
}

bool suGeoJson::load(const Json::Value& jv, suGeoColl& gc)
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
    suPoint _p;
    suMultiPoint _mp;
    suLine _l;
    suMultiLine _ml;
    suPoly _pl;
    suMultiPoly _mpl;
    switch (suGeoStr2Type(jvGeo[K_TYPE].asString()))
    {
        case eGeometryType::Point:
            load(jv[K_COORDINATES], _p);
            gc.set(_p);
            break;
        case eGeometryType::MultiPoint:
            load(jv[K_COORDINATES], _mp);
            gc.set(_mp);
            break;
        case eGeometryType::LineString:
            load(jv[K_COORDINATES], _l);
            gc.set(_l);
            break;
        case eGeometryType::MultiLineString:
            load(jv[K_COORDINATES], _ml);
            gc.set(_ml);
            break;
        case eGeometryType::Polygon:
            load(jv[K_COORDINATES], _pl);
            gc.set(_pl);
            break;
        case eGeometryType::MultiPolygon:
            load(jv[K_COORDINATES], _mpl);
            gc.set(_mpl);
            break;
        default:
            break;
    }
    if (eGeometryType::Invalid == gc.type())
    {
        return false;
    }
    if (jsonpp::check_obj(jv, K_PROPERTIES, jvProps))
    {
        if (!load(jvProps, gc.properties()))
        {
            // 提示?
        }
    }
    return true;
}

bool suGeoJson::load(const Json::Value& jv, suPoint& point)
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

bool suGeoJson::load(const Json::Value& jv, suMultiPoint& mpoint)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        suPoint point;
        if (!load(jv_point, point))
        {
            return false;
        }
        mpoint.push_back(point);
    }
    return true;
}

bool suGeoJson::load(const Json::Value& jv, suLine& line)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        suPoint point;
        if (!load(jv_point, point))
        {
            return false;
        }
        line.push_back(point);
    }

    return true;
}

bool suGeoJson::load(const Json::Value& jv, suMultiLine& mline)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_line : jv)
    {
        suLine line;
        if (!load(jv_line, line))
        {
            return false;
        }
        mline.push_back(line);
    }
    return true;
}

bool suGeoJson::load(const Json::Value& jv, suRing& ring)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_point : jv)
    {
        suPoint point;
        if (!load(jv_point, point))
        {
            return false;
        }
        ring.points.push_back(point);
    }
    return true;
}

bool suGeoJson::load(const Json::Value& jv, suPoly& poly)
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
            suRing ring;
            if (!load(jv_ring, ring))
            {
                return false;
            }
            poly.holes.push_back(ring);
        }
    }
    return true;
}

bool suGeoJson::load(const Json::Value& jv, suMultiPoly& mpoly)
{
    if (!jv.isArray())
        return false;
    for (auto& jv_poly : jv)
    {
        suPoly poly;
        if (!load(jv_poly, poly))
        {
            return false;
        }
        mpoly.push_back(poly);
    }
    return true;
}

bool suGeoJson::load(const Json::Value& jv, std::unordered_map<std::string, suGeoProp>& props)
{
    if (!jv.isObject())
        return false;
    for (auto name : jv.getMemberNames())
    {
        if (jv[name].isString())
        {
            props[name] = suGeoProp(jv[name].asString());
        }
        else if (jv[name].isInt64())
        {
            props[name] = suGeoProp(static_cast<long long>(jv[name].asInt64()));
        }
        else if (jv[name].isDouble())
        {
            props[name] = suGeoProp(jv[name].asDouble());
        }
        else if (jv[name].isInt())
        {
            props[name] = suGeoProp(jv[name].asInt());
        }
    }
    return true;
}

std::string suGeoJson::stringify(const std::vector<suGeoColl>& geo_colls, const int& precision)
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

std::string suGeoJson::stringify(const std::vector<suPoint>& points, const int& precision)
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

std::string suGeoJson::stringify(const std::vector<suLine>& lines, const int& precision)
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

std::string suGeoJson::stringify(const std::vector<suPoly>& polys, const int& precision)
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

std::string suGeoJson::stringify(const std::vector<suMultiPoint>& mpoints, const int& precision)
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

std::string suGeoJson::stringify(const std::vector<suMultiLine>& mlines, const int& precision)
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

std::string suGeoJson::stringify(const std::vector<suMultiPoly>& mpolys, const int& precision)
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

Json::Value suGeoJson::jsonify(const suGeoColl& gc)
{
    Json::Value ret;
    switch (gc.type())
    {
        case eGeometryType::Point:
            ret = jsonify(gc.point());
            break;
        case eGeometryType::MultiPoint:
            ret = jsonify(gc.multiPoint());
            break;
        case eGeometryType::LineString:
            ret = jsonify(gc.line());
            break;
        case eGeometryType::MultiLineString:
            ret = jsonify(gc.multiLine());
            break;
        case eGeometryType::Polygon:
            ret = jsonify(gc.poly());
            break;
        case eGeometryType::MultiPolygon:
            ret = jsonify(gc.multiLine());
            break;
        default:
            return Json::nullValue;
    }
    Json::Value jvProp = Json::objectValue;
    if (!gc.properties().empty())
    {
        for (const auto& [k, v] : gc.properties())
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

Json::Value suGeoJson::jsonify(const suPoint& point)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    ret[K_GEOMETRY][K_TYPE] = suGeoType2Str(eGeometryType::Point);
    Json::Value jvPoint = Json::arrayValue;
    jvPoint.append(point.x);
    jvPoint.append(point.y);
    ret[K_GEOMETRY][K_COORDINATES] = jvPoint;
    return ret;
}

Json::Value suGeoJson::jsonify(const suLine& line)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    ret[K_GEOMETRY][K_TYPE] = suGeoType2Str(eGeometryType::LineString);
    Json::Value jvLine = Json::arrayValue;
    for (auto& pt : line)
    {
        Json::Value jvPoint = Json::arrayValue;
        jvPoint.append(pt.x);
        jvPoint.append(pt.y);
        jvLine.append(jvPoint);
    }
    ret[K_GEOMETRY][K_COORDINATES] = jvLine;
    return ret;
}

Json::Value suGeoJson::jsonify(const suPoly& poly)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    jvGeo[K_TYPE] = suGeoType2Str(eGeometryType::Polygon);
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
    ret[K_GEOMETRY][K_COORDINATES] = jvGeo;
    return ret;
}

Json::Value suGeoJson::jsonify(const suMultiPoint& mpoint)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    ret[K_GEOMETRY][K_TYPE] = suGeoType2Str(eGeometryType::MultiPoint);
    Json::Value jvPoints = Json::arrayValue;
    for (const auto& point : mpoint)
    {
        Json::Value jvPoint = Json::arrayValue;
        jvPoint.append(point.x);
        jvPoint.append(point.y);
        jvPoints.append(jvPoint);
    }

    ret[K_GEOMETRY][K_COORDINATES] = jvPoints;
    return ret;
}

Json::Value suGeoJson::jsonify(const suMultiLine& mline)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    ret[K_GEOMETRY][K_TYPE] = suGeoType2Str(eGeometryType::MultiLineString);
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

    ret[K_GEOMETRY][K_COORDINATES] = jvLines;
    return ret;
}

Json::Value suGeoJson::jsonify(const suMultiPoly& mpoly)
{
    Json::Value ret = Json::objectValue;
    ret[K_TYPE] = V_FEATURE;
    Json::Value jvGeo = Json::objectValue;
    jvGeo[K_TYPE] = suGeoType2Str(eGeometryType::MultiPolygon);
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
    ret[K_GEOMETRY][K_COORDINATES] = jvGeo;
    return ret;
}

Json::Value suGeoJson::stringify(const suGeoColl& geo_coll, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(geo_coll), opt);
}

std::string suGeoJson::stringify(const suPoint& point, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(point), opt);
}

std::string suGeoJson::stringify(const suLine& line, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(line), opt);
}

std::string suGeoJson::stringify(const suPoly& poly, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(poly), opt);
}

std::string suGeoJson::stringify(const suMultiPoint& mpoint, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(mpoint), opt);
}

std::string suGeoJson::stringify(const suMultiLine& mline, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(mline), opt);
}

std::string suGeoJson::stringify(const suMultiPoly& mpoly, const int& precision)
{
    sujson::style opt;
    opt.precision = precision;
    return sujson::stringify(jsonify(mpoly), opt);
}

bool suGeoJson::check(const Json::Value& jv, suPoint& point)
{
    if (suGeoStr2Type(jv[K_TYPE].asString()) == eGeometryType::Point)
    {
        return load(jv[K_COORDINATES], point);
    }
    return false;
}

bool suGeoJson::check(const Json::Value& jv, suMultiPoint& mpoint)
{
    if (suGeoStr2Type(jv[K_TYPE].asString()) == eGeometryType::MultiPoint)
    {
        return load(jv[K_COORDINATES], mpoint);
    }
    return false;
}

bool suGeoJson::check(const Json::Value& jv, suLine& line)
{
    if (suGeoStr2Type(jv[K_TYPE].asString()) == eGeometryType::LineString)
    {
        return load(jv[K_COORDINATES], line);
    }
    return false;
}

bool suGeoJson::check(const Json::Value& jv, suMultiLine& mline)
{
    if (suGeoStr2Type(jv[K_TYPE].asString()) == eGeometryType::MultiLineString)
    {
        return load(jv[K_COORDINATES], mline);
    }
    return false;
}

bool suGeoJson::check(const Json::Value& jv, suPoly& poly)
{
    if (suGeoStr2Type(jv[K_TYPE].asString()) == eGeometryType::Polygon)
    {
        return load(jv[K_COORDINATES], poly);
    }
    return false;
}

bool suGeoJson::check(const Json::Value& jv, suMultiPoly& mpoly)
{
    if (suGeoStr2Type(jv[K_TYPE].asString()) == eGeometryType::MultiPolygon)
    {
        return load(jv[K_COORDINATES], mpoly);
    }
    return false;
}