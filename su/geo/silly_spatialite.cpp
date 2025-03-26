//
// Created by dell on 2023/10/24.
//

#include "silly_spatialite.h"
#include <geo/silly_geo.h>
#include <geos_c.h>
#include <sqlite3.h>
#include <spatialite.h>
#include <spatialite/gaiageo.h>

//////////////////////////////////////////////////////////////////////////////////////////
///////  silly_geo转为spatialite类型互转 实现
//////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// 区分类型将silly_geo转为spatialite内的矢量类型(单点类型)
/// </summary>
/// <param name="point">silly类型点</param>
/// <param name="ggcp">spatialite类型点</param>
/// <returns></returns>
static bool silly_point_to_gaiageo(const silly_point &point, gaiaGeomCollPtr &ggcp)
{
    ggcp = gaiaAllocGeomColl();
    ggcp->DimensionModel = GAIA_XY;
    gaiaAddPointToGeomColl(ggcp, point.x, point.y);
    return true;
}

/// <summary>
/// 区分类型将silly_geo转为spatialite内的矢量类型(多点类型)
/// </summary>
/// <param name="mpoint">silly类型多点</param>
/// <param name="ggcp">spatialite类型多点</param>
/// <returns></returns>
static bool silly_multi_point_to_gaiageo(const silly_multi_point &mpoint, gaiaGeomCollPtr &ggcp)
{
    if (!mpoint.empty())
    {
        ggcp = gaiaAllocGeomColl();
        ggcp->DimensionModel = GAIA_XY;
        for (const auto &point : mpoint)
        {
            gaiaAddPointToGeomColl(ggcp, point.x, point.y);
        }
        return true;
    }

    return false;
}

/// <summary>
/// 区分类型将silly_geo转为spatialite内的矢量类型(单线类型)
/// </summary>
/// <param name="line">silly类型单线</param>
/// <param name="ggcp">spatialite类型单线</param>
/// <returns></returns>
static bool silly_line_to_gaiageo(const silly_line &line, gaiaGeomCollPtr &ggcp)
{
    if (!line.empty())
    {
        ggcp = gaiaAllocGeomColl();
        ggcp->DimensionModel = GAIA_XY;
        gaiaLinestringPtr gaiaLine = gaiaAddLinestringToGeomColl(ggcp, line.size());
        for (int i = 0; i < line.size(); i++)
        {
            gaiaSetPoint(gaiaLine->Coords, i, line[i].x, line[i].y);
        }
        return true;
    }
    return false;
}

/// <summary>
/// 区分类型将silly_geo转为spatialite内的矢量类型(多线类型)
/// </summary>
/// <param name="mline">silly类型多线</param>
/// <param name="ggcp">spatialite类型多线</param>
/// <returns></returns>
static bool silly_multi_silly_line_to_gaiageo(const silly_multi_silly_line &mline, gaiaGeomCollPtr &ggcp)
{
    if (!mline.empty())
    {
        ggcp = gaiaAllocGeomColl();
        ggcp->DimensionModel = GAIA_XY;
        for (const auto &line : mline)
        {
            gaiaLinestringPtr gaiaLine = gaiaAddLinestringToGeomColl(ggcp, line.size());
            for (size_t i = 0; i < line.size(); i++)
            {
                gaiaSetPoint(gaiaLine->Coords, i, line[i].x, line[i].y);
            }
        }
        return true;
    }
    return false;
}

