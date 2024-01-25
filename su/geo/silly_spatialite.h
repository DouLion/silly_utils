//
// Created by dell on 2023/10/24.
//

#ifndef SILLY_UTILS_SILLY_SPATIALITE_H
#define SILLY_UTILS_SILLY_SPATIALITE_H
#include <iostream>
#include <string.h>
#include <geo/silly_geo_coll.h>
#include <sqlite3.h>
#include <mutex>

class silly_spatialite
{
  public:
    silly_spatialite() = default;
    ~silly_spatialite();

    /// <summary>
    /// 初始化spatialite和sqlite3环境
    /// </summary>
    /// <param name="db_path"></param>
    /// <returns></returns>
    bool initialize(const std::string &db_path);

    /// <summary>
    /// 释放spatialite和sqlite3环境
    /// </summary>
    void destory();

    /// <summary>
    /// 创建表
    /// </summary>
    /// <param name="sql">创建表的sql语句</param>
    /// <returns></returns>
    bool create_table(const std::string &sql);

    /// <summary>
    /// 插入矢量对象
    /// </summary>
    /// <param name="gc">如果gc为空则只执行sql</param>
    /// <param name="sql"></param>
    /// <param name="bind_index">gc数据绑定到sql语句中占位符的位置,默认是1</param>
    /// <returns></returns>
    int insert_geo(const std::vector<silly_geo_coll> &gc, const std::string &sql, int bind_index = 1);

    /// <summary>
    /// 获取矢量对象
    /// </summary>
    /// <param name="gc"></param>
    /// <param name="sql"></param>
    /// <returns>几条数据收到影响</returns>
    int select_geo(std::vector<silly_geo_coll> &gc, const std::string &sql);

    /// <summary>
    /// 修改矢量对象
    /// </summary>
    /// <param name="gc">如果gc为空则只执行sql</param>
    /// <param name="sql"></param>
    /// <param name="bind_index">gc数据绑定到sql语句中占位符的位置,默认是1</param>
    /// <returns>几条数据收到影响</returns>
    int modify_geo(const silly_geo_coll &gc, const std::string &sql, int bind_index = 1);

    /// <summary>
    /// 移除矢量对象
    /// </summary>
    /// <param name="gc">如果gc为空则只执行sql</param>
    /// <param name="sql"></param>
    /// <returns>几条数据收到影响</returns>
    int remove_geo(const std::string &sql);

  private:
    sqlite3 *m_db{nullptr};
    void *m_sp_cache{nullptr};
    std::mutex m_mutex;
    /// 全局判断类内对象是否已经初始化
    bool m_is_init{false};
};

#endif  // SILLY_UTILS_SILLY_SPATIALITE_H
