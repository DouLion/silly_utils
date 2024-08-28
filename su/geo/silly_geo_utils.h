#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_OPERATION_H
#define SILLY_UTILS_SILLY_GEO_OPERATION_H

#include <geo/silly_geo_coll.h>
#include "gdal_priv.h"

// 矢量文件后缀名
#define SILLY_SHP_SUFFIX ".shp"
#define SILLY_TAB_SUFFIX ".tab"
#define SILLY_GEOJSON_SUFFIX ".geojson"
#define SILLY_SQLITE_SUFFIX ".sqlite"
#define SILLY_CSV_SUFFIX ".csv"
#define SILLY_KML_SUFFIX ".kml"
#define SILLY_GML_SUFFIX ".gml"
#define SILLY_XLSX_SUFFIX ".xlsx"

// 矢量文件存储格式
#define SILLY_SHP_DRIVER_NAME "ESRI Shapefile"
#define SILLY_TAB_DRIVER_NAME "Mapinfo File"
#define SILLY_GEOJSON_DRIVER_NAME "GeoJSON"
#define SILLY_SQLITE_DRIVER_NAME "SQLite"
#define SILLY_CSV_DRIVER_NAME "CSV"
#define SILLY_KML_DRIVER_NAME "KML"
#define SILLY_GML_DRIVER_NAME "GML"
#define SILLY_XLSX_DRIVER_NAME "XLSX"

class silly_geo_utils
{
  public:
    /// <summary>
    /// 初始化 GDAL 环境,只需要初始化一次
    /// </summary>
    static void init_gdal_env();

    /// <summary>
    /// 销毁 GDAL 环境, 一旦执行此方法,那么GDAL可能就无法使用,
    /// </summary>
    static void destroy_gdal_env();

    /// <summary>
    /// 求一个面的形心(几何中心),利用ogr库算法
    /// </summary>
    /// <param name="poly"></param>
    /// <returns></returns>
    static silly_point poly_centroid(silly_poly poly);

    /// <summary>
    /// 判断两个面是否相交,如果相交求出相交区域,如果不相交,返回一个空变量
    /// 利用ogr库算法
    /// 可以两个面相交得到多面的情况
    /// </summary>
    /// <param name="poly"></param>
    /// <returns></returns>
    static std::vector<silly_poly> intersection_area(silly_poly poly_main, silly_poly poly_deputy);

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
    static bool read_geo_coll(const std::string& file, std::vector<silly_geo_coll>& collections, const bool& ignore_prop = false);

    /// <summary>
    /// 将silly_geo_coll数据结构写入矢量文件(如shp文件)
    /// </summary>
    /// <param name="file"></param>
    /// <param name="collection"></param>
    /// <returns></returns>
    /// 注:写入 shp , geojson 类型文件中经测试可以实现
    static bool write_geo_coll(const std::string& file, const std::vector<silly_geo_coll>& collections);

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
    static bool check_shp_info(const std::string& shp_file, enum_geometry_type& type, std::map<std::string, silly_geo_prop::enum_prop_type>& properties);

    /// <summary>
    /// 根据文件类型得到对应的 gdal 中的存储格式
    /// 支持的文件格式对应的存储类型: shp tab geojson sqlite csv kml gml xlsx
    /// </summary>
    /// <param name="file">文件名</param>
    /// <param name="driverName">存储类型</param>
    /// <returns></returns>
    static bool get_driver_name(const char* file, std::string& driverName);

    /// <summary>
    /// 矢量与面是否相交
    /// </summary>
    /// <param name=""></param>
    /// <param name=""></param>
    /// <returns></returns>
    static bool intersect(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2);

    static bool intersect(const silly_multi_poly& mpoly, const silly_point& point);

    static bool intersect(const silly_multi_poly& mpoly, const silly_line& line);

    /// <summary>
    /// 点距离线在一定范围内
    /// </summary>
    /// <param name="point"></param>
    /// <param name="line"></param>
    /// <param name="dist"></param>
    /// <returns></returns>
    static bool nearby(const silly_point& point, const silly_line& line, const double& dist);

    /// <summary>
    /// 两个面的相交区域
    /// </summary>
    /// <param name="mpoly1"></param>
    /// <param name="mpoly2"></param>
    /// <returns></returns>
    static std::vector<silly_poly> intersection(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2);

    /// <summary>
    /// 线相交面的部分
    /// </summary>
    /// <param name="mpoly1"></param>
    /// <param name="line"></param>
    /// <returns></returns>
    static std::vector<silly_line> intersection(const silly_multi_poly& mpoly, const silly_line& line);


