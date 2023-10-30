//
// Created by dell on 2023/10/24.
//

#include "silly_spatialite.h"
#include <geo/silly_geo.h>
#include <geos_c.h>
#include <sqlite3.h>
#include <spatialite.h>
#include <spatialite/gaiageo.h>

/// <summary>
/// 区分类型将silly_geo转为spatialite内的矢量类型
/// </summary>
/// <param name="point"></param>
/// <param name="ggcp"></param>
/// <returns></returns>
static bool silly_point_to_gaiageo(const silly_point& point, gaiaGeomCollPtr ggcp)
{
	return false;
}

static bool silly_multi_point_to_gaiageo(const silly_multi_point& mpoint, gaiaGeomCollPtr ggcp)
{
	return false;
}

static bool silly_line_to_gaiageo(const silly_line& line, gaiaGeomCollPtr ggcp)
{
	return false;
}

static bool silly_multi_silly_line_to_gaiageo(const silly_multi_silly_line& mline, gaiaGeomCollPtr ggcp)
{
	return false;
}

static bool silly_poly_to_gaiageo(const silly_poly& poly, gaiaGeomCollPtr ggcp)
{
	return false;
}

static bool silly_multi_poly_to_gaiageo(const silly_multi_poly& mpoly, gaiaGeomCollPtr ggcp)
{
	return false;
}

// TODO: 参照silly_point_to_gaiageo将其他类型补充完整
/// <summary>
/// 区分类型将spatialite内的矢量类型转为silly_geo
/// </summary>
/// <param name="ggcp"></param>
/// <param name=""></param>
/// <returns></returns>

static bool gaiageo_to_silly_point(gaiaGeomCollPtr ggcp, silly_point& point)
{

}

/// <summary>
/// 将silly_geo中的类类型转换为spatialite中的类型
/// </summary>
/// <param name="gc"></param>
/// <param name="ggcp"></param>
/// <returns></returns>
bool geo_silly_to_spatialite(const geo_collection& gc, gaiaGeomCollPtr ggcp)
{
	bool status = false;
	// TODO: 补充其他类型
	switch (gc.m_type)
	{
	case enum_geometry_types::ePoint:
		status = silly_point_to_gaiageo(gc.m_point, ggcp);
		break;
	case enum_geometry_types::eLineString:
		status = silly_line_to_gaiageo(gc.m_line, ggcp);
		break;
	case enum_geometry_types::eMultiPolygon:
		status = silly_multi_poly_to_gaiageo(gc.m_m_poly, ggcp);
		break;
	default:
		break;
	}
	
	return status;
}

// TODO: 参照geo_silly_to_spatialite实现
/// <summary>
/// 将spatialite中的类型转换为silly_geo
/// </summary>
/// <param name="ggcp"></param>
/// <param name="gc"></param>
/// <returns></returns>
bool geo_spatialite_to_silly(const gaiaGeomCollPtr& ggcp, geo_collection& gc)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
/////  silly_spatialite 类的主体实现
//////////////////////////////////////////////////////////////////////////////////////////

bool silly_spatialite::initialize(const std::string& db_path)
{
	if (m_is_init)
	{
		m_is_init = true;
		return m_is_init;
	}
	int rc;
	// 打开数据库连接
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
	
	return m_is_init;
}

void silly_spatialite::destory()
{
	if (m_is_init)
	{
		spatialite_cleanup_ex(m_sp_cache);
		sqlite3_close(m_db);
		m_is_init = false;
	}
}


silly_spatialite::~silly_spatialite()
{
	destory();
}

bool silly_spatialite::create_table(const std::string& sql)
{
	return false;
}


int silly_spatialite::insert_geo(const std::vector<geo_collection>& gc, const std::string& sql)
{
	int affect_rows = 0;
	if (!m_is_init)
	{
		return affect_rows;
	}
	sqlite3_stmt* stmt;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
	    sqlite3_close(m_db);
	    return affect_rows;
	}
	
	// TODO: 循环中,如果有一个矢量插入有问题应该怎么处理
	for (const auto& g : gc)
	{
		unsigned char* blob;
		int blob_size;
		gaiaGeomCollPtr tmp_ggcp = nullptr;
		if (!geo_silly_to_spatialite(g, tmp_ggcp))
		{
			continue;
		}

		// TODO: 确认这个地方 后面是否需要释放内存
		/* 将此几何图形转换为 SpatiaLite BLOB 格式 */
		gaiaToSpatiaLiteBlobWkb(tmp_ggcp, &blob, &blob_size);
		// 绑定geomColl到SQL语句的参数
		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);
		// TODO: 理解这里绑定1的含义,表里面多个字段还能这么用吗; 
		// TODO: 如果这里没有执行成功,free会执行吗
		rc = sqlite3_bind_blob(stmt, 1, blob, blob_size, free);
		if (rc != SQLITE_OK)
		{
			printf("warn_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
		}
	}

	// 执行SQL语句并提交事务
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE || rc == SQLITE_ROW)
	{
		printf("ok");
	}
	else
	{
		printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
	}
	// TODO:获取提交之后有几条成功了
	affect_rows = 0;// 调用函数或者其他方法
	
	return affect_rows;
}

int silly_spatialite::remove_geo(const std::vector<geo_collection>& gc, const std::string& sql)
{
	return 0;
}

int silly_spatialite::modify_geo(const std::vector<geo_collection>& gc, const std::string& sql)
{
	return 0;
}

bool silly_spatialite::select_geo(std::vector<geo_collection>& gc, const std::string& sql)
{
	return false;
}
