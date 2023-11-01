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

#define FLOAT_IGNORE_DIFF   0.000000000000001

//geometry types
// 点
#define GEOJSON_GEOMETRY_POINT				"Point"
// 线
#define GEOJSON_GEOMETRY_LINE_STRING		"LineString"
// 面
#define GEOJSON_GEOMETRY_POLYGON			"Polygon"
// 多点
#define GEOJSON_GEOMETRY_MULTI_POINT		"MultiPoint"
// 多线
#define GEOJSON_GEOMETRY_MULTI_LINE_STRING	"MultiLineString"
// 多面
#define GEOJSON_GEOMETRY_MULTI_POLYGON		"MultiPolygon"

// 参照GDAL

enum enum_geometry_types{
	eInvalid = 0,			// 无效
	ePoint = 1,				// 单点
	eLineString = 2,		// 单线
	ePolygon = 3,			// 单面
	eMultiPoint = 4,		// 多点
	eMultiLineString = 5,	// 多线
	eMultiPolygon = 6 ,		// 多面
	eCompositeType = 7,		// 复合数据类型  

};

enum enum_geoprop_types {
	eNone = 0,				// 无效
	eInt = 1,				// 整形
	eNumeric = 2,			// 浮点数
	eString = 3,			// 字符串
	eBinary = 4,			// 二进制流
	eTime = 5,				// 时间
	eDate = 6  ,			// 日期
	eDateTime = 7,			// 日期加时间
	eLong = 8				// 长整形
};

/// 点
struct silly_point    // 普通坐标点
{
	silly_point() = default;

	silly_point(double x, double y)
	{
		lgtd = x;
		lttd = y;
	}

	double lgtd{ 0 };
	double lttd{ 0 };

	silly_point& operator=(const silly_point& point)
	{
		lgtd = point.lgtd;
		lttd = point.lttd;
		return *this;
	}

	bool operator==(const silly_point& point) const
	{
		return std::abs(point.lgtd - this->lgtd) <= FLOAT_IGNORE_DIFF && std::abs(point.lttd - this->lttd) <= FLOAT_IGNORE_DIFF;
	}

	bool operator!=(const silly_point& point) const
	{
		return std::abs(point.lgtd - this->lgtd) > FLOAT_IGNORE_DIFF || std::abs(point.lttd - this->lttd) > FLOAT_IGNORE_DIFF;
	}
};

struct silly_geo_rect    // 普通坐标点
{
	silly_geo_rect() = default;

	silly_geo_rect(double l, double t, double r, double b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	double left{ 0 };
	double top{ 0 };
	double right{ 0 };
	double bottom{ 0 };

	silly_geo_rect& operator=(const silly_geo_rect& rect) = default;

	bool operator==(const silly_geo_rect& rect) const
	{
		return std::abs(rect.left - this->left) <= FLOAT_IGNORE_DIFF && std::abs(rect.top - this->top) <= FLOAT_IGNORE_DIFF && std::abs(rect.right - this->right) <= FLOAT_IGNORE_DIFF && std::abs(rect.bottom - this->bottom) <= FLOAT_IGNORE_DIFF;
	}

	bool operator!=(const silly_geo_rect& rect) const
	{
		return std::abs(rect.left - this->left) > FLOAT_IGNORE_DIFF || std::abs(rect.top - this->top) > FLOAT_IGNORE_DIFF || std::abs(rect.right - this->right) > FLOAT_IGNORE_DIFF || std::abs(rect.bottom - this->bottom) > FLOAT_IGNORE_DIFF;
	}
};

typedef std::vector<silly_point> silly_multi_point;

/// 线

typedef std::vector<silly_point> silly_line;

typedef std::vector<silly_line>  silly_multi_silly_line;

/// 面
struct silly_ring
{
	std::vector<silly_point> points;
	int is_outer{ 1 };
};

struct silly_poly	// 普通面
{
	silly_ring outer_ring;		// 外环
	std::vector<silly_ring> inner_rings; // 内环
};

typedef  std::vector<silly_poly> silly_multi_poly ;


/// 工具类
class silly_geo
{
public:
	static enum_geometry_types check_geojson_type(const std::string& content);
	static std::string dump_geojson(const std::vector<silly_point> points);
	static std::string dump_geojson(const std::vector<silly_line> lines);
	static std::string dump_geojson(const std::vector<silly_poly> polys);
	static std::vector<silly_poly> load_geojson(const std::string& geojson);

};

#endif //SILLY_UTILS_SILLY_GEO_H
