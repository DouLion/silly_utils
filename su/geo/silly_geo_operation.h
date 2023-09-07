#pragma once


#ifndef SILLY_UTILS_SILLY_GEO_CONVERT_H
#define SILLY_UTILS_SILLY_GEO_CONVERT_H

#include "silly_geo.h"
#define UM_PI 3.14159265358979323846

class geo_operation
{
public:

	// 求一个面的形心(几何中心)
	static silly_point ring_to_center(silly_ring ring);

	// 求两个点的方位角
	static double two_point_azimuth(silly_point p1, silly_point p2);

	// shp转geojson
	static bool shp_to_geojson(const char* shpFile, const char* geojsonFile);
	// gesjson转shp
	static bool geojson_to_shp(const char* geojsonFile, const char* shpFile);

	geo_operation() = default;
	~geo_operation() = default;

private:

};




#endif // SILLY_UTILS_SILLY_GEO_CONVERT_H