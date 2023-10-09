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


	/// <summary>
	/// 读取geojson中的矢量环,并返回多个环的数据(该函数有问题)
	/// </summary>
	/// <param name="geojsonFile">读取geojson文件地址</param>
	static std::vector<silly_ring> read_geojson(const char* geojsonFile);

	/// <summary>
	/// 读取shp中的矢量环,并返回多个环的数据
	/// </summary>
	/// <param name="shp">读取SHP文件地址</param>
	static std::vector<silly_ring> read_shp_ring(const char* shp);

	/// <summary>
	/// 读取一个shp文件,将多个坐标点绘制在读取的shp文件中的位置,并生成一个新的shp文件
	/// </summary>
	/// <param name="points">需要绘制的坐标点</param>
	/// <param name="shpFilePath">读取SHP文件地址</param>
	/// <param name="outputShpFilePath">写入SHP文件地址</param>
	static bool points_to_shp(std::vector<silly_point>& points, const char* shpFilePath, const char* outputShpFilePath);

	/// <summary>
	/// 判断两个面是否相交,如果相交求出相交区域,如果不相交,返回一个空面的数组
	/// 两个面的相交可能有多个连续的区域,每个区域要单独存储,返回多个面的数组
	/// </summary>
	/// <param name="ring_1">面1</param>
	/// <param name="ring_2">面2</param>
	static std::vector<silly_ring>  intersect_area(silly_ring ring_1, silly_ring ring_2);

};


#endif // SILLY_UTILS_SILLY_GEO_OPERATION_H