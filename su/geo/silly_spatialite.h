//
// Created by dell on 2023/10/24.
//

#ifndef SILLY_UTILS_SILLY_SPATIALITE_H
#define SILLY_UTILS_SILLY_SPATIALITE_H
#include <iostream>
#include <string.h>
#include <geo/silly_geo.h>
#include <sqlite3.h>
#include <mutex>

struct geo_collection
{
	enum_geometry_types m_type{ enum_geometry_types::eInvalid };
	silly_point m_point;
	silly_line m_line;
	silly_multi_poly m_m_poly;

	geo_collection& operator=(const geo_collection& other)
	{
		this->m_type = other.m_type;
		this->m_point = other.m_point;
		this->m_line = other.m_line;
		this->m_m_poly = other.m_m_poly;
		return *this;
	}

};

class silly_spatialite
{
public:
	silly_spatialite() = default;
	~silly_spatialite();

	bool initialize(const std::string& db_path);

	void destory();

	/// <summary>
	/// 创建表
	/// </summary>
	/// <param name="sql"></param>
	/// <returns></returns>
	bool create_table(const std::string& sql);

	/// <summary>
	/// 插入矢量对象
	/// </summary>
	/// <param name="gc">如果gc为空则只执行sql</param>
	/// <param name="sql"></param>
	/// <returns>几条数据收到影响</returns>
	int insert_geo(const std::vector<geo_collection>& gc, const std::string& sql);

	/// <summary>
	/// 移除矢量对象
	/// </summary>
	/// <param name="gc">如果gc为空则只执行sql</param>
	/// <param name="sql"></param>
	/// <returns>几条数据收到影响</returns>
	int remove_geo(const std::vector<geo_collection>& gc, const std::string& sql);

	/// <summary>
	/// 修改矢量对象
	/// </summary>
	/// <param name="gc">如果gc为空则只执行sql</param>
	/// <param name="sql"></param>
	/// <returns>几条数据收到影响</returns>
	int modify_geo(const std::vector<geo_collection>& gc, const std::string& sql);

	/// <summary>
	/// 获取矢量对象
	/// </summary>
	/// <param name="gc"></param>
	/// <param name="sql"></param>
	/// <returns>如果gc为空,根据True 或者 False 判断是应为没有合适条件的数据,还是中间执行出错</returns>
	bool select_geo(std::vector<geo_collection>& gc, const std::string& sql);

private:
	sqlite3*				m_db{ nullptr };
	void*					m_sp_cache{ nullptr };
	std::mutex				m_mutex;
	/// 全局判断类内对象是否已经初始化
	bool					m_is_init{ false };
};

#endif //SILLY_UTILS_SILLY_SPATIALITE_H
