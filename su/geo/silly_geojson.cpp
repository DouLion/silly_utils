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

enum_geometry_type geojson::type(const std::string& str)
{
    if (GEOJSON_GEOMETRY_POINT == str)
    {
        return enum_geometry_type::egtPoint;
    }
    else if (GEOJSON_GEOMETRY_LINE_STRING == str)
    {
        return enum_geometry_type::egtLineString;
    }
    else if (GEOJSON_GEOMETRY_POLYGON == str)
    {
        return enum_geometry_type::egtPolygon;
    }
    else if (GEOJSON_GEOMETRY_MULTI_POINT == str)
    {
        return enum_geometry_type::egtMultiPoint;
    }
    else if (GEOJSON_GEOMETRY_MULTI_LINE_STRING == str)
    {
        return enum_geometry_type::egtMultiLineString;
    }
    else if (GEOJSON_GEOMETRY_MULTI_POLYGON == str)
    {
        return enum_geometry_type::egtMultiPolygon;
    }
    return enum_geometry_type::egtInvalid;
}

std::vector<silly_geo_coll> geojson::loadf(const std::string& file)
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
            case enum_geometry_type::egtPoint:
                gc.m_type = enum_geometry_type::egtPoint;
                read(jv[K_COORDINATES], gc.m_point);
                break;
            case enum_geometry_type::egtMultiPoint:
                gc.m_type = enum_geometry_type::egtMultiPoint;
                read(jv[K_COORDINATES], gc.m_m_points);
                break;
            case enum_geometry_type::egtLineString:
                gc.m_type = enum_geometry_type::egtLineString;
                read(jv[K_COORDINATES], gc.m_line);
                break;
            case enum_geometry_type::egtMultiLineString:
                gc.m_type = enum_geometry_type::egtMultiLineString;
                read(jv[K_COORDINATES], gc.m_m_lines);
                break;
            case enum_geometry_type::egtPolygon:
                gc.m_type = enum_geometry_type::egtPolygon;
                read(jv[K_COORDINATES], gc.m_poly);
                break;
            case enum_geometry_type::egtMultiPolygon:
                gc.m_type = enum_geometry_type::egtMultiPolygon;
                read(jv[K_COORDINATES], gc.m_m_polys);
                break;
            default:
                gc.m_type = enum_geometry_type::egtInvalid;
                break;
        }
        if (gc.m_type != enum_geometry_type::egtInvalid)
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
    if(TYPE_FEATURE_COLLECTION == type)
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
        case enum_geometry_type::egtPoint:
            gc.m_type = enum_geometry_type::egtPoint;
            read(jvGeo[K_COORDINATES], gc.m_point);
            break;
        case enum_geometry_type::egtMultiPoint:
            gc.m_type = enum_geometry_type::egtMultiPoint;
            read(jvGeo[K_COORDINATES], gc.m_m_points);
            break;
        case enum_geometry_type::egtLineString:
            gc.m_type = enum_geometry_type::egtLineString;
            read(jvGeo[K_COORDINATES], gc.m_line);
            break;
        case enum_geometry_type::egtMultiLineString:
            gc.m_type = enum_geometry_type::egtMultiLineString;
            read(jvGeo[K_COORDINATES], gc.m_m_lines);
            break;
        case enum_geometry_type::egtPolygon:
            gc.m_type = enum_geometry_type::egtPolygon;
            read(jvGeo[K_COORDINATES], gc.m_poly);
            break;
        case enum_geometry_type::egtMultiPolygon:
            gc.m_type = enum_geometry_type::egtMultiPolygon;
            read(jvGeo[K_COORDINATES], gc.m_m_polys);
            break;
        default:
            gc.m_type = enum_geometry_type::egtInvalid;
            break;
    }
    if (enum_geometry_type::egtInvalid == gc.m_type)
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
        if (poly.outer_ring.points.empty())
        {
            if (!read(jv_ring, poly.outer_ring))
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
            poly.inner_rings.push_back(ring);
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
            props[name] = silly_geo_prop(jv[name].asInt64());
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

