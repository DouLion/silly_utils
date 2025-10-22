//
// Created by dell on 2024/1/25.
//

#include "silly_geo_coll.h"
silly_geo_coll::silly_geo_coll(const suPoint& p)
{
    m_type = eGeometryType::Point;
    m_point = p;
}

silly_geo_coll::silly_geo_coll(const silly_multi_point& mp)
{
    m_type = eGeometryType::MultiPoint;
    m_m_points = mp;
}

silly_geo_coll::silly_geo_coll(const suLine& l)
{
    m_type = eGeometryType::LineString;
    m_line = l;
}

silly_geo_coll::silly_geo_coll(const suMultiLine& ml)
{
    m_type = eGeometryType::MultiLineString;
    m_m_lines = ml;
}

silly_geo_coll::silly_geo_coll(const suPoly& p)
{
    m_type = eGeometryType::Polygon;
    m_poly = p;
}

silly_geo_coll::silly_geo_coll(const suMultiPoly& mp)
{
    m_type = eGeometryType::MultiPolygon;
    m_m_polys = mp;
}

void silly_geo_coll::add(const suPoint& p)
{
    m_type = eGeometryType::Point;
    m_point = p;
}

void silly_geo_coll::add(const silly_multi_point& mp)
{
    m_type = eGeometryType::MultiPoint;
    m_m_points = mp;
}

void silly_geo_coll::add(const suLine& l)
{
    m_type = eGeometryType::LineString;
    m_line = l;
}

void silly_geo_coll::add(const suMultiLine& ml)
{
    m_type = eGeometryType::MultiLineString;
    m_m_lines = ml;
}

void silly_geo_coll::add(const suPoly& p)
{
    m_type = eGeometryType::Polygon;

    m_poly = p;
}

void silly_geo_coll::add(const suMultiPoly& mp)
{
    m_type = eGeometryType::MultiPolygon;

    m_m_polys = mp;
}
