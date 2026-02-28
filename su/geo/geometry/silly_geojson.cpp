//
// Created by dell on 2024/1/25.
//

#include <geo/geometry/silly_geojson.h>
#include <files/silly_file.h>

// geojson中的key
#define K_FEATURES "features"
#define K_GEOMETRY "geometry"
#define K_TYPE "type"
#define K_COORDINATES "coordinates"
#define K_PROPERTIES "properties"

// geojson中的值
#define V_FEATURE "Feature"
#define V_FEATURE_COLLECTION "FeatureCollection"

bool suGeoJson::Parse(const Json::Value& geojson, suPoint& meta)
{
    Json::Value jv = geojson;
    if (!jv.isArray())
    {
        if (jv.isObject())
        {
            if (jv.isMember(K_COORDINATES))
            {
                jv = geojson[K_COORDINATES];
            }
            else if (geojson.isMember(K_GEOMETRY))
            {
                jv = geojson[K_GEOMETRY][K_COORDINATES];
            }
        }
    }

    if (jv.isArray())
    {
        if (jv.size() == 2 && jv[0].isNumeric())
        {
            meta.x = jv[0].asDouble();
            meta.y = jv[1].asDouble();
            return true;
        }
    }
    return false;
}

bool suGeoJson::Parse(const Json::Value& geojson, suMultiPoint& meta)
{
    Json::Value jv = geojson;
    if (!jv.isArray())
    {
        if (jv.isObject())
        {
            if (jv.isMember(K_COORDINATES))
            {
                jv = geojson[K_COORDINATES];
            }
            else if (geojson.isMember(K_GEOMETRY))
            {
                jv = geojson[K_GEOMETRY][K_COORDINATES];
            }
        }
    }

    if (jv.isArray())
    {
        for (const auto& jvp : jv)
        {
            if (jvp.isArray() && jvp[0].isNumeric())
            {
                suPoint tmp;
                tmp.x = jvp[0].asDouble();
                tmp.y = jvp[1].asDouble();
                meta.push_back(tmp);
            }
            else
            {
                return false;
            }
        }
        return !meta.empty();
    }
    return false;
}

bool suGeoJson::Parse(const Json::Value& geojson, suLine& meta)
{
    Json::Value jv = geojson;
    if (!jv.isArray())
    {
        if (jv.isObject())
        {
            if (jv.isMember(K_COORDINATES))
            {
                jv = geojson[K_COORDINATES];
            }
            else if (geojson.isMember(K_GEOMETRY))
            {
                jv = geojson[K_GEOMETRY][K_COORDINATES];
            }
        }
    }

    if (jv.isArray())
    {
        for (const auto& jvp : jv)
        {
            if (jvp.isArray() && jvp[0].isNumeric())
            {
                suPoint tmp;
                tmp.x = jvp[0].asDouble();
                tmp.y = jvp[1].asDouble();
                meta.push_back(tmp);
            }
            else
            {
                return false;
            }
        }
        return !meta.empty();
    }
    return false;
}

bool suGeoJson::Parse(const Json::Value& geojson, suMultiLine& meta)
{
    Json::Value jv = geojson;
    if (!jv.isArray())
    {
        if (jv.isObject())
        {
            if (jv.isMember(K_COORDINATES))
            {
                jv = geojson[K_COORDINATES];
            }
            else if (geojson.isMember(K_GEOMETRY))
            {
                jv = geojson[K_GEOMETRY][K_COORDINATES];
            }
        }
    }

    if (jv.isArray())
    {
        for (const auto& jvl : jv)
        {
            suLine tmpl;
            if (!Parse(jvl, tmpl))
            {
                return false;
            }
            meta.push_back(tmpl);
        }
        return !meta.empty();
    }
    return false;
}

bool suGeoJson::Parse(const Json::Value& geojson, suPoly& meta)
{
    Json::Value jv = geojson;
    if (!jv.isArray())
    {
        if (jv.isObject())
        {
            if (jv.isMember(K_COORDINATES))
            {
                jv = geojson[K_COORDINATES];
            }
            else if (geojson.isMember(K_GEOMETRY))
            {
                jv = geojson[K_GEOMETRY][K_COORDINATES];
            }
        }
    }

    if (jv.isArray())
    {
        for (const auto& jvp : jv)
        {
            if (!jvp.isArray())
            {
                return false;
            }
            suRing tmpr;
            for (const auto jvr : jvp)
            {
                if (jvr.isArray() && jvr[0].isNumeric())
                {
                    suPoint tmp;
                    tmp.x = jvr[0].asDouble();
                    tmp.y = jvr[1].asDouble();
                    tmpr.points.push_back(tmp);
                }
                else
                {
                    std::cout << sujson::stringify(jvp) << std::endl;
                    return false;
                }
            }
            if (meta.outer.points.empty())
            {
                meta.outer = std::move(tmpr);
                meta.outer.is_outer = 1;
            }
            else
            {
                meta.holes.emplace_back(tmpr);
            }
        }
        return meta.outer.is_outer;
    }
    return false;
}

