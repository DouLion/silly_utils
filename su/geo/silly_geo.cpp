//
// Created by dly on 2023/7/19.
//

#include "silly_geo.h"
#include <json/json.h>
#include "su_marco.h"

#define GEOJSON_KEY_FEATURES "features"
#define GEOJSON_KEY_GEOMETRY "geometry"
#define GEOJSON_KEY_TYPE "type"
#define GEOJSON_KEY_COORDINATES "coordinates"
#define GEOJSON_KEY_PROPERTIES "properties"
#define GEOJSON_KEY_GEOMETRIES "geometries"
#define GEOJSON_VAL_FEATURE_COLLECTION "FeatureCollection"
#define GEOJSON_VAL_FEATURE "Feature"

enum_geometry_types silly_geo::check_geojson_type(const std::string& content)
{
    enum_geometry_types ret_type = eInvalid;
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(content, root))
    {
        if (root.isMember(GEOJSON_KEY_FEATURES))
        {
            for (auto& f : root[GEOJSON_KEY_FEATURES])
            {
                if (f.isMember(GEOJSON_KEY_GEOMETRY))
                {
                    std::string type_str = f[GEOJSON_KEY_GEOMETRY].asString();
                    if (GEOJSON_GEOMETRY_POINT == type_str)
                    {
                        ret_type = ePoint;
                    }
                    else if (GEOJSON_GEOMETRY_LINE_STRING == type_str)
                    {
                        ret_type = eLineString;
                    }
                    else if (GEOJSON_GEOMETRY_POLYGON == type_str)
                    {
                        ret_type = ePolygon;
                    }
                    else if (GEOJSON_GEOMETRY_MULTI_POINT == type_str)
                    {
                        ret_type = eMultiPoint;
                    }
                    else if (GEOJSON_GEOMETRY_MULTI_LINE_STRING == type_str)
                    {
                        ret_type = eMultiLineString;
                    }
                    else if (GEOJSON_GEOMETRY_MULTI_POLYGON == type_str)
                    {
                        ret_type = eMultiPolygon;
                    }
                }
            }
        }
    }
    return ret_type;
}

std::string silly_geo::dump_geojson(const std::vector<silly_point> points)
{
    return std::string();
}

std::string silly_geo::dump_geojson(const std::vector<silly_line> lines)
{
    return std::string();
}

std::string silly_geo::dump_geojson(const std::vector<silly_poly> polys)
{
    std::string ret;
    if (polys.empty())
    {
        return ret;
    }
    Json::Value groot = Json::objectValue;
    groot[GEOJSON_KEY_TYPE] = GEOJSON_VAL_FEATURE_COLLECTION;
    groot[GEOJSON_KEY_FEATURES] = Json::arrayValue;
    for (auto& poly : polys)
    {
        Json::Value o = Json::objectValue;
        o[GEOJSON_KEY_TYPE] = GEOJSON_VAL_FEATURE;
        o[GEOJSON_KEY_GEOMETRY] = Json::objectValue;
        o[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_POLYGON;
        o[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_COORDINATES] = Json::arrayValue;

        // Json::Value coord_arr = Json::arrayValue;
        Json::Value outer_arr = Json::arrayValue;
        for (auto point : poly.outer_ring.points)
        {
            Json::Value point_arr = Json::arrayValue;
            point_arr.append(point.lgtd);
            point_arr.append(point.lttd);
            outer_arr.append(point_arr);
        }
        o[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_COORDINATES].append(outer_arr);

        for (auto& ring : poly.inner_rings)
        {
            Json::Value ring_arr = Json::arrayValue;
            for (auto point : ring.points)
            {
                Json::Value point_arr = Json::arrayValue;
                point_arr.append(point.lgtd);
                point_arr.append(point.lttd);
                ring_arr.append(point_arr);
            }
            o[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_COORDINATES].append(ring_arr);
        }
        groot[GEOJSON_KEY_FEATURES].append(o);
    }
    return Json::FastWriter().write(groot);
}

std::vector<silly_poly> silly_geo::load_geojson(const std::string& geojson)
{
    std::vector<silly_poly> ret_polys;
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(geojson, root))
    {
        if (root.isMember(GEOJSON_KEY_FEATURES))
        {
            for (auto& f : root[GEOJSON_KEY_FEATURES])
            {
                std::map<std::string, std::string> props;
                for (auto& key : f[GEOJSON_KEY_PROPERTIES].getMemberNames())
                {
                    if (f[GEOJSON_KEY_PROPERTIES][key].isString())
                    {
                        props[key] = f[GEOJSON_KEY_PROPERTIES][key].asString();
                    }
                    else if (f[GEOJSON_KEY_PROPERTIES][key].isInt())
                    {
                        props[key] = std::to_string(f[GEOJSON_KEY_PROPERTIES][key].asInt());
                    }
                    else if (f[GEOJSON_KEY_PROPERTIES][key].isDouble())
                    {
                        props[key] = std::to_string(f[GEOJSON_KEY_PROPERTIES][key].asDouble());
                    }
                }
                if (f.isMember(GEOJSON_KEY_GEOMETRY))
                {
                    std::string type_str = f[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_TYPE].asString();
                    if (GEOJSON_GEOMETRY_MULTI_POLYGON == type_str)
                    {
                        // 多面处理
                        for (auto& poly : f[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_COORDINATES])
                        {
                            silly_poly tmp_poly;
                            for (int i = 0; i < poly.size(); ++i)
                            {
                                silly_ring ring;
                                for (auto& pnt : poly[i])
                                {
                                    silly_point s_point;
                                    s_point.lgtd = pnt[0].asDouble();
                                    s_point.lttd = pnt[1].asDouble();
                                    ring.points.push_back(s_point);
                                }
                                if (0 == i)
                                {
                                    tmp_poly.outer_ring = ring;
                                }
                                else
                                {
                                    tmp_poly.inner_rings.push_back(ring);
                                }
                            }
                            // tmp_poly.props = props;
                            ret_polys.push_back(tmp_poly);
                        }
                    }
                    else if (GEOJSON_GEOMETRY_POLYGON == type_str)
                    {
                        // 单面处理
                    }
                }
            }
        }
    }

    return ret_polys;
}

