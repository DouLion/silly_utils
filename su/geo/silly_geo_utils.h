#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_OPERATION_H
#define SILLY_UTILS_SILLY_GEO_OPERATION_H

#include <geo/silly_geo_coll.h>
#include <log/silly_log.h>
#include <geo/proj/gdal/silly_projection_define.h>
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

namespace silly
{
namespace geo
{
class utils
{
  public:
    /// <summary>
    /// 初始化 GDAL 环境,只需要初始化一次
    /// </summary>
    static void init_gdal_env();

    /// <summary>
    /// 销毁 GDAL 环境, 一旦执行此方法,那么当前进程及其子线程中GDAL就无法使用
    /// </summary>
    static void destroy_gdal_env();

    /// <summary>
    /// 求一个面的形心(几何中心),利用ogr库算法
    /// </summary>
    /// <param name="poly"></param>
    /// <returns></returns>
    static silly_point poly_centroid(const silly_poly& poly);

    /// <summary>
    /// 求两个点的方位角,p2相对于p1的方位角(左上角右下角坐标系均可), 正北方向为0度,顺时针
    /// </summary>
    /// <param name="p1">参照物</param>
    /// <param name="p2">参照方向</param>
    /// <returns>p2相对于p1的方位角,结果为角度值,</returns>
    static double azimuth(silly_point from, silly_point to);

    /// <summary>
    /// 角度转方向 正北方向为0度, 顺时针
    /// </summary>
    /// <param name="angle"></param>
    /// <returns></returns>
    static std::string angle_to_desc(const double& angle);

    /// <summary>
    /// 读取矢量文件中的数据存储到silly_geo_coll数据结构中
    /// </summary>
    /// <param name="u8file">utf8</param>
    /// <param name="collection"></param>
    /// <returns></returns>
    /// 注:读取 shp , geojson 类型文件中可以实现
    static bool read_geo_coll(const std::string& u8file, std::vector<silly_geo_coll>& collections, const bool& ignore_prop = false);

    /// <summary>
    /// 将silly_geo_coll数据结构写入矢量文件(如shp文件)
    /// </summary>
    /// <param name="u8file">utf8</param>
    /// <param name="collection"></param>
    /// <returns></returns>
    /// 注:写入 shp , geojson 类型文件中经测试可以实现
    static bool write_geo_coll(const std::string& u8file, const std::vector<silly_geo_coll>& collections, const proj::CRS::type& prj = proj::CRS::GCS_WGS_1984);

    /// <summary>
    /// 是否为一个标准的shp文件
    /// </summary>
    /// <param name="u8file">utf8</param>
    /// <returns></returns>
    static bool is_valid_shp(const std::string& u8file);

    /// <summary>
    /// 检查shp文件组中缺失的文件
    /// .shp: 存储几何矢量
    /// .dbf: 存储属性信息 (该文件不存在 gdal也能打开shp,成功读取矢量,但是无法读取属性信息)
    /// .shx: 存储几何矢量索引
    /// </summary>
    /// <param name="u8file"></param>
    /// <returns></returns>
    static std::vector<std::string> shp_missing_file(const std::string& u8file);

    /// <summary>
    /// 加载shp文件中的属性信息
    /// </summary>
    /// <param name="u8file">utf8</param>
    /// <param name="type"></param>
    /// <param name="properties"></param>
    /// <returns></returns>
    static bool check_shp_info(const std::string& u8file, enum_geometry_type& type, std::map<std::string, silly_geo_prop::enum_prop_type>& properties);

    /// <summary>
    /// 根据文件类型得到对应的 gdal 中的存储格式
    /// 支持的文件格式对应的存储类型: shp tab geojson sqlite csv kml gml xlsx
    /// </summary>
    /// <param name="u8file">文件名</param>
    /// <param name="driverName">存储类型</param>
    /// <returns></returns>
    static bool get_driver_name(const std::string& u8file, std::string& driverName);

    /// <summary>
    /// 矢量与面是否相交
    /// </summary>
    /// <param name=""></param>
    /// <param name=""></param>
    /// <returns></returns>
    static bool intersect(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2);

    /// <summary>
    /// 点是否与矢量面相交(射线算法),点的相交在面内或者在边界上都算是相交
    /// </summary>
    /// <param name="mpoly"></param>
    /// <param name="point"></param>
    /// <returns></returns>
    static bool intersect(const silly_poly& mpoly, const silly_point& point);

    /// <summary>
    /// 点是否与矢量多面是否相交(射线算法),点的相交在面内或者在边界上都算是相交
    /// </summary>
    /// <param name="mpoly"></param>
    /// <param name="point"></param>
    /// <returns></returns>
    static bool intersect(const silly_multi_poly& mpoly, const silly_point& point);

