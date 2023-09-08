#pragma once


#ifndef SILLY_UTILS_SILLY_GEO_OPERATION_H
#define SILLY_UTILS_SILLY_GEO_OPERATION_H

#include "silly_geo.h"
#define UM_PI 3.14159265358979323846

class geo_operation
{
public:

	// ��һ���������(��������)
	static silly_point ring_to_center(silly_ring ring);

	/// <summary>
	/// ��������ķ�λ��,p2�����p1�ķ�λ��(���Ͻ����½�����ϵ����)
	/// </summary>
	/// <param name="p1">������</param>
	/// <param name="p2">���շ���</param>
	/// <returns>p2�����p1�ķ�λ��,���Ϊ�Ƕ�ֵ,</returns>
	static double two_point_azimuth(silly_point p1, silly_point p2);

	// shpתgeojson
	static bool shp_to_geojson(const char* shpFile, const char* geojsonFile);
	// gesjsonתshp
	static bool geojson_to_shp(const char* geojsonFile, const char* shpFile);

	geo_operation() = default;
	~geo_operation() = default;

private:

};




#endif // SILLY_UTILS_SILLY_GEO_OPERATION_H