/// <summary>
/// 将silly_geo转为spatialite内的矢量类型(单面类型)
/// </summary>
/// <param name="poly">silly类型单面</param>
/// <param name="ggcp">spatialite类型多面</param>
/// <returns></returns>
static bool silly_poly_to_gaiageo(const silly_poly &poly, gaiaGeomCollPtr &ggcp)
{
    if (!poly.outer_ring.points.empty())
    {
        ggcp = gaiaAllocGeomColl();
        ggcp->DimensionModel = GAIA_XY;
        gaiaPolygonPtr gaiaPoly = gaiaAddPolygonToGeomColl(ggcp, poly.outer_ring.points.size(), poly.inner_rings.size());
        // 添加外环
        gaiaRingPtr exteriorRing = gaiaPoly->Exterior;
        int exteriorPointNum = 0;
        for (const auto &point : poly.outer_ring.points)
        {
            gaiaSetPoint(exteriorRing->Coords, exteriorPointNum, point.x, point.y);
            exteriorPointNum++;
        }
        // 添加内环
        int inner_num = 0;
        for (const auto &innerRing : poly.inner_rings)
        {
            gaiaRingPtr interiorRing = gaiaAddInteriorRing(gaiaPoly, inner_num, innerRing.points.size());
            int interPointNum = 0;
            for (const auto &point : innerRing.points)
            {
                gaiaSetPoint(interiorRing->Coords, interPointNum, point.x, point.y);
                interPointNum++;
            }
            inner_num++;
        }
        return true;
    }
    return false;
}

/// <summary>
/// 将silly_geo转为spatialite内的矢量类型(多面类型)
/// </summary>
/// <param name="mpoly">silly类型多面</param>
/// <param name="ggcp">spatialite类型多面</param>
/// <returns></returns>
static bool silly_multi_poly_to_gaiageo(const silly_multi_poly &mpoly, gaiaGeomCollPtr &ggcp)
{
    if (!mpoly.empty())
    {
        ggcp = gaiaAllocGeomColl();
        ggcp->DimensionModel = GAIA_XY;
        for (const auto &poly : mpoly)
        {
            gaiaPolygonPtr gaiaPoly = gaiaAddPolygonToGeomColl(ggcp, poly.outer_ring.points.size(), poly.inner_rings.size());
            // 添加外环
            gaiaRingPtr exteriorRing = gaiaPoly->Exterior;
            int exteriorPointNum = 0;
            for (const auto &point : poly.outer_ring.points)
            {
                gaiaSetPoint(exteriorRing->Coords, exteriorPointNum, point.x, point.y);
                exteriorPointNum++;
            }
            // 添加内环
            int inner_num = 0;
            for (const auto &innerRing : poly.inner_rings)
            {
                gaiaRingPtr interiorRing = gaiaAddInteriorRing(gaiaPoly, inner_num, innerRing.points.size());
                int interPointNum = 0;
                for (const auto &point : innerRing.points)
                {
                    gaiaSetPoint(interiorRing->Coords, interPointNum, point.x, point.y);
                    interPointNum++;
                }
                inner_num++;
            }
        }
        return true;
    }
    return false;
}

/// <summary>
/// 将spatialite内的矢量类型转为silly_geo(单点类型)
/// </summary>
/// <param name="ggcp">spatialite类型点</param>
/// <param name="point">silly类型点</param>
/// <returns></returns>
static bool gaiageo_to_silly_point(gaiaGeomCollPtr ggcp, silly_point &point)
{
    if (ggcp->FirstPoint)
    {
        gaiaPointPtr pt;
        pt = ggcp->FirstPoint;
        point = silly_point(pt->X, pt->Y);
        return true;
    }
    return false;
}

/// <summary>
/// 将spatialite内的矢量类型转为silly_geo(多点类型)
/// </summary>
/// <param name="ggcp">spatialite类型多点</param>
/// <param name="mpoint">silly类型多点</param>
/// <returns></returns>
static bool gaiageo_to_silly_multi_point(gaiaGeomCollPtr ggcp, silly_multi_point &mpoint)
{
    gaiaPointPtr pt;
    pt = ggcp->FirstPoint;
    while (pt)
    {
        mpoint.push_back(silly_point(pt->X, pt->Y));
        pt = pt->Next;
    }
    return true;
}

/// <summary>
/// 将spatialite内的矢量类型转为silly_geo(单线类型)
/// </summary>
/// <param name="ggcp">spatialite类型单线</param>
/// <param name="line">silly类型单线</param>
/// <returns></returns>
static bool gaiageo_to_silly_line(gaiaGeomCollPtr ggcp, silly_line &line)
{
    gaiaLinestringPtr gaia_line = ggcp->FirstLinestring;
    if (gaia_line)
    {
        for (int i = 0; i < gaia_line->Points; i++)
        {
            double x = 0.0, y = 0.0;
            gaiaGetPoint(gaia_line->Coords, i, &x, &y);
            line.push_back(silly_point(x, y));
        }
        return true;
    }

    return false;
}

