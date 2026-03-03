/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All max.xs
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/19 15:17
 * @version: 1.0.1
 * @software: silly_utils
 * @description: Computational Geometry  计算几何基本类型定义
 */
#pragma once

#ifndef SILLY_GEO_H
#define SILLY_GEO_H
#include <geo/common/silly_point.h>
#include <geo/common/silly_circle.h>
#include <geo/common/silly_ellipse.h>
#include <geo/common/silly_line.h>
#include <geo/common/silly_polygon.h>
#include <geo/common/silly_rect.h>
#include <geo/common/silly_segment.h>
#include <geo/common/silly_point.h>
#include <geo/common/silly_triangle.h>

// 参照GDAL
enum class eGeometryType : int8_t
{
    Invalid = 0,          // 无效
    Point = 1,            // 单点
    LineString = 2,       // 单线
    Polygon = 3,          // 单面
    MultiPoint = 4,       // 多点
    MultiLineString = 5,  // 多线
    MultiPolygon = 6,     // 多面
                          // egtCompositeType = 7,    // 复合数据类型
};
// 点
#define GEOJSON_GEOMETRY_POINT "Point"
// 线
#define GEOJSON_GEOMETRY_LINE_STRING "LineString"
// 面
#define GEOJSON_GEOMETRY_POLYGON "Polygon"
// 多点
#define GEOJSON_GEOMETRY_MULTI_POINT "MultiPoint"
// 多线
#define GEOJSON_GEOMETRY_MULTI_LINE_STRING "MultiLineString"
// 多面
#define GEOJSON_GEOMETRY_MULTI_POLYGON "MultiPolygon"

static std::string suGeoType2Str(const eGeometryType& type)
{
    switch (type)
    {
        case eGeometryType::Point:
            return GEOJSON_GEOMETRY_POINT;
        case eGeometryType::LineString:
            return GEOJSON_GEOMETRY_LINE_STRING;
        case eGeometryType::Polygon:
            return GEOJSON_GEOMETRY_POLYGON;
        case eGeometryType::MultiPoint:
            return GEOJSON_GEOMETRY_MULTI_POINT;
        case eGeometryType::MultiLineString:
            return GEOJSON_GEOMETRY_MULTI_LINE_STRING;
        default:
            break;
    }
    return "";
}

static eGeometryType suGeoStr2Type(const std::string& str)
{
    if (GEOJSON_GEOMETRY_POINT == str)
    {
        return eGeometryType::Point;
    }
    if (GEOJSON_GEOMETRY_LINE_STRING == str)
    {
        return eGeometryType::LineString;
    }
    if (GEOJSON_GEOMETRY_POLYGON == str)
    {
        return eGeometryType::Polygon;
    }
    if (GEOJSON_GEOMETRY_MULTI_POINT == str)
    {
        return eGeometryType::MultiPoint;
    }
    if (GEOJSON_GEOMETRY_MULTI_LINE_STRING == str)
    {
        return eGeometryType::MultiLineString;
    }
    if (GEOJSON_GEOMETRY_MULTI_POLYGON == str)
    {
        return eGeometryType::MultiPolygon;
    }
    return eGeometryType::Invalid;
}

#endif  // SILLY_GEO_H
