#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_OPERATION_H
#define SILLY_UTILS_SILLY_GEO_OPERATION_H

#include "silly_geo.h"


class geo_utils
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
	/// 读取geojson和shp中的点数据,并存储多个点到std::vector<silly_point>
	/// 可以处理 wkbPolygon和wkbPolygon25D类型两种矢量
	/// </summary>
	/// <param name="File">文件地址</param>
	/// <returns></returns>
	static std::vector<silly_point> read_vector_points(const char* File);

	/// <summary>
	/// 读取geojson和shp中的线数据,并存储多个点到std::vector<silly_line>
	/// 可以处理wkbLineString  wkbLineString25D  wkbMultiLineString  wkbMultiLineString25D 四种矢量
	/// </summary>
	/// <param name="File"></param>
	/// <returns></returns>
	static std::vector<silly_line> read_vector_lines(const char* File);

	/// <summary>
	/// 读取geojson和shp中的面,并存储为一个多面
	/// </summary>
	/// <param name="geojsonFile">读取文件地址</param>
	static std::vector<silly_multi_poly> geo_utils::read_vector_polys(const char* File);

	/// <summary>
	/// 判断两个面是否相交,如果相交求出相交区域,如果不相交,返回一个空面的数组
	/// 两个面的相交可能有多个连续的区域,每个区域要单独存储,返回多个面的数组
	/// </summary>
	/// <param name="ring_1">面1</param>
	/// <param name="ring_2">面2</param>
	static std::vector<silly_ring>  intersect_area(silly_ring ring_1, silly_ring ring_2);

	/// <summary>
	/// 初始化 GDAL 环境,只需要初始化一次
	/// </summary>
	static void init_gdal_env();


	/// <summary>
	/// 销毁 GDAL 环境, 一旦执行此方法,那么GDAL可能就无法使用,
	/// </summary>
	static void destory_gdal_env();

	/// <summary>
	/// 是否为一个标准的shp文件
	/// </summary>
	/// <param name="shp_file"></param>
	/// <returns></returns>
	static bool is_valid_shp(const std::string& shp_file);


	/// <summary>
	/// 加载shp文件中的属性信息
	/// </summary>
	/// <param name="shp_file"></param>
	/// <param name="type"></param>
	/// <param name="properties"></param>
	/// <returns></returns>
	static bool check_shp_info(const std::string& shp_file, enum_geometry_types& type, std::map<std::string, std::string>& properties);


};


#endif // SILLY_UTILS_SILLY_GEO_OPERATION_H