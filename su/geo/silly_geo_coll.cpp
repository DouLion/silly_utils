//
// Created by dell on 2024/1/25.
//

#include "silly_geo_coll.h"
silly_geo_coll::silly_geo_coll(const silly_point& p)
{
    m_type = eGeometryType::Point;
    m_point = p;
}

silly_geo_coll::silly_geo_coll(const silly_multi_point& mp)
{
    m_type = eGeometryType::MultiPoint;
    m_m_points = mp;
}

silly_geo_coll::silly_geo_coll(const silly_line& l)
{
    m_type = eGeometryType::LineString;
    m_line = l;
}

silly_geo_coll::silly_geo_coll(const silly_multi_line& ml)
{
    m_type = eGeometryType::MultiLineString;
    m_m_lines = ml;
}

silly_geo_coll::silly_geo_coll(const silly_poly& p)
{
    m_type = eGeometryType::Polygon;
    m_poly = p;
}

silly_geo_coll::silly_geo_coll(const silly_multi_poly& mp)
{
    m_type = eGeometryType::MultiPolygon;
    m_m_polys = mp;
}

void silly_geo_coll::add(const silly_point& p)
{
    m_type = eGeometryType::Point;
    m_point = p;
}

void silly_geo_coll::add(const silly_multi_point& mp)
{
    m_type = eGeometryType::MultiPoint;
    m_m_points = mp;
}

void silly_geo_coll::add(const silly_line& l)
{
    m_type = eGeometryType::LineString;
    m_line = l;
}

void silly_geo_coll::add(const silly_multi_line& ml)
{
    m_type = eGeometryType::MultiLineString;
    m_m_lines = ml;
}

void silly_geo_coll::add(const silly_poly& p)
{
    m_type = eGeometryType::Polygon;

    m_poly = p;
}

void silly_geo_coll::add(const silly_multi_poly& mp)
{
    m_type = eGeometryType::MultiPolygon;

    m_m_polys = mp;
}