// 读取单点数据类型
bool read_point(const Json::Value& root, silly_geo_coll& coll)
{
    bool status = false;
    if (root.isMember(GEOJSON_KEY_COORDINATES))
    {
        Json::Value coordinates = root[GEOJSON_KEY_COORDINATES];
        size_t numRing = coordinates.size();
        double lon = coordinates[0].asDouble();
        double lat = coordinates[1].asDouble();
        coll.m_point.lgtd = lon;
        coll.m_point.lttd = lat;
        status = true;
    }
    return status;
}

// 读取多点
bool read_multi_point(const Json::Value& root, silly_geo_coll& coll)
{
    bool status = false;
    if (root.isMember(GEOJSON_KEY_COORDINATES))
    {
        Json::Value coordinates = root[GEOJSON_KEY_COORDINATES];
        size_t numPoints = coordinates.size();
        for (const auto& coord : coordinates)
        {
            double lon = coord[0].asDouble();
            double lat = coord[1].asDouble();
            coll.m_m_points.push_back(silly_point(lon, lat));
        }
        status = true;
    }
    return status;
}

// 读取单线
bool read_line(const Json::Value& root, silly_line& coll_line)
{
    bool status = false;
    if (root.isMember(GEOJSON_KEY_COORDINATES))
    {
        Json::Value line = root[GEOJSON_KEY_COORDINATES];
        size_t numPoints = line.size();
        for (const auto& point : line)
        {
            double lon = point[0].asDouble();
            double lat = point[1].asDouble();
            coll_line.push_back(silly_point(lon, lat));
        }

        status = true;
    }
    return status;
}

// 读取多线数据类型
bool read_multi_line(const Json::Value& root, silly_geo_coll& coll)
{
    bool status = false;
    if (root.isMember(GEOJSON_KEY_COORDINATES))
    {
        Json::Value lines = root[GEOJSON_KEY_COORDINATES];
        size_t numLines = lines.size();
        for (const auto& line : lines)
        {
            silly_line temp_line;
            for (const auto& point : line)
            {
                double lon = point[0].asDouble();
                double lat = point[1].asDouble();
                temp_line.push_back(silly_point(lon, lat));
            }
            coll.m_m_lines.push_back(temp_line);
        }
        status = true;
    }
    return status;
}

// 读取环
bool read_rings(const Json::Value& coords, silly_ring& ring)
{
    for (const auto& coord : coords)
    {
        double lon = coord[0].asDouble();
        double lat = coord[1].asDouble();
        ring.points.push_back(silly_point(lon, lat));
    }
    return true;
}

/// <summary>
/// 读取Polygon类型的数据(单面:内环+外环)
/// </summary>
/// <param name="root"></param>
/// <param name="geo_coll"></param>
/// <returns></returns>
bool read_polygon(const Json::Value& root, silly_geo_coll& coll)
{
    bool status = false;
    if (root.isMember(GEOJSON_KEY_COORDINATES))
    {
        Json::Value coordinates = root[GEOJSON_KEY_COORDINATES];
        size_t numRing = coordinates.size();
        Json::ValueIterator it = coordinates.begin();
        if (numRing > 0)
        {
            status = read_rings(*it, coll.m_poly.outer_ring);  // 外环
        }
        for (size_t num = 1; (num < numRing) && (it != coordinates.end()); num++)  // 内环
        {
            it++;
            silly_ring temp_ring;
            read_rings(*it, temp_ring);
            coll.m_poly.inner_rings.push_back(temp_ring);
        }

        status = true;
    }
    return status;
}