bool suGeoJson::Parse(const Json::Value& geojson, suMultiPoly& meta)
{
    Json::Value jv = geojson;

    if (!jv.isArray())
    {
        std::string ss = sujson::stringify(jv);
        if (jv.isObject())
        {
            if (jv.isMember(K_COORDINATES))
            {
                jv = jv[K_COORDINATES];
            }
            else if (jv.isMember(K_GEOMETRY))
            {
                jv = jv[K_GEOMETRY][K_COORDINATES];
            }
        }
    }

    if (jv.isArray())
    {
        for (const auto& jvp : jv)
        {
            suPoly tmp;
            if (!Parse(jvp, tmp))
            {
                return false;
            }
            meta.push_back(tmp);
        }
        return !meta.empty();
    }
    return false;
}

std::string suGeoJson::Stringify(const std::vector<suPoly>& polys)
{
    std::string ret;
    sujson::style opt;
    opt.precision = 8;
    Json::Value jvObj = Json::objectValue;
    jvObj[K_TYPE] = V_FEATURE_COLLECTION;
    Json::Value jvArr = Json::arrayValue;
    for (const auto& item : polys)
    {
        jvArr.append(jsonify(item));
    }
    jvObj[K_FEATURES] = jvArr;
    return sujson::stringify(jvObj, opt);
}

bool suGeoJson::Write(const suPath& file, const std::vector<suPoly>& polys)
{
    return sufile::write(file, Stringify(polys)) > 0;
}

bool suGeoJson::ParseProperties(const Json::Value& prop, std::unordered_map<std::string, suGeoProp>& kv, std::map<uint16_t, std::string>& sort)
{
    return false;
}

std::vector<suGeoColl> suGeoJson::Read(const suPath& file)
{
    Json::Value jv = sujson::read(file);
    return Parse(jv);
}

std::vector<suGeoColl> suGeoJson::Parse(const std::string& geojson)
{
    Json::Value jv = sujson::loads(geojson);
    return Parse(jv);
}

bool suGeoJson::Parse(const std::string& geojson, suGeoColl& gc)
{
    Json::Value jv = sujson::loads(geojson);
    if (!jv.isObject())
    {
        return false;
    }
    return Parse(jv, gc);
}

bool suGeoJson::Parse(const Json::Value& geojson, suGeoColl& gc)
{
    if (!geojson.isMember(K_GEOMETRY))
    {
        return false;
    }
    const Json::Value& jvGeo = geojson[K_GEOMETRY];
    if (!jvGeo.isMember(K_TYPE))
    {
        return false;
    }
    std::string type = jvGeo[K_TYPE].asString();
    const auto etp = suGeoStr2Type(type);
    if (etp == eGeometryType::Invalid)
    {
        return false;
    }
    if (etp == eGeometryType::Point)
    {
        suPoint meta;
        if (!Parse(jvGeo, meta))
        {
            return false;
        }
        gc.set(meta);
    }
    else if (etp == eGeometryType::MultiPoint)
    {
        suMultiPoint meta;
        if (!Parse(jvGeo, meta))
        {
            return false;
        }
    }
    else if (etp == eGeometryType::LineString)
    {
        suLine meta;
        if (!Parse(jvGeo, meta))
        {
            return false;
        }
        gc.set(meta);
    }
    else if (etp == eGeometryType::MultiLineString)
    {
        suMultiLine meta;
        if (!Parse(jvGeo, meta))
        {
            return false;
        }
        gc.set(meta);
    }
    else if (etp == eGeometryType::Polygon)
    {
        suPoly meta;
        if (!Parse(jvGeo, meta))
        {
            return false;
        }
        gc.set(meta);
    }
    else if (etp == eGeometryType::MultiPolygon)
    {
        suMultiPoly meta;
        if (!Parse(jvGeo, meta))
        {
            return false;
        }
    }

    return true;
}

