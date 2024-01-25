/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/19 15:17
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_H
#define SILLY_UTILS_SILLY_GEO_H
#include <vector>
#include <iostream>
#include <map>
#include <cstring>

// 小于这个值就认为是同一个值,经纬度小数后8位约1cm精确度
#define SILLY_GEO_FLOAT_IGNORE_DIFF 1e-8

// geometry types
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
// 复合类型
#define GEOJSON_GEOMETRY_COLLECTION "GeometryCollection"

// 参照GDAL

enum class enum_geometry_type
{
    egtInvalid = 0,          // 无效
    egtPoint = 1,            // 单点
    egtLineString = 2,       // 单线
    egtPolygon = 3,          // 单面
    egtMultiPoint = 4,       // 多点
    egtMultiLineString = 5,  // 多线
    egtMultiPolygon = 6,     // 多面
    egtCompositeType = 7,    // 复合数据类型

};

/// 点
struct silly_point  // 普通坐标点
{
    silly_point() = default;

    silly_point(double x, double y)
    {
        lgtd = x;
        lttd = y;
    }

    double lgtd{0.};
    double lttd{0.};

    silly_point& operator=(const silly_point& point)
    {
        lgtd = point.lgtd;
        lttd = point.lttd;
        return *this;
    }

    bool operator==(const silly_point& point) const
    {
        return std::abs(point.lgtd - this->lgtd) <= SILLY_GEO_FLOAT_IGNORE_DIFF && std::abs(point.lttd - this->lttd) <= SILLY_GEO_FLOAT_IGNORE_DIFF;
    }

    bool operator!=(const silly_point& point) const
    {
        return std::abs(point.lgtd - this->lgtd) > SILLY_GEO_FLOAT_IGNORE_DIFF || std::abs(point.lttd - this->lttd) > SILLY_GEO_FLOAT_IGNORE_DIFF;
    }
};

struct silly_geo_rect  // 普通坐标点
{
    silly_geo_rect() = default;

    silly_geo_rect(double l, double t, double r, double b)
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    void correct()
    {
        if (left > right)
        {
            double tmp = left;
            left = right;
            right = tmp;
        }
        if (bottom > top)
        {
            double tmp = bottom;
            bottom = top;
            top = tmp;
        }
    }

    double left{0};
    double top{0};
    double right{0};
    double bottom{0};

    silly_geo_rect& operator=(const silly_geo_rect& rect) = default;

    bool operator==(const silly_geo_rect& rect) const
    {
        return std::abs(rect.left - this->left) <= SILLY_GEO_FLOAT_IGNORE_DIFF && std::abs(rect.top - this->top) <= SILLY_GEO_FLOAT_IGNORE_DIFF && std::abs(rect.right - this->right) <= SILLY_GEO_FLOAT_IGNORE_DIFF &&
               std::abs(rect.bottom - this->bottom) <= SILLY_GEO_FLOAT_IGNORE_DIFF;
    }

    bool operator!=(const silly_geo_rect& rect) const
    {
        return std::abs(rect.left - this->left) > SILLY_GEO_FLOAT_IGNORE_DIFF || std::abs(rect.top - this->top) > SILLY_GEO_FLOAT_IGNORE_DIFF || std::abs(rect.right - this->right) > SILLY_GEO_FLOAT_IGNORE_DIFF ||
               std::abs(rect.bottom - this->bottom) > SILLY_GEO_FLOAT_IGNORE_DIFF;
    }

    bool intersect(const silly_geo_rect& rect)
    {
        int zx = std::abs(rect.left + rect.right - left - right);
        int x = std::abs(rect.left - rect.right) + std::abs(left - right);
        int zy = std::abs(rect.top + rect.bottom - top - bottom);
        int y = std::abs(rect.top - rect.bottom) + std::abs(top - bottom);
        if (zx <= x && zy <= y)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

typedef std::vector<silly_point> silly_multi_point;

/// 线
typedef std::vector<silly_point> silly_line;

typedef std::vector<silly_line> silly_multi_silly_line;

/// 环
struct silly_ring
{
    std::vector<silly_point> points;
    int is_outer{1};
};

struct silly_poly  // 普通面
{
    silly_ring outer_ring;                // 外环
    std::vector<silly_ring> inner_rings;  // 内环
};

typedef std::vector<silly_poly> silly_multi_poly;

#endif  // SILLY_UTILS_SILLY_GEO_H