/// <summary>
/// 读取MultiPolygon类型的数据(多面)
/// </summary>
/// <param name="root"></param>
/// <param name="coll"></param>
/// <returns></returns>
bool read_multi_polygon(const Json::Value& root, silly_geo_coll& coll)
{
    bool status = false;
    if (root.isMember(GEOJSON_KEY_COORDINATES))
    {
        Json::Value coordinates = root[GEOJSON_KEY_COORDINATES];
        size_t numPoly = coordinates.size();
        Json::ValueIterator it = coordinates.begin();
        for (size_t num = 0; (num < numPoly) && (it != coordinates.end()); num++)  // 逐个面
        {
            Json::Value poly_coords = *it;
            size_t numRing = poly_coords.size();
            Json::ValueIterator ring_it = poly_coords.begin();
            silly_poly poly;
            if (numRing > 0)  // 外环
            {
                status = read_rings(*ring_it, poly.outer_ring);
            }
            for (size_t ring_num = 1; (ring_num < numRing) && (ring_it != poly_coords.end()); ring_num++)  // 内环
            {
                ring_it++;
                silly_ring temp_ring;
                read_rings(*ring_it, temp_ring);
                poly.inner_rings.push_back(temp_ring);
            }

            coll.m_m_polys.push_back(poly);
            it++;
        }

        status = true;
    }

    return status;
}

enum_geometry_types enum_type(std::string type)
{
    if (type == GEOJSON_GEOMETRY_POINT)
    {  // 单点
        return ePoint;
    }
    else if (type == GEOJSON_GEOMETRY_LINE_STRING)
    {  // 单线
        return eLineString;
    }
    else if (type == GEOJSON_GEOMETRY_POLYGON)
    {  // 单面
        return ePolygon;
    }
    else if (type == GEOJSON_GEOMETRY_MULTI_POINT)
    {  // 多点
        return eMultiPoint;
    }
    else if (type == GEOJSON_GEOMETRY_MULTI_LINE_STRING)
    {  // 多线
        return eMultiLineString;
    }
    else if (type == GEOJSON_GEOMETRY_MULTI_POLYGON)
    {  // 多面
        return eMultiPolygon;
    }
    else if (type == GEOJSON_GEOMETRY_COLLECTION)
    {  // 复合
        return eCompositeType;
    }

    return eInvalid;
}

/// <summary>
/// 读取所有类型的数据
/// </summary>
/// <param name="type"></param>
/// <param name="root"></param>
/// <param name="coll"></param>
/// <returns></returns>
bool read_all_type_data(std::string type, const Json::Value& root, silly_geo_coll& coll)
{
    bool status = false;
    if (type == "")
    {
        coll.m_type = eInvalid;
    }
    else if (type == GEOJSON_GEOMETRY_POINT)
    {  // 单点
        coll.m_type = ePoint;
        status = read_point(root, coll);
    }
    else if (type == GEOJSON_GEOMETRY_LINE_STRING)
    {  // 单线
        coll.m_type = eLineString;
        status = read_line(root, coll.m_line);
    }
    else if (type == GEOJSON_GEOMETRY_POLYGON)
    {  // 单面
        coll.m_type = ePolygon;
        status = read_polygon(root, coll);
    }
    else if (type == GEOJSON_GEOMETRY_MULTI_POINT)
    {  // 多点
        coll.m_type = eMultiPoint;
        status = read_multi_point(root, coll);
    }
    else if (type == GEOJSON_GEOMETRY_MULTI_LINE_STRING)
    {  // 多线
        coll.m_type = eMultiLineString;
        status = read_multi_line(root, coll);
    }
    else if (type == GEOJSON_GEOMETRY_MULTI_POLYGON)
    {  // 多面
        coll.m_type = eMultiPolygon;
        status = read_multi_polygon(root, coll);
    }
    else if (type == GEOJSON_GEOMETRY_COLLECTION)
    {  // 复合数据类型
        if (root.isMember(GEOJSON_KEY_GEOMETRIES))
        {
            Json::Value geometries = root[GEOJSON_KEY_GEOMETRIES];
            for (const auto& geometry : geometries)
            {
                std::string temp_type = geometry[GEOJSON_KEY_TYPE].asString();
                coll.comp_type.push_back(enum_type(temp_type));
                read_all_type_data(temp_type, geometry, coll);  // 递归
            }
            coll.m_type = eCompositeType;
        }
    }
    else
    {
        coll.m_type = eInvalid;
    }
    return status;
}