    static bool intersect(const silly_multi_poly& mpoly, const silly_line& line);

    static bool intersect(const silly_point& point, const std::vector<silly_point>& points);

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
    /// 两个线段相交点
    /// </summary>
    /// <param name="s1"></param>
    /// <param name="s2"></param>
    /// <returns></returns>
    static std::optional<silly_point> intersection(const silly_segment& s1, const silly_segment& s2);

    /// <summary>
    /// 两个相交面的 不相交部分, 必须是相交的
    /// </summary>
    /// <param name="mpoly1"></param>
    /// <param name="mpoly2"></param>
    /// <returns></returns>
    static std::vector<silly_poly> trans_intersection(const silly_multi_poly& mpoly1, const silly_multi_poly& mpoly2);

    /// <summary>
    /// 线相交面的部分
    /// </summary>
    /// <param name="mpoly1"></param>
    /// <param name="line"></param>
    /// <returns></returns>
    static std::vector<silly_line> intersection(const silly_multi_poly& mpoly, const silly_line& line);

    /// <summary>
    /// 穿过面的线, 不再面内的部分
    /// </summary>
    /// <param name="mpoly1"></param>
    /// <param name="mpoly2"></param>
    /// <returns></returns>
    static std::vector<silly_line> trans_intersection(const silly_multi_poly& mpoly1, const silly_line& line);

    /// <summary>
    /// 距离,直接数值计算
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <returns></returns>
    static double distance(const silly_point& p1, const silly_point& p2);

    /// <summary>
    /// 距离的平方,直接数值计算
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <returns></returns>
    static double distance_sq(const silly_point& p1, const silly_point& p2);

    /// <summary>
    /// 距离,经纬度转换为千米计算,Vincenty公式,
    /// 相较于墨卡托投影方法, 跨带也能正常使用
    /// https://github.com/atychang/geo-distance/blob/master/vincenty/cpp/CalcDistance.cc
    /// http://www.movable-type.co.uk/scripts/latlong-vincenty.html
    /// https://en.wikipedia.org/wiki/Vincenty's_formulae
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <returns></returns>
    static double distance_km(const silly_point& p1, const silly_point& p2);

    /// <summary>
    /// 面积
    /// </summary>
    /// <param name="points"></param>
    /// <returns></returns>
    static double area(const std::vector<silly_point>& points);

    /// <summary>
    /// 面积
    /// </summary>
    /// <param name="xs"></param>
    /// <param name="ys"></param>
    /// <returns></returns>
    template <typename T>
    static double area(const std::vector<T>& xs, const std::vector<T>& ys);

    /// <summary>
    /// 面积
    /// </summary>
    /// <param name="pnum">点数量</param>
    /// <param name="xs"></param>
    /// <param name="ys"></param>
    /// <returns></returns>
    template <typename T>
    // typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    static double area(const int& pnum, const T* xs, const T* ys);

    /// <summary>
    /// 面积
    /// </summary>
    /// <param name="pnum">点数量</param>
    /// <param name="points">x0,y0,x1,y1 .... xn,yn n = pnum - 1</param>
    /// <returns></returns>
    template <typename T>
    static double area(const int& pnum, const T* points);

    /// <summary>
    /// 面积
    /// </summary>
    /// <param name="mpoly"></param>
    /// <returns></returns>
    static double area(const silly_poly& poly);

    /// <summary>
    /// 计算面积平方公里
    /// </summary>
    /// <param name="points">经纬度点</param>
    /// <returns></returns>
    static double area_sqkm(const std::vector<silly_point>& points);

    /// <summary>
    /// 计算面积平方公里
    /// </summary>
    /// <param name="mpoly">经纬度面</param>
    /// <returns></returns>
    static double area_sqkm(const silly_poly& poly);

    /// <summary>
    /// 面积
    /// </summary>
    /// <param name="mpoly"></param>
    /// <returns></returns>
    static double area(const silly_multi_poly& mpoly);

    /// <summary>
    /// 计算面积平方公里
    /// </summary>
    /// <param name="mpoly">经纬度面</param>
    /// <returns></returns>
    static double area_sqkm(const silly_multi_poly& mpoly);

    /// <summary>
    /// 平滑线
    /// </summary>
    /// <param name="line"></param>
    /// <param name="mod">二次贝塞尔 或者 三次贝塞尔</param>
    /// <param name="interp">插值</param>
    /// <returns></returns>
    static std::vector<silly_point> smooth_line(const std::vector<silly_point>& line, const int& mod, const int& interp);

    /// <summary>
    /// 平滑闭合环
    /// </summary>
    /// <param name="line"></param>
    /// <param name="mod">二次贝塞尔 或者 三次贝塞尔 或者其他</param>
    /// <param name="interp">插值</param>
    /// <returns></returns>
    static std::vector<silly_point> smooth_ring(const std::vector<silly_point>& ring, const int& mod, const int& interp);