/// <summary>
/// 将spatialite内的矢量类型转为silly_geo(多线类型)
/// </summary>
/// <param name="ggcp">spatialite类型多线</param>
/// <param name="mline">silly类型多线</param>
/// <returns></returns>
static bool gaiageo_to_silly_multi_line(gaiaGeomCollPtr ggcp, silly_multi_silly_line &mline)
{
    gaiaLinestringPtr line = ggcp->FirstLinestring;
    while (line)
    {
        silly_line sillyLine;
        for (int i = 0; i < line->Points; i++)
        {
            double x = 0.0, y = 0.0;
            gaiaGetPoint(line->Coords, i, &x, &y);
            sillyLine.push_back(silly_point(x, y));
        }
        mline.push_back(sillyLine);
        line = line->Next;
    }
    return true;
}

/// <summary>
/// 将spatialite内的矢量类型转为silly_geo(单面类型)
/// </summary>
/// <param name="ggcp">spatialite类型单面</param>
/// <param name="poly">silly类型单面</param>
/// <returns></returns>
static bool gaiageo_to_silly_poly(gaiaGeomCollPtr ggcp, silly_poly &sillyPoly)
{
    gaiaPolygonPtr ggcpoly = ggcp->FirstPolygon;
    if (ggcpoly)
    {
        // 外环
        silly_ring outerRing;
        gaiaRingPtr exteriorRing = ggcpoly->Exterior;
        for (int i = 0; i < exteriorRing->Points; i++)
        {
            double x = 0.0, y = 0.0;
            gaiaGetPoint(exteriorRing->Coords, i, &x, &y);
            outerRing.points.push_back(silly_point(x, y));
        }
        sillyPoly.outer_ring = outerRing;
        // 内环
        int interNum = ggcpoly->NumInteriors;
        for (int n = 0; n < interNum; n++)
        {
            gaiaRingPtr interiorRing = ggcpoly->Interiors;
            while (interiorRing)
            {
                silly_ring innerRing;
                for (int j = 0; j < interiorRing->Points; j++)
                {
                    double x = 0.0, y = 0.0;
                    gaiaGetPoint(interiorRing->Coords, j, &x, &y);
                    innerRing.points.push_back(silly_point(x, y));
                }
                sillyPoly.inner_rings.push_back(innerRing);
                interiorRing = interiorRing->Next;
            }
        }
    }
    return true;
}

/// <summary>
/// 将spatialite内的矢量类型转为silly_geo(多面类型)
/// </summary>
/// <param name="ggcp">spatialite类型多面</param>
/// <param name="mpoly">silly类型多多面</param>
/// <returns></returns>
static bool gaiageo_to_silly_multi_poly(gaiaGeomCollPtr ggcp, silly_multi_poly &mpoly)
{
    gaiaPolygonPtr poly = ggcp->FirstPolygon;
    while (poly)
    {
        silly_poly sillyPoly;
        // 外环
        silly_ring outerRing;
        gaiaRingPtr exteriorRing = poly->Exterior;
        for (int i = 0; i < exteriorRing->Points; i++)
        {
            double x = 0.0, y = 0.0;
            gaiaGetPoint(exteriorRing->Coords, i, &x, &y);
            outerRing.points.push_back(silly_point(x, y));
        }
        sillyPoly.outer_ring = outerRing;
        // 内环
        int interNum = poly->NumInteriors;
        for (int n = 0; n < interNum; n++)
        {
            gaiaRingPtr interiorRing = poly->Interiors;
            while (interiorRing)
            {
                silly_ring innerRing;
                for (int j = 0; j < interiorRing->Points; j++)
                {
                    double x = 0.0, y = 0.0;
                    gaiaGetPoint(interiorRing->Coords, j, &x, &y);
                    innerRing.points.push_back(silly_point(x, y));
                }
                sillyPoly.inner_rings.push_back(innerRing);
                interiorRing = interiorRing->Next;
            }
        }
        mpoly.push_back(sillyPoly);
        poly = poly->Next;
    }
    return true;
}

