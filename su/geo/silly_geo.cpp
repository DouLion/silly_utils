//
// Created by dly on 2023/7/19.
//

#include "silly_geo.h"
#include <json/json.h>

#define GEOJSON_KEY_FEATURES  "features"
#define GEOJSON_KEY_GEOMETRY  "geometry"
#define GEOJSON_KEY_TYPE  "type"
#define GEOJSON_KEY_COORDINATES  "coordinates"

// --------
#define GEOJSON_VAL_FEATURE_COLLECTION "FeatureCollection"
#define GEOJSON_VAL_FEATURE  "Feature"

enum_geometry_types silly_geo::check_geojson_type(const std::string& content)
{
	enum_geometry_types ret_type = eInvalid;
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(content, root))
	{
		if (root.isMember(GEOJSON_KEY_FEATURES))
		{
			for (auto& f: root[GEOJSON_KEY_FEATURES])
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
	for (auto& poly: polys)
	{
		Json::Value o = Json::objectValue;
		o[GEOJSON_KEY_TYPE] = GEOJSON_VAL_FEATURE;
		o[GEOJSON_KEY_GEOMETRY] = Json::objectValue;
		o[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_TYPE] = GEOJSON_GEOMETRY_POLYGON;
		o[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_COORDINATES] = Json::arrayValue;

		// Json::Value coord_arr = Json::arrayValue;
		Json::Value outer_arr = Json::arrayValue;
		for (auto point: poly.outer_ring.points)
		{
			Json::Value point_arr = Json::arrayValue;
			point_arr.append(point.lgtd);
			point_arr.append(point.lttd);
			outer_arr.append(point_arr);

		}
		o[GEOJSON_KEY_GEOMETRY][GEOJSON_KEY_COORDINATES].append(outer_arr);

		for (auto& ring: poly.inner_rings)
		{
			Json::Value ring_arr = Json::arrayValue;
			for (auto point: ring.points)
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

