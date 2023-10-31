//
// Created by dell on 2023/10/24.
//

#ifndef SILLY_UTILS_SILLY_SPATIALITE_H
#define SILLY_UTILS_SILLY_SPATIALITE_H
#include <iostream>
#include <string.h>
#include <geo/silly_geo.h>
#include <geos_c.h>
#include <sqlite3.h>
#include <spatialite.h>
#include <spatialite/gaiageo.h>



struct vector_detail
{
    std::string m_source;
    enum_geometry_types m_type{ enum_geometry_types::eInvalid };
    bool is_modified{ false };
    std::vector<silly_point> m_points;
    std::vector<silly_line> m_lines;
    std::vector<silly_multi_poly> m_m_polys;
    time_t 	m_last_visit;	// 最新一次的访问时间
    vector_detail& operator=(const vector_detail& other)
    {
        this->m_source = other.m_source;
        this->m_type = other.m_type;
        this->m_points = other.m_points;
        this->m_lines = other.m_lines;
        this->m_m_polys = other.m_m_polys;
        this->m_last_visit = other.m_last_visit;
        return *this;
    }

};

static char* geom_type(int type)
{
    /* 实用程序函数以字符串形式返回相应的GeometryType */
    static char* name = "EMPTY / NULL GEOMETRY";
    if (type == GAIA_POINT)
        name = "POINT";
    if (type == GAIA_LINESTRING)
        name = "LINESTRING";
    if (type == GAIA_POLYGON)
        name = "POLYGON";
    if (type == GAIA_MULTIPOINT)
        name = "MULTIPOINT";
    if (type == GAIA_MULTILINESTRING)
        name = "MULTILINESTRING";
    if (type == GAIA_MULTIPOLYGON)
        name = "MULTIPOLYGON";
    if (type == GAIA_GEOMETRYCOLLECTION)
        name = "GEOMETRYCOLLECTION";
    return name;
}


/// <summary>
/// 打印查看gaiaGeomCollPtr类型对象的类型和坐标点
/// </summary>
/// <param name="geom"></param>
static void geometry_printout(gaiaGeomCollPtr geom)
{
    gaiaPointPtr pt;
    gaiaLinestringPtr ln;
    gaiaPolygonPtr pg;
    gaiaRingPtr rng;
    int n_pts = 0, n_lns = 0, n_pgs = 0;
    int cnt, iv, ir;
    double x = 0.0, y = 0.0;
    // 计算点数
    pt = geom->FirstPoint;
    while (pt) {
        n_pts++;
        pt = pt->Next;
    }
    // 计算线串数
    ln = geom->FirstLinestring;
    while (ln) {
        n_lns++;
        ln = ln->Next;
    }
    // 计算多边形数
    pg = geom->FirstPolygon;
    while (pg) {
        n_pgs++;
        pg = pg->Next;
    }
    // 打印点坐标
    if (n_pts) {
        cnt = 0;
        pt = geom->FirstPoint;
        while (pt) {
            // 扫描POINT的链接列表
            std::cout << "\t\t\tPOINT " << cnt << "/" << n_pts << " x=" << pt->X << " y=" << pt->Y << std::endl;
            cnt++;
            pt = pt->Next;
        }
    }
    // 打印线串坐标
    if (n_lns) {
        cnt = 0;
        ln = geom->FirstLinestring;
        while (ln) {
            // 扫描线串的链接列表
            std::cout << "\t\t\tLINESTRING " << cnt << "/" << n_lns << " has " << ln->Points << " vertices" << std::endl;
            for (iv = 0; iv < ln->Points; iv++) {
                // 检索每个顶点的坐标
                gaiaGetPoint(ln->Coords, iv, &x, &y);
                std::cout << "\t\t\t\tvertex " << iv << "/" << ln->Points << " x=" << x << " y=" << y << std::endl;
            }
            cnt++;
            ln = ln->Next;
        }
    }
    // 打印多边形坐标
    if (n_pgs) {
        cnt = 0;
        pg = geom->FirstPolygon;
        while (pg) {
            // 扫描多边形的链接列表
            std::cout << "\t\t\tPOLYGON " << cnt << "/" << n_pgs << " has " << pg->NumInteriors << " hole";
            if (pg->NumInteriors != 1) {
                std::cout << "s";
            }
            std::cout << std::endl;
            // 打印外环
            rng = pg->Exterior;
            std::cout << "\t\t\t\tExteriorRing has " << rng->Points << " vertices" << std::endl;
            for (iv = 0; iv < rng->Points; iv++) {
                // 检索每个顶点的坐标
                gaiaGetPoint(rng->Coords, iv, &x, &y);
                std::cout << "\t\t\t\t\tvertex " << iv << "/" << rng->Points << " x=" << x << " y=" << y << std::endl;
            }
            for (ir = 0; ir < pg->NumInteriors; ir++) {
                // 多边形可以包含任意数量的内环（包括零）
                rng = pg->Interiors + ir;
                std::cout << "\t\t\t\tInteriorRing " << ir << "/" << pg->NumInteriors << " has " << rng->Points << " vertices" << std::endl;
                for (iv = 0; iv < rng->Points; iv++) {
                    // 检索每个顶点的坐标
                    gaiaGetPoint(rng->Coords, iv, &x, &y);
                    std::cout << "\t\t\t\t\tvertex " << iv << "/" << rng->Points << " x=" << x << " y=" << y << std::endl;
                }
            }
            cnt++;
            pg = pg->Next;
        }
    }
}

