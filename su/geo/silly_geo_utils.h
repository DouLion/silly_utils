#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_OPERATION_H
#define SILLY_UTILS_SILLY_GEO_OPERATION_H

#include "silly_geo.h"
#include "gdal_priv.h"

// 矢量文件后缀名
#define SILLY_SHP_SUFFIX			".shp"
#define SILLY_TAB_SUFFIX			".tab"
#define SILLY_GEOJSON_SUFFIX		".geojson"
#define SILLY_SQLITE_SUFFIX			".sqlite"
#define SILLY_CSV_SUFFIX			".csv"
#define SILLY_KML_SUFFIX			".kml"
#define SILLY_GML_SUFFIX			".gml"
#define SILLY_XLSX_SUFFIX			".xlsx"

// 矢量文件存储格式
#define SILLY_SHP_DRIVER_NAME			"ESRI Shapefile"
#define SILLY_TAB_DRIVER_NAME			"Mapinfo File"
#define SILLY_GEOJSON_DRIVER_NAME		"GeoJSON"
#define SILLY_SQLITE_DRIVER_NAME		"SQLite"
#define SILLY_CSV_DRIVER_NAME			"CSV"
#define SILLY_KML_DRIVER_NAME			"KML"
#define SILLY_GML_DRIVER_NAME			"GML"
#define SILLY_XLSX_DRIVER_NAME			"XLSX"


class geo_utils
{
public:

	// 求一个面的形心(几何中心)
	static silly_point ring_to_center(silly_ring ring);

	/// <summary>
	/// 求两个点的方位角,p2相对于p1的方位角(左上角右下角坐标系均可), 正北方向为0度,顺时针
	/// </summary>
	/// <param name="p1">参照物</param>
	/// <param name="p2">参照方向</param>
	/// <returns>p2相对于p1的方位角,结果为角度值,</returns>
	static double two_point_azimuth(silly_point from, silly_point to);

	/// <summary>
	/// 读取矢量文件中的数据存储到silly_geo_coll数据结构中  
	/// </summary>
	/// <param name="file"></param>
	/// <param name="collection"></param>
	/// <returns></returns>
	/// 注:读取 shp , geojson 类型文件中可以实现
	static bool read_geo_coll(const char* file, std::vector<silly_geo_coll>& collections);

	/// <summary>
	/// 将silly_geo_coll数据结构写入矢量文件(如shp文件)
	/// </summary>
	/// <param name="file"></param>
	/// <param name="collection"></param>
	/// <returns></returns>
	/// 注:写入 shp , geojson 类型文件中经测试可以实现
	static bool write_geo_coll(const char* file, const std::vector<silly_geo_coll>& collections);


	/// <summary>
	/// 判断两个面是否相交,如果相交求出相交区域,如果不相交,返回一个空面的数组
	/// 两个面的相交可能有多个连续的区域,每个区域要单独存储,返回多个面的数组
	/// </summary>
	/// <param name="ring_1">面1</param>
	/// <param name="ring_2">面2</param>
	static std::vector<silly_ring> intersect_area(silly_ring ring_1, silly_ring ring_2);

	/// <summary>
	/// 初始化 GDAL 环境,只需要初始化一次
	/// </summary>
	static void init_gdal_env();


	/// <summary>
	/// 销毁 GDAL 环境, 一旦执行此方法,那么GDAL可能就无法使用,
	/// </summary>
	static void destroy_gdal_env();

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
	static bool check_shp_info(const std::string& shp_file, enum_geometry_types& type, std::map<std::string, enum_geoprop_types>& properties);

	static bool check_shp_info(const std::string& shp_file, enum_geometry_types& type, std::map<std::string, std::string>& properties);

	/// <summary>
	/// 根据文件类型得到对应的 gdal 中的存储格式
	/// 支持的文件格式对应的存储类型: shp tab geojson sqlite csv kml gml xlsx
	/// </summary>
	/// <param name="file">文件名</param>
	/// <param name="driverName">存储类型</param>
	/// <returns></returns>
	static bool get_driver_name(const char* file, std::string& driverName);


	// ================ gdal中矢量与silly utils中矢量互转 ================

	// ================ 单环 ================
	// 将 silly_ring 转换为 OGRPolygon
	static OGRPolygon* SillyRingToPolygon(const silly_ring& ring);
	// 环OGRLinearRing对象，将其转换为silly_ring对象  (环)
	static silly_ring ORGRingToSillyRing(OGRLinearRing* ring);

	// ================ 单点 ================
	// 将 OGRPoint(单点) 转换为 silly_point(单点) 类型
	static silly_point OGRPointToSillyPoint(OGRPoint* ogrPoint);
	// 将 silly_point(单点) 转换为 OGRPoint(单点) 类型
	static OGRPoint* SillyPointToOGRPoint(const silly_point& point);


	// ================ 多点 ================
	// 将 OGRMultiPoint(多点) 转换为 silly_multi_point(多点) 类型
	static silly_multi_point OGRMulPointToSillyMulPoint(OGRMultiPoint* ogrMultiPoint);
	// 将 silly_multi_point(多点) 转换为 OGRMultiPoint(多点) 类型
	static OGRMultiPoint* SillyMulPointToOGRMulPoint(const silly_multi_point& mulitPoint);

	// ================ 单线 ================
	// OGRLineString(线)类型转为silly_line(线)类型
	static silly_line OGRLineToSillyLine(OGRLineString* lineString);
	// 将 silly_line(线) 转换为 OGRLineString(线)类型
	static OGRLineString* SillyLineToOGRLine(const silly_line& line);

	// ================ 多线 ================
	// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
	static silly_multi_silly_line OGRMulLineToSillyMulLine(OGRMultiLineString* multiLineString);
	// 将 silly_multiline(多线) 转换为 OGRMultiLineString(多线)类型
	static OGRMultiLineString* SillyMulLineToOGRMulLine(const silly_multi_silly_line& multiLine);

	// ================ 单面 ================
	// OGRPolygon 对象转换为 silly_poly (多环:外环+内环)对象  (单面)
	static silly_poly OGRPolyToSillyPoly(OGRPolygon* polygon);
	// 将 silly_poly 转换为 OGRPolygon(单面)
	static OGRPolygon* SillyPolyToOGRPoly(const silly_poly& poly);

	// ================ 多面 ================
	// 多面的OGRMultiPolygon对象转换为silly_multi_poly(多面)
	static silly_multi_poly ORGMulPolyToSillyMulPoly(OGRMultiPolygon* multiPolygon);
	// 将silly_multi_poly对象转换为OGRMultiPolygon对象(多面)
	static OGRMultiPolygon* SillyMulPolyToORGMulPoly(const silly_multi_poly& multiPoly);



};




#endif // SILLY_UTILS_SILLY_GEO_OPERATION_H