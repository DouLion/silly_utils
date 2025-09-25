#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_OPERATION_H
#define SILLY_UTILS_SILLY_GEO_OPERATION_H

#include <geo/silly_geo_coll.h>
#include <geo/proj/gdal/silly_projection_define.h>
#include <geo/silly_gdal.h>

namespace silly::geo
{
class utils
{
  public:

    static void init_proj_env();

    /// <summary>
    /// 初始化 GDAL 环境,只需要初始化一次
    /// </summary>
    static void init_gdal_env();

    /// <summary>
    /// 销毁 GDAL 环境, 一旦执行此方法,那么当前进程及其子线程中GDAL就无法使用
    /// </summary>
    static void destroy_gdal_env();

    static void centroid(const silly_ring& ring, double& area, double& sumX, double& sumY);
    /// <summary>
    /// 求一个面的形心(几何中心),利用ogr库算法
    /// </summary>
    /// <param name="poly"></param>
    /// <returns></returns>
    static silly_point centroid(const silly_poly& poly);

    static void centroid(const silly_poly& poly, silly_point& centroid, double& area);
    static silly_point centroid(const silly_multi_poly& multiPoly);

    /// <summary>
    /// 求两个点的方位角,p2相对于p1的方位角(左上角右下角坐标系均可), 正北方向为0度,顺时针
    /// </summary>
    /// <param name="from">参照物</param>
    /// <param name="to">参照方向</param>
    /// <returns>p2相对于p1的方位角,结果为角度值,</returns>
    static double azimuth(const silly_point& from, const silly_point& to);

    /// <summary>
    /// 角度转方向 正北方向为0度, 顺时针
    /// </summary>
    /// <param name="angle"></param>
    /// <returns></returns>
    static std::string angle_to_desc(const double& angle);

    /// <summary>
    /// 读取矢量文件中的数据存储到silly_geo_coll数据结构中
    /// </summary>
    /// <param name="file"></param>
    /// <param name="collection"></param>
    /// <returns></returns>
    /// 注:读取 shp , geojson 类型文件中可以实现
    static bool read(const std::filesystem::path& file, std::vector<silly_geo_coll>& collection, const bool& ignore_prop = false);
    static std::vector<silly_geo_coll> read(const std::filesystem::path& file, const bool& ignore_prop = false);

    /// <summary>
    /// 将silly_geo_coll数据结构写入矢量文件(如shp文件)
    /// </summary>
    /// <param name="file"></param>
    /// <param name="collection"></param>
    /// <param name="prj">指定坐标系</param>
    /// <param name="encode">指定输出的中文编码</param>
    /// <returns></returns>
    /// 注:写入 shp , geojson 类型文件中经测试可以实现
    static bool write(const std::filesystem::path& file, const std::vector<silly_geo_coll>& collection, const eCrsEpsgCode& prj = GCS_WGS_1984, const std::string& encode = "UTF-8");

    static bool intersect(const silly_geo_coll& gc1, const silly_geo_coll& gc2);

    /// <summary>
    /// 矢量与面是否相交
    /// </summary>
    /// <param name="multiPoly1"></param>
    /// <param name="multiPoly2"></param>
    /// <returns></returns>
    static bool intersect(const silly_multi_poly& multiPoly1, const silly_multi_poly& multiPoly2);

    /// <summary>
    /// 点是否与矢量面相交(射线算法),点的相交在面内或者在边界上都算是相交
    /// </summary>
    /// <param name="multiPoly"></param>
    /// <param name="point"></param>
    /// <returns></returns>
    static bool intersect(const silly_poly& multiPoly, const silly_point& point);

    /// <summary>
    /// 点是否与矢量多面是否相交(射线算法),点的相交在面内或者在边界上都算是相交
    /// </summary>
    /// <param name="multiPoly"></param>
    /// <param name="point"></param>
    /// <returns></returns>
    static bool intersect(const silly_multi_poly& multiPoly, const silly_point& point);

    static bool intersect(const silly_multi_poly& multiPoly, const silly_line& line);

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
    /// <param name="multiPoly1"></param>
    /// <param name="multiPoly2"></param>
    /// <returns></returns>
    static std::vector<silly_poly> intersection(const silly_multi_poly& multiPoly1, const silly_multi_poly& multiPoly2);

    /// <summary>
    /// 两个线段相交点
    /// </summary>
    /// <param name="s1"></param>
    /// <param name="s2"></param>
    /// <returns></returns>
    static std::optional<silly_point> intersection(const silly_segment& s1, const silly_segment& s2);
    static std::optional<silly_pointZ> intersection(const silly_segmentZ& s1, const silly_segmentZ& s2);

    /// <summary>
    /// 两个相交面的 不相交部分, 必须是相交的
    /// </summary>
    /// <param name="multiPoly1"></param>
    /// <param name="multiPoly2"></param>
    /// <returns></returns>
    static std::vector<silly_poly> trans_intersection(const silly_multi_poly& multiPoly1, const silly_multi_poly& multiPoly2);

    /// <summary>
    /// 线相交面的部分
    /// </summary>
    /// <param name="multiPoly"></param>
    /// <param name="line"></param>
    /// <returns></returns>
    static std::vector<silly_line> intersection(const silly_multi_poly& multiPoly, const silly_line& line);

    /// <summary>
    /// 穿过面的线, 不再面内的部分
    /// </summary>
    /// <param name="multiPoly1"></param>
    /// <param name="line"></param>
    /// <returns></returns>
    static std::vector<silly_line> trans_intersection(const silly_multi_poly& multiPoly1, const silly_line& line);

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
    /// <param name="poly"></param>
    /// <returns></returns>
    static double area(const silly_poly& poly);

    /// <summary>
    /// 计算面积平方公里
    /// </summary>
    /// <param name="points">经纬度点</param>
    /// <returns></returns>
    static double area_sqkm(const std::vector<silly_point>& points, const double& l0);

    /// <summary>
    /// 计算面积平方公里
    /// </summary>
    /// <param name="poly">经纬度面</param>
    /// <returns></returns>
    static double area_sqkm(const silly_poly& poly, const double& l0);

    /// <summary>
    /// 面积
    /// </summary>
    /// <param name="multiPoly">高斯或者墨卡托面</param>
    /// <returns></returns>
    static double area(const silly_multi_poly& multiPoly);

    /// <summary>
    /// 计算面积平方公里
    /// </summary>
    /// <param name="multiPoly">经纬度面</param>
    /// <returns></returns>
    static double area_sqkm(const silly_multi_poly& multiPoly, const double& l0);

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
    /// <param name="ring"></param>
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
    /// <param name="ring"></param>
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
}  // namespace silly::geo

typedef silly::geo::utils silly_geo_utils;
typedef silly::geo::utils geo_utils;  // 兼容之前的写法
typedef silly::geo::utils sugeoutils;

#endif  // SILLY_UTILS_SILLY_GEO_OPERATION_H