    /// ================ gdal中矢量与silly utils中矢量互转 ================


    /// ================ 单点 ================

    /// <summary>
    /// 将 OGRPoint(单点) 转换为 silly_point(单点) 类型
    /// </summary>
    /// <param name="ogrPoint"></param>
    /// <returns></returns>
    static silly_point silly_point_from_ogr(const OGRPoint* ogrPoint);

    /// <summary>
    /// 将 silly_point(单点) 转换为 OGRPoint(单点) 类型
    /// </summary>
    /// <param name="point"></param>
    /// <returns></returns>
    static OGRPoint silly_point_to_ogr(const silly_point& point);

    /// ================ 多点 ================

    /// <summary>
    /// 将 OGRMultiPoint(多点) 转换为 silly_multi_point(多点) 类型
    /// </summary>
    /// <param name="ogrMultiPoint"></param>
    /// <returns></returns>
    static silly_multi_point silly_multi_point_from_ogr(const OGRMultiPoint* ogrMultiPoint);

    /// <summary>
    /// 将 silly_multi_point(多点) 转换为 OGRMultiPoint(多点) 类型
    /// </summary>
    /// <param name="mulitPoint"></param>
    /// <returns></returns>
    static OGRMultiPoint silly_multi_point_to_ogr(const silly_multi_point& mulitPoint);

    /// ================ 单线 ================

    /// <summary>
    /// OGRLineString(线)类型转为silly_line(线)类型
    /// </summary>
    /// <param name="lineString"></param>
    /// <returns></returns>
    static silly_line silly_line_from_ogr(const OGRLineString* lineString);

    /// <summary>
    /// 将 silly_line(线) 转换为 OGRLineString(线)类型
    /// </summary>
    /// <param name="line"></param>
    /// <returns></returns>
    static OGRLineString silly_line_to_ogr(const silly_line& line);

    /// ================ 多线 ================

    /// <summary>
    /// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
    /// </summary>
    /// <param name="multiLineString"></param>
    /// <returns></returns>
    static silly_multi_silly_line silly_multi_line_from_ogr(const OGRMultiLineString* multiLineString);

    /// <summary>
    /// 将 silly_multiline(多线) 转换为 OGRMultiLineString(多线)类型
    /// </summary>
    /// <param name="multiLine"></param>
    /// <returns></returns>
    static OGRMultiLineString silly_multi_line_to_ogr(const silly_multi_silly_line& multiLine);

    /// ================ 闭合环 ================

    /// <summary>
    /// 环OGRLinearRing对象，将其转换为silly_ring对象  (环)
    /// </summary>
    /// <param name="ring"></param>
    /// <returns></returns>
    static silly_ring silly_ring_from_ogr(const OGRLinearRing* ring);

    /// <summary>
    /// 将 silly_ring 转换为 OGRPolygon
    /// </summary>
    /// <param name="ring"></param>
    /// <returns></returns>
    static OGRLinearRing silly_ring_to_ogr(const silly_ring& ring);


    /// ================ 单面 ================

    /// <summary>
    /// OGRPolygon 对象转换为 silly_poly (多环:外环+内环)对象  (单面)
    /// </summary>
    /// <param name="polygon"></param>
    /// <returns></returns>
    static silly_poly silly_poly_from_ogr(const OGRPolygon* polygon);

    /// <summary>
    /// 将 silly_poly 转换为 OGRPolygon(单面)
    /// </summary>
    /// <param name="poly"></param>
    /// <returns></returns>
    static OGRPolygon silly_poly_to_ogr(const silly_poly& poly);

    /// ================ 多面 ================

    /// <summary>
    /// 多面的OGRMultiPolygon对象转换为silly_multi_poly(多面)
    /// </summary>
    /// <param name="multiPolygon"></param>
    /// <returns></returns>
    static silly_multi_poly silly_multi_poly_from_ogr(const OGRMultiPolygon* multiPolygon);

    /// <summary>
    /// 将silly_multi_poly对象转换为OGRMultiPolygon对象(多面)
    /// </summary>
    /// <param name="multiPoly"></param>
    /// <returns></returns>
    static OGRMultiPolygon silly_multi_poly_to_ogr(const silly_multi_poly& multiPoly);
};

typedef silly_geo_utils geo_utils;  // 兼容之前的写法

#endif  // SILLY_UTILS_SILLY_GEO_OPERATION_H