/// <summary>
/// 将silly_geo中的类类型转换为spatialite中的类型
/// </summary>
/// <param name="gc">包含silly_geo类型的结构体</param>
/// <param name="ggcp">spatialite的矢量类型</param>
/// <returns></returns>
bool geo_silly_to_spatialite(const silly_geo_coll &gc, gaiaGeomCollPtr &ggcp)
{
    bool status = false;
    switch (gc.m_type)
    {
        case enum_geometry_type::egtPoint:
            status = silly_point_to_gaiageo(gc.m_point, ggcp);
            break;
        case enum_geometry_type::egtMultiPoint:
            status = silly_multi_point_to_gaiageo(gc.m_m_points, ggcp);
            break;
        case enum_geometry_type::egtLineString:
            status = silly_line_to_gaiageo(gc.m_line, ggcp);
            break;
        case enum_geometry_type::egtMultiLineString:
            status = silly_multi_silly_line_to_gaiageo(gc.m_m_lines, ggcp);
            break;
        case enum_geometry_type::egtPolygon:
            status = silly_poly_to_gaiageo(gc.m_poly, ggcp);
            break;
        case enum_geometry_type::egtMultiPolygon:
            status = silly_multi_poly_to_gaiageo(gc.m_m_polys, ggcp);
            break;
        default:
            break;
    }
    return status;
}

// 检测gaiaGeometryType(gaiaGeomCollPtr geom)函数检测gaiaGeomCollPtr类型
int check_geom_type(int type)
{
    enum_geometry_type result;
    switch (type)
    {
        case GAIA_POINT:
            result = enum_geometry_type::egtPoint;
            break;
        case GAIA_LINESTRING:
            result = enum_geometry_type::egtLineString;
            break;
        case GAIA_POLYGON:
            result = enum_geometry_type::egtPolygon;
            break;
        case GAIA_MULTIPOINT:
            result = enum_geometry_type::egtMultiPoint;
            break;
        case GAIA_MULTILINESTRING:
            result = enum_geometry_type::egtMultiLineString;
            break;
        case GAIA_MULTIPOLYGON:
            result = enum_geometry_type::egtMultiPolygon;
            break;
        default:
            result = enum_geometry_type::egtInvalid;
    }
    return static_cast<int>(result);
}

/// <summary>
/// 将spatialite中的类型转换为silly_geo
/// </summary>
/// <param name="ggcp">spatialite的矢量类型</param>
/// <param name="gc">包含silly_geo类型的结构体</param>
/// <returns></returns>
bool geo_spatialite_to_silly(const gaiaGeomCollPtr &ggcp, silly_geo_coll &gc)
{
    bool status = false;
    // TODO: 下面这个函数在做什么事情
    int type = check_geom_type(gaiaGeometryType(ggcp));
    /*
    switch (type)
    {
        case enum_geometry_type::egtPoint:
            status = gaiageo_to_silly_point(ggcp, gc.m_point);
            gc.m_type = enum_geometry_type::egtPoint;
            break;
        case enum_geometry_type::egtMultiPoint:
            status = gaiageo_to_silly_multi_point(ggcp, gc.m_m_points);
            gc.m_type = enum_geometry_type::egtMultiPoint;
            break;
        case enum_geometry_type::egtLineString:
            status = gaiageo_to_silly_line(ggcp, gc.m_line);
            gc.m_type = enum_geometry_type::egtLineString;
            break;
        case enum_geometry_type::egtMultiLineString:
            status = gaiageo_to_silly_multi_line(ggcp, gc.m_m_lines);
            gc.m_type = enum_geometry_type::egtMultiLineString;
            break;
        case enum_geometry_type::egtPolygon:
            status = gaiageo_to_silly_poly(ggcp, gc.m_poly);
            gc.m_type = enum_geometry_type::egtPolygon;
            break;
        case enum_geometry_type::egtMultiPolygon:
            status = gaiageo_to_silly_multi_poly(ggcp, gc.m_m_polys);
            gc.m_type = enum_geometry_type::egtMultiPolygon;
            break;
        default:
            break;
    }*/
    return status;
}