    /// <summary>
    /// 简化线,使用的道格拉斯抽稀算法,可以尽量保持矢量特征
    /// </summary>
    /// <param name="line"></param>
    /// <param name="dist">距离容差</param>
    /// <returns></returns>
    static std::vector<silly_point> simplify_line(const std::vector<silly_point>& line, const double& dist);

    /// <summary>
    /// 简化闭合环
    /// </summary>
    /// <param name="line"></param>
    /// <param name="dist">距离容差</param>
    /// <returns></returns>
    static std::vector<silly_point> simplify_ring(const std::vector<silly_point>& ring, const double& dist);

    /// <summary>
    /// 创建缓冲区
    /// </summary>
    /// <param name="coll"></param>
    /// <param name="distance">缓冲区大小(正负均可),要求coll中矢量的单位和distance单位保持一致</param>
    /// <returns></returns>
    static silly_geo_coll buffer(const silly_geo_coll& coll, const double& distance);

    /// <summary>
    /// 根据首尾高程调整线高程
    /// </summary>
    /// <param name="linez">线位置及其高程</param>
    /// <param name="bz">第一个点的矫正高程</param>
    /// <param name="ez">最后一个点的矫正高程</param>
    /// <returns></returns>
    static std::vector<std::pair<silly_point, double>> adjust(const std::vector<std::pair<silly_point, double>>& linez, const double& bz, const double& ez);

#if ENABLE_GDAL

    /// <summary>
    /// 读取shp文件
    /// </summary>
    /// <param name="u8file"></param>
    /// <param name="collections"></param>
    /// <param name="ignore_prop"></param>
    /// <returns></returns>
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
    /// <param name="multiPoint"></param>
    /// <returns></returns>
    static OGRMultiPoint silly_multi_point_to_ogr(const silly_multi_point& multiPoint);

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

    /// ================ OGRGeometry ================

    /// <summary>
    /// 将 OGRGeometry 对象转换为silly_geo_coll对象,
    /// OGRGeometry是一个抽象类，无法实例化,只能以指针的方式使用,
    /// 注意: 返回的对象需要手动释放,释放方法:
    /// if (OGRGeometry* != nullptr)
    /// {
    ///    OGRGeometryFactory::destroyGeometry(OGRGeometry*);
    ///    OGRGeometry* = nullptr;
    /// }
    /// </summary>
    /// <param name="coll"></param>
    /// <returns></returns>
    static OGRGeometry* silly_geo_coll_to_ogr(const silly_geo_coll& coll);

    /// <summary>
    /// 将silly_geo_coll对象转换为OGRGeometry对象
    /// </summary>
    /// <param name="geometry"></param>
    /// <returns></returns>
    static silly_geo_coll silly_geo_coll_from_ogr(const OGRGeometry* geometry);
#endif
};

template <typename T>
double utils::area(const std::vector<T>& xs, const std::vector<T>& ys)
{
    double result = 0.0;
    if (xs.size() != ys.size())
    {
        return result;
    }
    size_t pnum = xs.size();
    // 确保至少有3个点才能构成一个多边形
    if (pnum < 3)
    {
        return result;
    }

    for (int i = 0; i < pnum; ++i)
    {
        int j = (i + 1) % pnum;
        result += xs[i] * ys[j];
        result -= xs[j] * ys[i];
    }
    return std::abs(result) / 2.0;
}
template <typename T>
double utils::area(const int& pnum, const T* points)
{
    double result = 0.0;
    // 确保至少有3个点才能构成一个多边形
    if (pnum < 3)
    {
        return result;
    }

    for (int n = 0; n < pnum; ++n)
    {
        int i = n * 2;
        int j = ((n + 1) % pnum) * 2;
        result += points[i] * points[j + 1];
        result -= points[j] * points[i + 1];
    }
    return std::abs(result) / 2.0;
}
template <typename T>
double utils::area(const int& pnum, const T* xs, const T* ys)
{
    double result = 0.0;
    // 确保至少有3个点才能构成一个多边形
    if (pnum < 3)
    {
        return result;
    }

    for (int i = 0; i < pnum; ++i)
    {
        size_t j = (i + 1) % pnum;
        result += xs[i] * ys[j];
        result -= xs[j] * ys[i];
    }
    return std::abs(result) / 2.0;
}
}  // namespace geo
}  // namespace silly
typedef silly::geo::utils silly_geo_utils;
typedef silly_geo_utils geo_utils;  // 兼容之前的写法

#endif  // SILLY_UTILS_SILLY_GEO_OPERATION_H