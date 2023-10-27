//
// Created by dell on 2023/10/24.
//

#include "silly_spatialite.h"
#if IS_WIN32
void silly_spatialite::initSpatialite()
{
    void* cache = spatialite_alloc_connection();
    spatialite_init_ex(db, cache, 0);
}

void silly_spatialite::destorySpatialite()
{
    spatialite_cleanup_ex(cache);
}

bool silly_spatialite::openDatabase(const char* dbName)
{
    int rc;
    // 打开数据库连接
    rc = sqlite3_open_v2(dbName, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK) 
    {
        std::cout << "Failed to open database 连接: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    return true;
}

void silly_spatialite::closeDatabase()
{
    // 关闭数据库连接
    sqlite3_close(db);
}


bool silly_spatialite::createDatabase(const char* dbName)
{
    sqlite3* db;
    int rc;
    // 打开数据库连接
    rc = sqlite3_open_v2(dbName, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK) {
        std::cout << "Failed to open database 连接: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    void* cache = spatialite_alloc_connection();
    spatialite_init_ex(db, cache, 0);
    
    //// 初始化空间元数据
    //rc = sqlite3_exec(db, "SELECT InitSpatialMetadata(1)", 0, 0, 0);
    //if (rc != SQLITE_OK) {
    //    std::cout << "无法初始化空间元数据: " << sqlite3_errmsg(db) << std::endl;
    //    sqlite3_close(db);
    //    return false;
    //}
    // 创建包含几何字段的表
    //const char* createTableQuery = "CREATE TABLE spatial_data (PK INTEGER NOT NULL PRIMARY KEY)";
    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS spatial_data (id INTEGER PRIMARY KEY, geom BLOB);";
    rc = sqlite3_exec(db, createTableQuery, 0, 0, 0);
    if (rc != SQLITE_OK) {
        std::cout << "无法创建表: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }



    spatialite_cleanup_ex(cache);
    // 关闭数据库连接
    sqlite3_close(db);

    // 操作成功，返回true
    return true;
}



bool silly_spatialite::insertSpatialData(const char* dbName, gaiaGeomCollPtr geomColl)
{
    int rc;

    // //打开数据库连接
    sqlite3* db;
    rc = sqlite3_open_v2(dbName, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc) 
    {
        std::cout << "Failed to open database : " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    void* cache = spatialite_alloc_connection();
    spatialite_init_ex(db, cache, 0);

    // 准备插入数据的SQL语句
    const char* sql = "INSERT INTO spatial_data (geom) VALUES (?);";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        std::cout << "INSERT SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    unsigned char* blob;
    int blob_size;
    /* 将此几何图形转换为 SpatiaLite BLOB 格式 */
    gaiaToSpatiaLiteBlobWkb(geomColl, &blob, &blob_size);
    // 绑定geomColl到SQL语句的参数
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
    rc = sqlite3_bind_blob(stmt, 1, blob, blob_size, free);
    if (rc != SQLITE_OK) 
    {
        std::cout << "Failed to bind geomColl: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        spatialite_cleanup_ex(cache);
        sqlite3_close(db);
        return true;
    }

    // 执行SQL语句并提交事务
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE || rc == SQLITE_ROW) 
    {
        std::cout << "矢量数据存储成功！" << std::endl;
        sqlite3_close(db);
        spatialite_cleanup_ex(cache);
        sqlite3_finalize(stmt);
        return true;
    }
    else 
    {
        std::cout << "无法存储矢量数据: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        spatialite_cleanup_ex(cache);
        sqlite3_finalize(stmt);
        return false;
    }
}


bool silly_spatialite::insertSpatialData(const char* dbName, std::vector<gaiaGeomCollPtr> geomColls)
{
    // 打开数据库连接
    sqlite3* db;
    int rc = sqlite3_open_v2(dbName, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (rc != SQLITE_OK) 
    {
        std::cout << "Failed to open database : " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    // 准备插入数据的SQL语句
    const char* sql = "INSERT INTO spatial_data (geom) VALUES (?);";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (rc != SQLITE_OK) {
        std::cout << "INSERT SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    // 绑定geomColl到SQL语句的参数
    for (const auto& geomColl : geomColls)
    {
        unsigned char* blob;
        int blob_size;
        /* 将此几何图形转换为 SpatiaLite BLOB 格式 */
        gaiaToSpatiaLiteBlobWkb(geomColl, &blob, &blob_size);
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
        rc = sqlite3_bind_blob(stmt, 1, blob, blob_size, free);
        if (rc != SQLITE_OK) 
        {
            std::cout << "Failed to bind geomColl: " << sqlite3_errmsg(db) << std::endl;
            continue;
        }
        // 执行SQL语句
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) 
        {
            std::cout << "无法存储矢量数据: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    // 关闭数据库连接
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return true;
}


bool silly_spatialite::selectSpatialData(const char* dbName, std::vector<gaiaGeomCollPtr>& geomColls)
{
    int ret;
    sqlite3* db;
    sqlite3_stmt* stmt;

    // 打开数据库连接
    ret = sqlite3_open_v2(dbName, &db, SQLITE_OPEN_READONLY, NULL);
    if (ret != SQLITE_OK)
    {
        std::cout << "Failed to open database : " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // 初始化SpatiaLite
    void* cache = spatialite_alloc_connection();
    spatialite_init_ex(db, cache, 0);

    // 准备查询语句
    const char* sql = "SELECT geom FROM spatial_data;";
    ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
    if (ret != SQLITE_OK)
    {
        std::cout << "查询SQL错误: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // 执行查询
    while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        // 获取结果中的几何数据
        const unsigned char* blob = static_cast<const unsigned char*>(sqlite3_column_blob(stmt, 0));
        int blob_size = sqlite3_column_bytes(stmt, 0);
        // 使用SpatiaLite函数将几何数据转换为gaiaGeomCollPtr类型
        gaiaGeomCollPtr geomColl = gaiaFromSpatiaLiteBlobWkb(blob, blob_size);
        if (geomColl == nullptr)
        {
            std::cout << "无法将几何数据转换为gaiaGeomCollPtr类型" << std::endl;
            continue;
        }
        geomColls.push_back(geomColl);
    }

    if (ret != SQLITE_DONE)
    {
        std::cout << "无法检索矢量数据: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        spatialite_cleanup_ex(cache);
        return false;
    }

    // 关闭数据库连接
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // 释放内存
    spatialite_cleanup_ex(cache);

    return true;
}




//bool silly_spatialite::selectSpatialData(const char* dbName, gaiaGeomCollPtr& geomColl)
//{
//    int ret;
//    sqlite3* handle;
//    sqlite3_stmt* stmt;
//    const void* blob;
//    int blob_size;
//
//    // 打开数据库连接
//    ret = sqlite3_open_v2(dbName, &handle, SQLITE_OPEN_READONLY, NULL);
//    if (ret != SQLITE_OK)
//    {
//        std::cout << "Failed to open database : " << sqlite3_errmsg(handle) << std::endl;
//        sqlite3_close(handle);
//        return false;
//    }
//
//    // 初始化SpatiaLite
//    void* cache = spatialite_alloc_connection();
//    spatialite_init_ex(handle, cache, 0);
//
//    // 准备查询语句
//    const char* sql = "SELECT geom FROM spatial_data;";
//    ret = sqlite3_prepare_v2(handle, sql, strlen(sql), &stmt, NULL);
//    if (ret != SQLITE_OK)
//    {
//        std::cout << "查询SQL错误: " << sqlite3_errmsg(handle) << std::endl;
//        sqlite3_close(handle);
//        return false;
//    }
//
//    // 执行查询
//    ret = sqlite3_step(stmt);
//    if (ret == SQLITE_ROW)
//    {
//        // 获取结果中的几何数据
//        const unsigned char* blob = static_cast<const unsigned char*>(sqlite3_column_blob(stmt, 0));
//        int blob_size = sqlite3_column_bytes(stmt, 0);
//        // 使用SpatiaLite函数将几何数据转换为gaiaGeomCollPtr类型
//        geomColl = gaiaFromSpatiaLiteBlobWkb(blob, blob_size);
//        if (geomColl == nullptr)
//        {
//            std::cout << "无法将几何数据转换为gaiaGeomCollPtr类型" << std::endl;
//            sqlite3_finalize(stmt);
//            sqlite3_close(handle);
//            return false;
//        }
//
//        std::cout << "矢量数据检索成功！" << std::endl;
//    }
//    else if (ret == SQLITE_DONE)
//    {
//        std::cout << "没有找到矢量数据" << std::endl;
//        sqlite3_finalize(stmt);
//        sqlite3_close(handle);
//        return false;
//    }
//    else
//    {
//        std::cout << "无法检索矢量数据: " << sqlite3_errmsg(handle) << std::endl;
//        sqlite3_finalize(stmt);
//        sqlite3_close(handle);
//        return false;
//    }
//
//    // 关闭数据库连接
//    sqlite3_finalize(stmt);
//    sqlite3_close(handle);
//
//    // 释放内存
//    spatialite_cleanup_ex(cache);
//
//    return true;
//
//}


bool silly_spatialite_selectSpatialData(const char* dbName, gaiaGeomCollPtr& geomColl)
{
    int ret;
    sqlite3* db;
    sqlite3_stmt* stmt;
    gaiaGeomCollPtr geom;
    char sql[256];
    int i;
    int ic;
    char** results;
    int n_rows;
    int n_columns;
    char* err_msg = NULL;
    int len;
    char* table_name;
    char** p_geotables = NULL;
    int n_geotables = 0;
    int row_no;
    const void* blob;
    int blob_size;
    int geom_type;

    // 打开数据库
    ret = sqlite3_open_v2(dbName, &db, SQLITE_OPEN_READONLY, NULL);
    if (ret != SQLITE_OK)
    {
        std::cout << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // 初始化SpatiaLite
    void* cache = spatialite_alloc_connection();
    spatialite_init_ex(db, cache, 0);

    // 查询地理表名称
    strcpy(sql, "SELECT DISTINCT geom FROM spatial_data");
    ret = sqlite3_get_table(db, sql, &results, &n_rows, &n_columns, &err_msg);
    if (ret != SQLITE_OK)
    {
        std::cout << "Failed to retrieve table names: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return false;
    }

    if (n_rows > 1)
    {
        // 分配动态指针数组以存储地理表名称
        n_geotables = n_rows;
        p_geotables = reinterpret_cast<char**>(malloc(sizeof(char*) * n_geotables));

        for (i = 1; i <= n_rows; i++)
        {
            table_name = results[i * n_columns + 0];
            len = strlen(table_name);
            p_geotables[i - 1] = reinterpret_cast<char*>(malloc(len + 1));
            strcpy(p_geotables[i - 1], table_name);
        }

        sqlite3_free_table(results);
    }

    for (i = 0; i < n_geotables; i++)
    {
        // 对于每个地理表，读取数据
        sprintf(sql, "SELECT * FROM %s", p_geotables[i]);
        ret = sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
        if (ret != SQLITE_OK)
        {
            std::cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return false;
        }
        n_columns = sqlite3_column_count(stmt);
        row_no = 0;
        while (1)
        {
            ret = sqlite3_step(stmt);
            if (ret == SQLITE_DONE)
            {
                // 没有更多的行要获取，退出循环
                break;
            }
            if (ret == SQLITE_ROW)
            {
                // 获取有效行数据
                row_no++;
                for (ic = 0; ic < n_columns; ic++)
                {
                    switch (sqlite3_column_type(stmt, ic))
                    {
                    case SQLITE_BLOB:
                        blob = sqlite3_column_blob(stmt, ic);
                        blob_size = sqlite3_column_bytes(stmt, ic);
                        geom = gaiaFromSpatiaLiteBlobWkb((const unsigned
                            char*)(blob), blob_size);
                        if (geom)
                        {
                            geomColl = geom;
                        }

                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                std::cout << "sqlite3_step() error: " << sqlite3_errmsg(db) << std::endl;
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return false;
            }
        }

        sqlite3_finalize(stmt);
    }

    // 关闭数据库
    sqlite3_close(db);

    // 释放内存
    spatialite_cleanup_ex(cache);
    for (i = 0; i < n_geotables; i++)
    {
        free(p_geotables[i]);
    }
    free(p_geotables);

    return true;
}




// 单点转换
// 将std::vector<silly_point> 转换为 gaiaGeomCollPtr 的函数
gaiaGeomCollPtr silly_spatialite::vectorToPointGeomColl(const std::vector<silly_point>& points)
{
    gaiaGeomCollPtr geo_mpt = gaiaAllocGeomColl();
    for (const silly_point& point : points)
    {
        gaiaAddPointToGeomColl(geo_mpt, point.lgtd, point.lttd);
    }
    return geo_mpt;
}
// 将 gaiaGeomCollPtr 转换为 std：：vector <silly_point>的函数
std::vector<silly_point> silly_spatialite::PointgeomCollToVector(const gaiaGeomCollPtr geo)
{
    std::vector<silly_point> points;
    gaiaPointPtr pt;
    pt = geo->FirstPoint;
    while (pt)
    {
        points.push_back(silly_point(pt->X, pt->Y));
        pt = pt->Next;
    }
    return points;
}

// 线的转换
// 从silly_line转换为GeomCollPtr (单线)
gaiaGeomCollPtr silly_spatialite::sillyLineToGeomColl(const silly_line& line)
{
    gaiaGeomCollPtr geo_ln = gaiaAllocGeomColl();
    gaiaLinestringPtr gaiaLine = gaiaAddLinestringToGeomColl(geo_ln, line.size());
    for (size_t i = 0; i < line.size(); i++)
    {
        gaiaSetPoint(gaiaLine->Coords, i, line[i].lgtd, line[i].lttd);
    }
    return geo_ln;
}
//silly_multi_silly_line转换为gaiaGeomCollPtr类型 (多线重载)
gaiaGeomCollPtr silly_spatialite::sillyLineToGeomColl(const silly_multi_silly_line& multiLine)
{
    gaiaGeomCollPtr geomColl = gaiaAllocGeomColl();
    for (const auto& line : multiLine)
    {
        gaiaLinestringPtr gaiaLine = gaiaAddLinestringToGeomColl(geomColl, line.size());
        for (size_t i = 0; i < line.size(); i++)
        {
            gaiaSetPoint(gaiaLine->Coords, i, line[i].lgtd, line[i].lttd);
        }
    }
    return geomColl;
}

//gaiaGeomCollPtr类型转换为silly_multi_silly_line (多线)
silly_multi_silly_line silly_spatialite::geomCollToMultiLine(const gaiaGeomCollPtr& geomColl)
{
    silly_multi_silly_line multiLine;

    gaiaLinestringPtr line = geomColl->FirstLinestring;
    while (line)
    {
        silly_line sillyLine;
        for (int i = 0; i < line->Points; i++)
        {
            double x, y;
            gaiaGetPoint(line->Coords, i, &x, &y);
            sillyLine.push_back(silly_point(x, y));
        }
        multiLine.push_back(sillyLine);
        line = line->Next;
    }
    return multiLine;
}


// 多面
//将silly_multi_poly转换为gaiaGeomCollPtr类型
gaiaGeomCollPtr silly_spatialite::multiPolyToGeomColl(const silly_multi_poly& multiPoly)
{
    gaiaGeomCollPtr geomColl = gaiaAllocGeomColl();
    for (const auto& poly : multiPoly)
    {
        gaiaPolygonPtr gaiaPoly = gaiaAddPolygonToGeomColl(geomColl, poly.outer_ring.points.size(), poly.inner_rings.size());
        // 添加外环
        gaiaRingPtr exteriorRing = gaiaPoly->Exterior;
        int exteriorPointNum = 0;
        for (const auto& point : poly.outer_ring.points)
        {
            gaiaSetPoint(exteriorRing->Coords, exteriorPointNum, point.lgtd, point.lttd);
            exteriorPointNum++;
        }
        // 添加内环
        int inner_num = 0;
        for (const auto& innerRing : poly.inner_rings)
        {
            gaiaRingPtr interiorRing = gaiaAddInteriorRing(gaiaPoly, inner_num, innerRing.points.size());
            int interPointNum = 0;
            for (const auto& point : innerRing.points)
            {
                gaiaSetPoint(interiorRing->Coords, interPointNum, point.lgtd, point.lttd);
                interPointNum++;
            }
            inner_num++;
        }
    }
    return geomColl;
}
// 将gaiaGeomCollPtr类型转换为silly_multi_poly
silly_multi_poly silly_spatialite::geomCollToMultiPoly(const gaiaGeomCollPtr& geomColl)
{
    silly_multi_poly multiPoly;
    gaiaPolygonPtr poly = geomColl->FirstPolygon;
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
        multiPoly.push_back(sillyPoly);
        poly = poly->Next;
    }
    return multiPoly;
}


































//int silly_spatialite::creat(std::string spa)
//{
//    // 初始化 Spatialite
//    sqlite3* db;
//    void* cache;
//    sqlite3_open_v2(spa.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
//    cache = spatialite_alloc_connection();
//    spatialite_init_ex(db, cache, 0);
//
//    // 创建一个 Spatialite 数据库
//    //sqlite3_exec(db, spa.c_str(), nullptr, nullptr, nullptr);
//     sqlite3_exec(db, "SELECT InitSpatialMetadata(1)", nullptr, nullptr, nullptr);
//
//    // 创建几何对象的表
//    sqlite3_exec(db, "CREATE TABLE geom_table (id INTEGER PRIMARY KEY AUTOINCREMENT, geom GEOMETRY)", nullptr, nullptr, nullptr);
//
//    // 创建四个点
//    sqlite3_exec(db, "INSERT INTO geom_table (geom) VALUES (GeomFromText('POINT(0.0 0.0)'))", nullptr, nullptr, nullptr);
//    sqlite3_exec(db, "INSERT INTO geom_table (geom) VALUES (GeomFromText('POINT(1.0 1.0)'))", nullptr, nullptr, nullptr);
//    sqlite3_exec(db, "INSERT INTO geom_table (geom) VALUES (GeomFromText('POINT(2.0 2.0)'))", nullptr, nullptr, nullptr);
//    sqlite3_exec(db, "INSERT INTO geom_table (geom) VALUES (GeomFromText('POINT(3.0 3.0)'))", nullptr, nullptr, nullptr);
//
//    // 创建一条线
//    sqlite3_exec(db, "INSERT INTO geom_table (geom) VALUES (GeomFromText('LINESTRING(0.0 0.0, 1.0 1.0, 2.0 2.0, 3.0 3.0)'))", nullptr, nullptr, nullptr);
//
//    // 创建两条多线
//    sqlite3_exec(db, "INSERT INTO geom_table (geom) VALUES (GeomFromText('MULTILINESTRING((0.0 0.0, 1.0 1.0, 2.0 2.0, 3.0 3.0),(1.0 1.0, 2.0 2.0, 3.0 3.0, 4.0 4.0))'))", nullptr, nullptr, nullptr);
//
//    // 创建两个多边形
//    sqlite3_exec(db, "INSERT INTO geom_table (geom) VALUES (GeomFromText('POLYGON((0.0 0.0, 1.0 1.0, 2.0 2.0, 0.0 0.0))'))", nullptr, nullptr, nullptr);
//    sqlite3_exec(db, "INSERT INTO geom_table (geom) VALUES (GeomFromText('POLYGON((1.0 1.0, 2.0 2.0, 3.0 3.0, 1.0 1.0))'))", nullptr, nullptr, nullptr);
//
//    // 查询并输出几何对象
//    sqlite3_exec(db, "SELECT AsText(geom) FROM geom_table", [](void* data, int argc, char** argv, char** colName) {
//        for (int i = 0; i < argc; ++i) {
//            std::cout << argv[i] << "\t";
//        }
//        std::cout << std::endl;
//        return 0;
//        }, nullptr, nullptr);
//
//    // 清理资源
//    sqlite3_close(db);
//    spatialite_cleanup_ex(cache);
//
//    return 0;
//}



int silly_spatialite::test()
{
    gaiaGeomCollPtr geo_pt = NULL;
    gaiaGeomCollPtr geo_ln = NULL;
    gaiaGeomCollPtr geo_pg = NULL;
    gaiaGeomCollPtr geo_mpt = NULL;
    gaiaGeomCollPtr geo_mln = NULL;
    gaiaGeomCollPtr geo_mpg = NULL;
    gaiaGeomCollPtr geo_coll = NULL;
    gaiaLinestringPtr line;
    gaiaPolygonPtr polyg;
    gaiaRingPtr ring;
    gaiaOutBuffer wkt;
    int ret;
    sqlite3* db;
    void* cache;

    ret = sqlite3_open_v2("E:test_sp.db", &db, SQLITE_OPEN_READONLY, NULL);
    if (ret != SQLITE_OK)
    {
        printf("cannot open '%s': %s\n", ":memory:",
            sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    cache = spatialite_alloc_connection();
    spatialite_init_ex(db, cache, 0);


#ifndef OMIT_GEOS               /* GEOS must be enabled */

    /*
    Step #1
    creating and checking a POINT Geometry
    */

    /* we'll allocate a Geometry object */
    geo_pt = gaiaAllocGeomColl();
   
    /* then we insert a POINT, directly passing its coordinates */
    gaiaAddPointToGeomColl(geo_pt, 1.5, 2.75);
   
    /* now we'll print the main attributes for this geometry */
    printf("step#1: %s\t\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_pt)),
        gaiaDimension(geo_pt), gaiaIsValid_r(cache, geo_pt));
    
    geometry_printout(geo_pt);



    /*
    Step #2
    creating and checking a LINESTRING Geometry
    */
    geo_ln = gaiaAllocGeomColl();
    /* then we insert a  LINESTRING, specifying how many vertices it contains */
    line = gaiaAddLinestringToGeomColl(geo_ln, 5);
    /*
    we've got a pointer referencing the linestring we've just inserted
    now we'll set coordinates for each vertex
    */
    gaiaSetPoint(line->Coords, 0, 1.0, 1.0);
    gaiaSetPoint(line->Coords, 1, 2.0, 1.0);
    gaiaSetPoint(line->Coords, 2, 2.0, 2.0);
    gaiaSetPoint(line->Coords, 3, 100.0, 2.0);
    gaiaSetPoint(line->Coords, 4, 100.0, 100.0);


    printf("\nstep#2: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_ln)),
        gaiaDimension(geo_ln), gaiaIsValid_r(cache, geo_ln));
    geometry_printout(geo_ln);



    /*
    Step #3
    creating and checking a POLYGON Geometry
    */
    geo_pg = gaiaAllocGeomColl();
    /*
    then we insert a  POLYGON, specifying:
    - how many vertices have to be allocated for the Exterior Ring
    - how many Interior Rings it has
    */
    polyg = gaiaAddPolygonToGeomColl(geo_pg, 5, 2);
    /*
    we've got a pointer referencing the polygon we've just inserted
    now we'll get a pointer referencing its Exterior ring
    */
    ring = polyg->Exterior;


    /* now we'll set coordinates for each Exterior ring vertex */
    gaiaSetPoint(ring->Coords, 0, 0.0, 0.0);
    gaiaSetPoint(ring->Coords, 1, 50.0, 0.0);
    gaiaSetPoint(ring->Coords, 2, 50.0, 50.0);
    gaiaSetPoint(ring->Coords, 3, 0.0, 50.0);
    /* please note: a Ring is a CLOSED figure, so last and first vertex have to be coincident */
    gaiaSetPoint(ring->Coords, 4, 0.0, 0.0);


    /*
    we'll now get a pointer referencing the FIRST interior ring,
    specifying how vertices have to be allocated
    */
    ring = gaiaAddInteriorRing(polyg, 0, 5);
    /* then setting coordinates for each Interior ring vertex */
    gaiaSetPoint(ring->Coords, 0, 40.0, 40.0);
    gaiaSetPoint(ring->Coords, 1, 41.0, 40.0);
    gaiaSetPoint(ring->Coords, 2, 41.0, 41.0);
    gaiaSetPoint(ring->Coords, 3, 40.0, 41.0);
    gaiaSetPoint(ring->Coords, 4, 40.0, 40.0);


    /*
    we'll now get a pointer referencing the SECOND interior ring,
    specifying how vertices have to be allocated
    */
    ring = gaiaAddInteriorRing(polyg, 1, 5);
    /* then setting coordinates for each Interior ring vertex */
    gaiaSetPoint(ring->Coords, 0, 30.0, 30.0);
    gaiaSetPoint(ring->Coords, 1, 31.0, 30.0);
    gaiaSetPoint(ring->Coords, 2, 31.0, 31.0);
    gaiaSetPoint(ring->Coords, 3, 30.0, 31.0);
    gaiaSetPoint(ring->Coords, 4, 30.0, 30.0);


    printf("\nstep#3: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_pg)),
        gaiaDimension(geo_pg), gaiaIsValid_r(cache, geo_pg));
    geometry_printout(geo_pg);



    /*
    Step #4
    creating and checking a MULTIPOINT Geometry
    */
    geo_mpt = gaiaAllocGeomColl();
    /* then we'll insert some POINTs */
    gaiaAddPointToGeomColl(geo_mpt, 5.0, 5.0);
    gaiaAddPointToGeomColl(geo_mpt, 15.0, 5.0);
    gaiaAddPointToGeomColl(geo_mpt, 5.0, 15.0);
    gaiaAddPointToGeomColl(geo_mpt, 25.0, 5.0);
    gaiaAddPointToGeomColl(geo_mpt, 5.0, 25.0);
    printf("\nstep#4: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_mpt)),
        gaiaDimension(geo_mpt), gaiaIsValid_r(cache, geo_mpt));
    geometry_printout(geo_mpt);



    /*
    Step #5
    creating and checking a MULTILINESTRING Geometry
    */
    geo_mln = gaiaAllocGeomColl();

    /* then we'll insert two LINESTRINGs */
    line = gaiaAddLinestringToGeomColl(geo_mln, 2);
    gaiaSetPoint(line->Coords, 0, 30.0, 10.0);
    gaiaSetPoint(line->Coords, 1, 10.0, 30.0);

    line = gaiaAddLinestringToGeomColl(geo_mln, 2);
    gaiaSetPoint(line->Coords, 0, 40.0, 50.0);
    gaiaSetPoint(line->Coords, 1, 50.0, 40.0);

    printf("\nstep#5: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_mln)),
        gaiaDimension(geo_mln), gaiaIsValid_r(cache, geo_mln));
    geometry_printout(geo_mln);



    /*
    Step #6
    creating and checking a MULTIPOLYGON Geometry
    */
    geo_mpg = gaiaAllocGeomColl();

    /* then we'll insert two POLYGONs */
    polyg = gaiaAddPolygonToGeomColl(geo_mpg, 5, 0);
    ring = polyg->Exterior;
    gaiaSetPoint(ring->Coords, 0, 60.0, 60.0);
    gaiaSetPoint(ring->Coords, 1, 70.0, 60.0);
    gaiaSetPoint(ring->Coords, 2, 70.0, 70.0);
    gaiaSetPoint(ring->Coords, 3, 60.0, 70.0);
    gaiaSetPoint(ring->Coords, 4, 60.0, 60.0);

    polyg = gaiaAddPolygonToGeomColl(geo_mpg, 5, 0);
    ring = polyg->Exterior;
    gaiaSetPoint(ring->Coords, 0, 80.0, 80.0);
    gaiaSetPoint(ring->Coords, 1, 90.0, 80.0);
    gaiaSetPoint(ring->Coords, 2, 90.0, 90.0);
    gaiaSetPoint(ring->Coords, 3, 80.0, 90.0);
    gaiaSetPoint(ring->Coords, 4, 80.0, 80.0);

    printf("\nstep#6: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_mpg)),
        gaiaDimension(geo_mpg), gaiaIsValid_r(cache, geo_mpg));
    geometry_printout(geo_mpg);



    /*
    Step #7
    creating and checking a GEOMETRYCOLLECTION Geometry
    */
    geo_coll = gaiaAllocGeomColl();

    /* then we'll insert two POINTs */
    gaiaAddPointToGeomColl(geo_coll, 100.0, 100.0);
    gaiaAddPointToGeomColl(geo_coll, 100.0, 0.0);

    /* then we'll insert two LINESTRINGs */
    line = gaiaAddLinestringToGeomColl(geo_coll, 2);
    gaiaSetPoint(line->Coords, 0, 130.0, 110.0);
    gaiaSetPoint(line->Coords, 1, 110.0, 130.0);

    line = gaiaAddLinestringToGeomColl(geo_coll, 2);
    gaiaSetPoint(line->Coords, 0, 140.0, 150.0);
    gaiaSetPoint(line->Coords, 1, 150.0, 140.0);

    /* then we'll insert two POLYGONs */
    polyg = gaiaAddPolygonToGeomColl(geo_coll, 5, 0);
    ring = polyg->Exterior;
    gaiaSetPoint(ring->Coords, 0, 160.0, 160.0);
    gaiaSetPoint(ring->Coords, 1, 170.0, 160.0);
    gaiaSetPoint(ring->Coords, 2, 170.0, 170.0);
    gaiaSetPoint(ring->Coords, 3, 160.0, 170.0);
    gaiaSetPoint(ring->Coords, 4, 160.0, 160.0);

    polyg = gaiaAddPolygonToGeomColl(geo_coll, 5, 0);
    ring = polyg->Exterior;
    gaiaSetPoint(ring->Coords, 0, 180.0, 180.0);
    gaiaSetPoint(ring->Coords, 1, 190.0, 180.0);
    gaiaSetPoint(ring->Coords, 2, 190.0, 190.0);
    gaiaSetPoint(ring->Coords, 3, 180.0, 190.0);
    gaiaSetPoint(ring->Coords, 4, 180.0, 180.0);

    printf("\nstep#7: %s\tDimension=%d IsValid=%d\n",
        geom_type(gaiaGeometryType(geo_coll)),
        gaiaDimension(geo_coll), gaiaIsValid_r(cache, geo_coll));
    geometry_printout(geo_coll);




    /*
    Step #8
    printing each geometry as Well Known Text (WKT)
    */

    printf("\nstep#8: checking WKT representations\n");

    /* first we'll get the WKT corresponding to geometry */
    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_pt);
    /* we have to check wkt is not NULL */
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        /* printing the WKT */
        printf("\n%s\n", wkt.Buffer);

        /* finally freeing the wkt temporary storage allocation */
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_ln);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_pg);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_mpt);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_mln);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_mpg);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }


    gaiaOutBufferInitialize(&wkt);
    gaiaOutWkt(&wkt, geo_coll);
    if (wkt.Error == 0 && wkt.Buffer != NULL)
    {
        printf("\n%s\n", wkt.Buffer);
        gaiaOutBufferReset(&wkt);
    }

#else
    printf("no GEOS support available: skipping any test\n");
#endif /* GEOS enabled/disabled */

    /*
    memory cleanup
    we have to destroy each object using temporary storage before exit
    */
    if (geo_pt)
        gaiaFreeGeomColl(geo_pt);
    if (geo_ln)
        gaiaFreeGeomColl(geo_ln);
    if (geo_pg)
        gaiaFreeGeomColl(geo_pg);
    if (geo_mpt)
        gaiaFreeGeomColl(geo_mpt);
    if (geo_mln)
        gaiaFreeGeomColl(geo_mln);
    if (geo_mpg)
        gaiaFreeGeomColl(geo_mpg);
    if (geo_coll)
        gaiaFreeGeomColl(geo_coll);
    sqlite3_close(db);
    spatialite_cleanup_ex(cache);
    spatialite_shutdown();
    return 0;
}


#endif