//////////////////////////////////////////////////////////////////////////////////////////
/////  silly_spatialite 类的主体实现
//////////////////////////////////////////////////////////////////////////////////////////

bool silly_spatialite::initialize(const std::string &db_path)
{
    if (m_is_init)  // 已经初始化
    {
        return m_is_init;
    }

    // 打开数据库连接
    int rc;
    rc = sqlite3_open_v2(db_path.c_str(), &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK || !m_db)
    {
        sqlite3_close(m_db);
        return m_is_init;
    }

    m_sp_cache = spatialite_alloc_connection();
#ifdef NDEBUG
    spatialite_init_ex(m_db, m_sp_cache, 0);
#else
    // TODO: 检查这里设置为1后是不是有输出信息
    spatialite_init_ex(m_db, m_sp_cache, 1);
#endif
    m_is_init = true;
    return m_is_init;
}

void silly_spatialite::destory()
{
    if (m_is_init)
    {
        sqlite3_close(m_db);
        spatialite_cleanup_ex(m_sp_cache);
        spatialite_shutdown();
        m_is_init = false;
    }
}

silly_spatialite::~silly_spatialite()
{
    destory();
}

bool silly_spatialite::create_table(const std::string &sql)
{
    bool result = false;
    int rc;
    // const char* sql_creat = "CREATE TABLE IF NOT EXISTS spatial_data (id INTEGER PRIMARY KEY, geom BLOB);";
    rc = sqlite3_exec(m_db, sql.c_str(), 0, 0, 0);
    if (SQLITE_OK == rc)
    {
        result = true;
    }
    else
    {
#ifdef _DEBUG
        std::cout << "ERROR: Create table failed: " << sqlite3_errmsg(m_db) << std::endl;
#endif
    }
    return result;
}

int silly_spatialite::insert_geo(const std::vector<silly_geo_coll> &gc, const std::string &sql, int bind_index)
{
    int affect_rows = 0;
    if (!m_is_init)
    {
#ifdef _DEBUG
        std::cout << "ERROR: SpatiaLite environment not initialized \nPlease execute the silly_spatialite::initialize() first  " << std::endl;
#endif
        return affect_rows;
    }
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
        sqlite3_close(m_db);
        return affect_rows;
    }

    // TODO: 循环中,如果有一个矢量插入有问题应该怎么处理
    for (const auto &g : gc)
    {
        unsigned char *blob;
        int blob_size;
        gaiaGeomCollPtr tmp_ggcp = gaiaAllocGeomColl();
        if (!geo_silly_to_spatialite(g, tmp_ggcp))
        {
            gaiaFreeGeomColl(tmp_ggcp);
            continue;
        }
        // TODO: 确认这个地方 后面是否需要释放内存
        // 将此几何图形转换为 SpatiaLite BLOB 格式
        gaiaToSpatiaLiteBlobWkb(tmp_ggcp, &blob, &blob_size);
        gaiaFreeGeomColl(tmp_ggcp);
        // 绑定geomColl到SQL语句的参数
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
        // TODO: 理解这里绑定1的含义,表里面多个字段还能这么用吗;
        // TODO: 如果这里没有执行成功,free会执行吗
        rc = sqlite3_bind_blob(stmt, bind_index, blob, blob_size, free);
        if (rc != SQLITE_OK)
        {
            printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
            continue;
        }
        // 执行SQL语句并提交事务
        rc = sqlite3_step(stmt);
        if (SQLITE_DONE == rc || SQLITE_ROW == rc)
        {
            affect_rows += sqlite3_changes(m_db);
        }
        else
        {
            printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
            continue;
        }
    }
    // TODO:获取提交之后有几条成功了
    sqlite3_finalize(stmt);
    return affect_rows;
}

