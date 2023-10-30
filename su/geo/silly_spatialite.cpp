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
/// �������ͽ�silly_geoתΪspatialite�ڵ�ʸ������
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

// TODO: ����silly_point_to_gaiageo���������Ͳ�������
/// <summary>
/// �������ͽ�spatialite�ڵ�ʸ������תΪsilly_geo
/// </summary>
/// <param name="ggcp"></param>
/// <param name=""></param>
/// <returns></returns>

static bool gaiageo_to_silly_point(gaiaGeomCollPtr ggcp, silly_point& point)
{

}

/// <summary>
/// ��silly_geo�е�������ת��Ϊspatialite�е�����
/// </summary>
/// <param name="gc"></param>
/// <param name="ggcp"></param>
/// <returns></returns>
bool geo_silly_to_spatialite(const geo_collection& gc, gaiaGeomCollPtr ggcp)
{
	bool status = false;
	// TODO: ������������
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

// TODO: ����geo_silly_to_spatialiteʵ��
/// <summary>
/// ��spatialite�е�����ת��Ϊsilly_geo
/// </summary>
/// <param name="ggcp"></param>
/// <param name="gc"></param>
/// <returns></returns>
bool geo_spatialite_to_silly(const gaiaGeomCollPtr& ggcp, geo_collection& gc)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
/////  silly_spatialite �������ʵ��
//////////////////////////////////////////////////////////////////////////////////////////

bool silly_spatialite::initialize(const std::string& db_path)
{
	if (m_is_init)
	{
		m_is_init = true;
		return m_is_init;
	}
	int rc;
	// �����ݿ�����
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
	// TODO: �����������Ϊ1���ǲ����������Ϣ
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
	
	// TODO: ѭ����,�����һ��ʸ������������Ӧ����ô����
	for (const auto& g : gc)
	{
		unsigned char* blob;
		int blob_size;
		gaiaGeomCollPtr tmp_ggcp = nullptr;
		if (!geo_silly_to_spatialite(g, tmp_ggcp))
		{
			continue;
		}

		// TODO: ȷ������ط� �����Ƿ���Ҫ�ͷ��ڴ�
		/* ���˼���ͼ��ת��Ϊ SpatiaLite BLOB ��ʽ */
		gaiaToSpatiaLiteBlobWkb(tmp_ggcp, &blob, &blob_size);
		// ��geomColl��SQL���Ĳ���
		sqlite3_reset(stmt);
		sqlite3_clear_bindings(stmt);
		// TODO: ��������1�ĺ���,���������ֶλ�����ô����; 
		// TODO: �������û��ִ�гɹ�,free��ִ����
		rc = sqlite3_bind_blob(stmt, 1, blob, blob_size, free);
		if (rc != SQLITE_OK)
		{
			printf("warn_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
		}
	}

	// ִ��SQL��䲢�ύ����
	rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE || rc == SQLITE_ROW)
	{
		printf("ok");
	}
	else
	{
		printf("err_%s_line%d: %s", __FILE__, __LINE__, sqlite3_errmsg(m_db));
	}
	// TODO:��ȡ�ύ֮���м����ɹ���
	affect_rows = 0;// ���ú���������������
	
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
