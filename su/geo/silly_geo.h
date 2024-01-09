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

#define SILLY_GEO_FLOAT_IGNORE_DIFF 1e-9

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

enum enum_geometry_types
{
    eInvalid = 0,          // 无效
    ePoint = 1,            // 单点
    eLineString = 2,       // 单线
    ePolygon = 3,          // 单面
    eMultiPoint = 4,       // 多点
    eMultiLineString = 5,  // 多线
    eMultiPolygon = 6,     // 多面
    eCompositeType = 7,    // 复合数据类型

};

enum enum_geoprop_types
{
    eNone = 0,      // 无效
    eInt = 1,       // 整形
    eNumeric = 2,   // 浮点数
    eString = 3,    // 字符串
    eBinary = 4,    // 二进制流
    eTime = 5,      // 时间
    eDate = 6,      // 日期
    eDateTime = 7,  // 日期加时间
    eLong = 8       // 长整形
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

    double lgtd{0};
    double lttd{0};

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

class silly_geo_prop
{
  public:
    enum_geoprop_types type{eNone};
    size_t len{0};
    unsigned char* data{nullptr};

    // 运算符重载
    silly_geo_prop& operator=(const silly_geo_prop& other)
    {
        if (this != &other)  // 避免自我赋值
        {
            this->type = other.type;
            this->len = other.len;
            // 释放原有的 data
            if (this->data != nullptr)
            {
                delete[] this->data;
            }
            // 分配新的内存并复制数据
            this->data = new unsigned char[len];
            memcpy(this->data, other.data, len);
        }
        return *this;
    }

    // 拷贝函数
    silly_geo_prop(const silly_geo_prop& other)
    {
        this->type = other.type;
        this->len = other.len;
        // 释放原有的 data
        if (this->data != nullptr)
        {
            delete[] this->data;
        }
        this->data = new unsigned char[other.len];
        memcpy(this->data, other.data, other.len);
    }

    ~silly_geo_prop()
    {
        if (data != nullptr)
        {
            // 释放 data
            delete[] data;
        }
    }
    // 构造函数
    silly_geo_prop()
    {
    }
};

class silly_geo_coll
{
  public:
    // 类型
    enum_geometry_types m_type{enum_geometry_types::eInvalid};
    // 内容
    silly_point m_point;                         // 单点
    silly_multi_point m_m_points;                // 多点
    silly_line m_line;                           // 单线
    silly_multi_silly_line m_m_lines;            // 多线
    silly_poly m_poly;                           // 单面(内环+外环)
    silly_multi_poly m_m_polys;                  // 多面(多个 单面)
    std::vector<enum_geometry_types> comp_type;  //用于存储复合数据类型变量中的类型变量
    // 属性列表
    std::map<std::string, silly_geo_prop> m_props;

    silly_geo_coll& operator=(const silly_geo_coll& other)
    {
        this->m_type = other.m_type;
        this->m_point = other.m_point;
        this->m_m_points = other.m_m_points;
        this->m_line = other.m_line;
        this->m_m_lines = other.m_m_lines;
        this->m_poly = other.m_poly;
        this->m_m_polys = other.m_m_polys;
        this->comp_type = other.comp_type;
        this->m_props.clear();  // 清空当前的 m_props
        for (const auto& [key, prop] : other.m_props)
        {
            this->m_props[key] = prop;
        }
        return *this;
    }

    // 构造
    silly_geo_coll()
    {
    }

    // 拷贝
    silly_geo_coll(const silly_geo_coll& other)
    {
        this->m_type = other.m_type;
        this->m_point = other.m_point;
        this->m_m_points = other.m_m_points;
        this->m_line = other.m_line;
        this->m_m_lines = other.m_m_lines;
        this->m_poly = other.m_poly;
        this->m_m_polys = other.m_m_polys;
        this->comp_type = other.comp_type;
        // 拷贝 m_props，需要依次拷贝里面的元素
        for (const auto& entry : other.m_props)
        {
            this->m_props[entry.first] = entry.second;
        }
    }
};

/// 工具类
class silly_geo
{
  public:
    static enum_geometry_types check_geojson_type(const std::string& content);
    static std::string dump_geojson(const std::vector<silly_point> points);
    static std::string dump_geojson(const std::vector<silly_line> lines);
    static std::string dump_geojson(const std::vector<silly_poly> polys);
    static std::vector<silly_poly> load_geojson(const std::string& geojson);

    /// <summary>
    /// 将silly_geo_coll回写为geojson中的表示方式
    /// </summary>
    /// <param name="sgc"></param>
    /// <returns></returns>
    static std::string dump_geo_coll(const silly_geo_coll& geo_coll);

    /// <summary>
    /// 从字符串加载silly_geo_coll
    /// </summary>
    /// <param name="content"></param>
    /// <returns></returns>
    static silly_geo_coll load_geo_coll(const std::string& content);
};

#endif  // SILLY_UTILS_SILLY_GEO_H