std::vector<suGeoColl> suGeoJson::Parse(const Json::Value& jv)
{
    std::vector<suGeoColl> ret;
    std::string type;
    sujson::check_str(jv, K_TYPE, type);
    if (V_FEATURE_COLLECTION == type)
    {
        Json::Value jvFeatures;
        if (sujson::check_arr(jv, K_FEATURES, jvFeatures))
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
    if (!sujson::check_str(jv, K_TYPE, type))
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
    if (!sujson::check_obj(jv, K_GEOMETRY, jvGeo))
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
            load(jvGeo[K_COORDINATES], _p);
            gc.set(_p);
            break;
        case eGeometryType::MultiPoint:
            load(jvGeo[K_COORDINATES], _mp);
            gc.set(_mp);
            break;
        case eGeometryType::LineString:
            load(jvGeo[K_COORDINATES], _l);
            gc.set(_l);
            break;
        case eGeometryType::MultiLineString:
            load(jvGeo[K_COORDINATES], _ml);
            gc.set(_ml);
            break;
        case eGeometryType::Polygon:
            load(jvGeo[K_COORDINATES], _pl);
            gc.set(_pl);
            break;
        case eGeometryType::MultiPolygon:
            load(jvGeo[K_COORDINATES], _mpl);
            gc.set(_mpl);
            break;
        default:
            break;
    }
    if (eGeometryType::Invalid == gc.type())
    {
        return false;
    }
    if (sujson::check_obj(jv, K_PROPERTIES, jvProps))
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
            props[name] = suGeoProp(static_cast<int64_t>(jv[name].asInt64()));
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
            ret = jsonify(gc.asPoint());
            break;
        case eGeometryType::MultiPoint:
            ret = jsonify(gc.asMultiPoint());
            break;
        case eGeometryType::LineString:
            ret = jsonify(gc.asLine());
            break;
        case eGeometryType::MultiLineString:
            ret = jsonify(gc.asMultiLine());
            break;
        case eGeometryType::Polygon:
            ret = jsonify(gc.asPoly());
            break;
        case eGeometryType::MultiPolygon:
            ret = jsonify(gc.asMultiPoly());
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
                    jvProp[k] = v.asInt32();
                    break;
                case eGeoFieldType::Long:
                    jvProp[k] = v.asInt32();
                    break;
                case eGeoFieldType::Numeric:
                    jvProp[k] = v.asDouble();
                    break;
                case eGeoFieldType::String:
                    jvProp[k] = v.asString();
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
    ret[K_GEOMETRY] = jvGeo;
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
    ret[K_GEOMETRY] = jvGeo;
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

void suGeoJson::CheckParse()
{
    suGeoColl meta;
    {
        std::string geojson = R"(
{
	"type": "Feature",
	"geometry": {
		"type": "Point",
		"coordinates": [125.6, 10.1]
	},
	"properties": {
		"name": "Dinagat Islands"
	}
}
        )";
        // suPoint meta;
        if (!suGeoJson::Parse(geojson, meta))
        {
            SU_ERROR_PRINT("suPoint 错误")
        }
        else
        {
            SU_INFO_PRINT("suPoint 通过")
        }
    }
    {
        std::string geojson = R"(
{
	"type": "Feature",
	"geometry": {
		"type": "MultiPoint",
		"coordinates": [
			[105.380859375,31.57853542647338],
			[105.580859375,31.52853542647338]
		]
	}
}
        )";
        // suMultiPoint meta;
        if (!suGeoJson::Parse(geojson, meta))
        {
            SU_ERROR_PRINT("suMultiPoint 错误")
        }
        else
        {
            SU_INFO_PRINT("suMultiPoint 通过")
        }
    }
    {
        std::string geojson = R"(
{
	"geometry":{
		"type":"LineString",
		"coordinates":[
			[105.6005859375,30.65681556429287],
			[107.95166015624999,31.98944183792288],
			[109.3798828125,30.031055426540206],
			[107.7978515625,29.935895213372444]
		]
	}
}
        )";
        // suLine meta;
        if (!suGeoJson::Parse(geojson, meta))
        {
            SU_ERROR_PRINT("suLine 错误")
        }
        else
        {
            SU_INFO_PRINT("suLine 通过")
        }
    }
    {
        std::string geojson = R"(
{
	"geometry":{
		"type":"MultiLineString",
		"coordinates":[
			[
				[105.6005859375,30.65681556429287],
				[107.95166015624999,31.98944183792288],
				[109.3798828125,30.031055426540206],
				[107.7978515625,29.935895213372444]
			],
			[
				[109.3798828125,30.031055426540206],
				[107.1978515625,31.235895213372444]
			]
		]
	}
}
        )";
        // suMultiLine meta;
        if (!suGeoJson::Parse(geojson, meta))
        {
            SU_ERROR_PRINT("suMultiLine 错误")
        }
        else
        {
            SU_INFO_PRINT("suMultiLine 通过")
        }
    }
    {
        std::string geojson = R"(
{
	"geometry":{
		"type":"Polygon",
		"coordinates":[
			[
				[106.10595703125,33.33970700424026],
				[106.32568359375,32.41706632846282],
				[108.03955078125,32.2313896627376],
				[108.25927734375,33.15594830078649],
				[106.10595703125,33.33970700424026]
			]
		]
	}
}
        )";
        // suPoly meta;
        if (!suGeoJson::Parse(geojson, meta))
        {
            SU_ERROR_PRINT("suPoly 错误")
        }
        else
        {
            SU_INFO_PRINT("suPoly 通过")
        }
    }
    {
        std::string geojson = R"(
{
	"geometry": {
		"type": "MultiPolygon",
		"coordinates":[
			[
				[
					[109.2041015625,30.088107753367257],
					[115.02685546875,30.088107753367257],
					[115.02685546875,32.7872745269555],
					[109.2041015625,32.7872745269555],
					[109.2041015625,30.088107753367257]
				]
			],
			[
				[
					[112.9833984375,26.82407078047018],
					[116.69677734375,26.82407078047018],
					[116.69677734375,29.036960648558267],
					[112.9833984375,29.036960648558267],
					[112.9833984375,26.82407078047018]
				]
			]
		]
	}
}
        )";
        // suMultiPoly meta;
        if (!suGeoJson::Parse(geojson, meta))
        {
            SU_ERROR_PRINT("suMultiPoly 错误")
        }
        else
        {
            SU_INFO_PRINT("suMultiPoly 通过")
        }
    }
}