class silly_spatialite_1
{
public:

    // silly中点线面与gaiaGeomCollPtr中的点线面的转换

    // 单点转换
    // 将std::vector<silly_point> 转换为 gaiaGeomCollPtr 的函数
    static gaiaGeomCollPtr vectorToPointGeomColl(const std::vector<silly_point>& points);

    // 将 gaiaGeomCollPtr 转换为 std：：vector <silly_point>的函数
    static std::vector<silly_point> PointgeomCollToVector(const gaiaGeomCollPtr geo);

    // 线的转换
    // 从silly_line转换为GeomCollPtr (单线)
    static gaiaGeomCollPtr sillyLineToGeomColl(const silly_line& line);
    //silly_multi_silly_line转换为gaiaGeomCollPtr类型 (多线重载)
    static  gaiaGeomCollPtr sillyLineToGeomColl(const silly_multi_silly_line& multiLine);

    // gaiaGeomCollPtr转换为silly_line的功能

    //gaiaGeomCollPtr类型转换为silly_multi_silly_line(多线)
    static silly_multi_silly_line geomCollToMultiLine(const gaiaGeomCollPtr& geomColl);


    // 多面
    //将silly_multi_poly转换为gaiaGeomCollPtr类型
    static gaiaGeomCollPtr multiPolyToGeomColl(const silly_multi_poly& multiPoly);

    // 将gaiaGeomCollPtr类型转换为silly_multi_poly
    static silly_multi_poly geomCollToMultiPoly(const gaiaGeomCollPtr& geomColl);

    static int test();


    // 初始化SpatiaLite
    void initSpatialite();
    // 销毁SpatiaLite
    void destorySpatialite();
    // 打开数据库
    bool openDatabase(const char* dbName);

    void closeDatabase();


    // 创建数据库和表
    static bool createDatabase(const char* dbName);

    // gaiaGeomCollPtr矢量数据存储到数据库表中
    static bool insertSpatialData(const char* dbName, gaiaGeomCollPtr geomColl);
    static bool insertSpatialData(const char* dbName, std::vector<gaiaGeomCollPtr> geomColls);

    static bool selectSpatialData(const char* dbName, std::vector<gaiaGeomCollPtr>& geomColls);

private:
    sqlite3* db;
    void* cache;
};

#endif //SILLY_UTILS_SILLY_SPATIALITE_H
