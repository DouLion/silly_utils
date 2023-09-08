#pragma once


#ifndef SILLY_UTILS_SILLY_GEO_OPERATION_H
#define SILLY_UTILS_SILLY_GEO_OPERATION_H

#include "silly_geo.h"
#define UM_PI 3.14159265358979323846

class geo_operation
{
public:

	// 求一个面的形心(几何中心)
	static silly_point ring_to_center(silly_ring ring);

	/// <summary>
	/// 求两个点的方位角,p2相对于p1的方位角(左上角右下角坐标系均可)
	/// </summary>
	/// <param name="p1">参照物</param>
	/// <param name="p2">参照方向</param>
	/// <returns>p2相对于p1的方位角,结果为角度值,</returns>
	static double two_point_azimuth(silly_point p1, silly_point p2);

	// shp转geojson
	static bool shp_to_geojson(const char* shpFile, const char* geojsonFile);
	// gesjson转shp
	static bool geojson_to_shp(const char* geojsonFile, const char* shpFile);

	geo_operation() = default;
	~geo_operation() = default;

private:

};




#endif // SILLY_UTILS_SILLY_GEO_OPERATION_H