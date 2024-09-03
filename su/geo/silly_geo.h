/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/19 15:17
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 平面经纬度矢量基本类型定义
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_H
#define SILLY_UTILS_SILLY_GEO_H
#include <su_marco.h>

// 小于这个值就认为是同一个值,经纬度小数后8位约1cm精确度

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
        return std::abs(point.lgtd - this->lgtd) <= SU_EPSILON && std::abs(point.lttd - this->lttd) <= SU_EPSILON;
    }

    inline bool operator<(const silly_point& point) const
    {
        return point.lttd < lttd || (point.lttd == lttd && point.lgtd < lgtd);
    }

    bool operator!=(const silly_point& point) const
    {
        return std::abs(point.lgtd - this->lgtd) > SU_EPSILON || std::abs(point.lttd - this->lttd) > SU_EPSILON;
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
        return std::abs(rect.left - this->left) <= SU_EPSILON && std::abs(rect.top - this->top) <= SU_EPSILON && std::abs(rect.right - this->right) <= SU_EPSILON &&
               std::abs(rect.bottom - this->bottom) <= SU_EPSILON;
    }

    bool operator!=(const silly_geo_rect& rect) const
    {
        return std::abs(rect.left - this->left) > SU_EPSILON || std::abs(rect.top - this->top) > SU_EPSILON || std::abs(rect.right - this->right) > SU_EPSILON ||
               std::abs(rect.bottom - this->bottom) > SU_EPSILON;
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

class silly_multi_point : public std::vector<silly_point>
{
  public:
    silly_multi_point() = default;
    silly_multi_point(std::vector<silly_point> points)
    {
        for (auto& p : points)
            push_back(p);
    }
};

/// 线
class silly_line : public std::vector<silly_point>
{
  public:
    silly_line() = default;
    silly_line(std::vector<silly_point> points)
    {
        for (auto& p : points)
            push_back(p);
    }
};

class silly_multi_silly_line : public std::vector<silly_line>
{
  public:
};

/// 环
class silly_ring
{
  public:
    std::vector<silly_point> points;
    int is_outer{1};
};

class silly_poly  // 普通面
{
  public:
    silly_ring outer_ring;                // 外环
    std::vector<silly_ring> inner_rings;  // 内环
};

class silly_multi_poly : public std::vector<silly_poly>
{
  public:
};

#endif  // SILLY_UTILS_SILLY_GEO_H