silly_geo_coll silly_geo::load_geo_coll(const std::string& content)
{
    silly_geo_coll result_geo_coll;
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(content, root))
    {
        if (root.isMember(GEOJSON_KEY_TYPE))
        {
            // 判断数据类型
            std::string type = root[GEOJSON_KEY_TYPE].asString();
            // 读取数据
            read_all_type_data(type, root, result_geo_coll);
        }
    }
    else
    {
        SU_ERROR_PRINT("Error: Input JSON string format error");
    }

    return result_geo_coll;
}

// 写入单点
bool write_point(const silly_geo_coll& coll, Json::Value& coordinates)
{
    coordinates[0] = coll.m_point.lgtd;
    coordinates[1] = coll.m_point.lttd;
    return true;
}

// 写入多点
bool write_multi_point(const silly_geo_coll& coll, Json::Value& root)
{
    for (const auto& point : coll.m_m_points)
    {
        Json::Value coord;
        coord[0] = point.lgtd;
        coord[1] = point.lttd;
        root.append(coord);
    }

    return true;
}

// 写入单线
bool write_line(const silly_line& coll_line, Json::Value& root)
{
    Json::Value lineCoords(Json::arrayValue);

    for (const auto& point : coll_line)
    {
        Json::Value coord;
        coord[0] = point.lgtd;
        coord[1] = point.lttd;
        root.append(coord);
    }
    return true;
}

// 写入多线
bool write_multi_line(const silly_geo_coll& coll, Json::Value& root)
{
    Json::Value linesArray(Json::arrayValue);
    for (const auto& line : coll.m_m_lines)
    {
        Json::Value lineObj;
        write_line(line, lineObj);
        root.append(lineObj);
    }
    return true;
}

// 写入环
bool write_rings(const silly_ring& ring, Json::Value& coords)
{
    for (const auto& point : ring.points)
    {
        Json::Value coord;
        coord[0] = point.lgtd;
        coord[1] = point.lttd;
        coords.append(coord);
    }
    return true;
}

// 写入单面
bool write_polygon(const silly_poly& poly, Json::Value& polyCoords)
{
    write_rings(poly.outer_ring, polyCoords[polyCoords.size()]);
    for (const auto& innerRing : poly.inner_rings)
    {
        int b = polyCoords.size();
        write_rings(innerRing, polyCoords[polyCoords.size()]);
    }
    return true;
}

// 写入多面
bool write_multi_polygon(const silly_geo_coll& coll, Json::Value& coordinates)
{
    for (const auto& poly : coll.m_m_polys)
    {
        Json::Value polyCoords(Json::arrayValue);
        write_polygon(poly, polyCoords);
        coordinates.append(polyCoords);
    }
    return true;
}

// 处理所有类型的silly_geo_coll数据写入json
bool write_all_type_data(enum_geometry_types type, Json::Value& root, const silly_geo_coll& coll)
{
    bool status = false;
    switch (type)
    {
        case eInvalid:
        {
            root[GEOJSON_KEY_TYPE] = "";
        }
        break;
        case ePoint:  // 单点
        {
            root[GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_POINT;
            status = write_point(coll, root[GEOJSON_KEY_COORDINATES]);
        }
        break;
        case eLineString:  // 单线
        {
            root[GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_LINE_STRING;
            status = write_line(coll.m_line, root[GEOJSON_KEY_COORDINATES]);
        }
        break;
        case ePolygon:  // 单面
        {
            root[GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_POLYGON;
            status = write_polygon(coll.m_poly, root[GEOJSON_KEY_COORDINATES]);
        }
        break;
        case eMultiPoint:  // 多点
        {
            root[GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_MULTI_POINT;
            status = write_multi_point(coll, root[GEOJSON_KEY_COORDINATES]);
        }
        break;
        case eMultiLineString:  // 多线
        {
            root[GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_MULTI_LINE_STRING;
            status = write_multi_line(coll, root[GEOJSON_KEY_COORDINATES]);
        }
        break;
        case eMultiPolygon:  // 多面
        {
            root[GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_MULTI_POLYGON;
            status = write_multi_polygon(coll, root[GEOJSON_KEY_COORDINATES]);
        }
        break;
        case eCompositeType:  // 复合
        {
            root[GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_COLLECTION;
            for (const auto& type : coll.comp_type)
            {
                Json::Value branch;
                write_all_type_data(type, branch, coll);
                root[GEOJSON_KEY_GEOMETRIES].append(branch);
            }
        }
        break;
        default:
            break;
    }

    return status;
}

std::string silly_geo::dump_geo_coll(const silly_geo_coll& geo_coll)
{
    std::string result;

    Json::Value root;
    // 查看silly_geo_coll的数据类型
    enum_geometry_types coll_type = geo_coll.m_type;
    // 写入数据
    write_all_type_data(coll_type, root, geo_coll);

    // 将 Json::Value 转换为字符串
    Json::StreamWriterBuilder writerBuilder;
    result = Json::writeString(writerBuilder, root);

    return result;
}
