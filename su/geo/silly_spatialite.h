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
    std::string m_source;
    enum_geometry_types m_type{ enum_geometry_types::eInvalid };
    bool is_modified{ false };
    std::vector<silly_point> m_points;
    std::vector<silly_line> m_lines;
    std::vector<silly_multi_poly> m_m_polys;
    time_t 	m_last_visit;	// 最新一次的访问时间
    geo_collection& operator=(const geo_collection& other)
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

class silly_spatialite
{
public:
    silly_spatialite() = default;

    bool init(const std::string& db_path);

    bool insert(const std::vector<geo_collection>& gc, const std::string& sql);
    bool remove(const std::vector<geo_collection>& gc, const std::string& sql);
    bool modify(const std::vector<geo_collection>& gc, const std::string& sql);
    bool select(const std::vector<geo_collection>& gc, const std::string& sql);

private:
    sqlite3*            m_db{nullptr};
    void*               m_sp_cache{nullptr};
    std::mutex          m_mutex;
};

#endif //SILLY_UTILS_SILLY_SPATIALITE_H