int silly_spatialite::select_geo(std::vector<silly_geo_coll> &gc, const std::string &sql)
{
    int affect_rows = 0;
    if (!m_is_init)
    {
#ifdef _DEBUG
        std::cout << "ERROR: SpatiaLite environment not initialized \nPlease execute the silly_spatialite::initialize() first  " << std::endl;
#endif
        return affect_rows;
    }
    sqlite3_stmt *stmt;
    // std::string selsql = "SELECT geom FROM spatial_data;";
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
        return affect_rows;
    }
    // 执行查询
    while (SQLITE_ROW == (rc = sqlite3_step(stmt)))
    {
        // 获取结果中的几何数据
        const unsigned char *blob = static_cast<const unsigned char *>(sqlite3_column_blob(stmt, 0));
        int blob_size = sqlite3_column_bytes(stmt, 0);
        // 使用SpatiaLite函数将几何数据转换为gaiaGeomCollPtr类型
        gaiaGeomCollPtr geomColl = gaiaFromSpatiaLiteBlobWkb(blob, blob_size);
        if (geomColl == nullptr)
        {
            gaiaFreeGeomColl(geomColl);
            std::cout << "Unable to convert Blob data to gaiaGeomCollPtr type " << std::endl;
            continue;
        }
        // gaiaGeomCollPtr类型转化为silly_geo_coll
        silly_geo_coll temp_gc;
        if (!geo_spatialite_to_silly(geomColl, temp_gc))
        {
            std::cout << "Unable to convert gaiaGeomCollPtr data to silly_geo_coll type " << std::endl;
            gaiaFreeGeomColl(geomColl);
            continue;
        }
        gc.push_back(temp_gc);
        gaiaFreeGeomColl(geomColl);
        affect_rows++;
    }
    if (SQLITE_DONE == rc)
    {
        sqlite3_finalize(stmt);
        return affect_rows;
    }
    else
    {
        std::cout << "sqlite3_step() error: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return affect_rows;
    }
}

int silly_spatialite::remove_geo(const std::string &sql)
{
    int affect_rows = 0;
    if (!m_is_init)
    {
#ifdef _DEBUG
        std::cout << "ERROR: SpatiaLite environment not initialized \nPlease execute the silly_spatialite::initialize() first  " << std::endl;
#endif
        return affect_rows;
    }
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
        sqlite3_close(m_db);
        return affect_rows;
    }
    if (SQLITE_DONE == sqlite3_step(stmt))
    {
        affect_rows += sqlite3_changes(m_db);
        sqlite3_finalize(stmt);
        return affect_rows;
    }
    else
    {
        printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
        sqlite3_finalize(stmt);
        return affect_rows;
    }
}

int silly_spatialite::modify_geo(const silly_geo_coll &gc, const std::string &sql, int bind_index)
{
    int affect_rows = 0;
    if (!m_is_init)
    {
#ifdef _DEBUG
        std::cout << "ERROR: SpatiaLite environment not initialized \nPlease execute the silly_spatialite::initialize() first  " << std::endl;
#endif
        return affect_rows;
    }
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        std::cout << "ERROR:" << __FILE__ << " line: " << __LINE__ << " : " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        return affect_rows;
    }

    unsigned char *blob;
    int blob_size;
    gaiaGeomCollPtr tmp_ggcp = gaiaAllocGeomColl();
    if (!geo_silly_to_spatialite(gc, tmp_ggcp))
    {
        gaiaFreeGeomColl(tmp_ggcp);
#ifdef _DEBUG
        std::cout << "ERROR: modify_geo : geo_silly_to_spatialite convert fail " << std::endl;
#endif
    }
    gaiaToSpatiaLiteBlobWkb(tmp_ggcp, &blob, &blob_size);
    gaiaFreeGeomColl(tmp_ggcp);
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    rc = sqlite3_bind_blob(stmt, bind_index, blob, blob_size, free);
    if (rc != SQLITE_OK)
    {
        std::cout << "WARN:" << __FILE__ << " line: " << __LINE__ << ": " << sqlite3_errmsg(m_db) << std::endl;
    }
    if (SQLITE_DONE == sqlite3_step(stmt))
    {
        affect_rows += sqlite3_changes(m_db);
        sqlite3_finalize(stmt);
        return affect_rows;
    }
    else
    {
        printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
        sqlite3_finalize(stmt);
        return affect_rows;
    }
}
