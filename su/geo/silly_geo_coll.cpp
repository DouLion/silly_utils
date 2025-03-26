//
// Created by dell on 2024/1/25.
//

#include "silly_geo_coll.h"
silly_geo_coll::silly_geo_coll(silly_point p)
{
    m_type = enum_geometry_type::egtPoint;
    m_point = p;
}

silly_geo_coll::silly_geo_coll(silly_multi_point mp)
{
    m_type = enum_geometry_type::egtMultiPoint;
    m_m_points = mp;
}

silly_geo_coll::silly_geo_coll(silly_line l)
{
    m_type = enum_geometry_type::egtLineString;
    m_line = l;
}

silly_geo_coll::silly_geo_coll(silly_multi_silly_line ml)
{
    m_type = enum_geometry_type::egtMultiLineString;
    m_m_lines = ml;
}

silly_geo_coll::silly_geo_coll(silly_poly p)
{
    m_type = enum_geometry_type::egtPolygon;
    m_poly = p;
}

silly_geo_coll::silly_geo_coll(silly_multi_poly mp)
{
    m_type = enum_geometry_type::egtMultiPolygon;
    m_m_polys = mp;
}

void silly_geo_coll::add(silly_point p)
{
    m_type = enum_geometry_type::egtPoint;
    m_point = p;
}

void silly_geo_coll::add(silly_multi_point mp)
{
    m_type = enum_geometry_type::egtMultiPoint;
    m_m_points = mp;
}

void silly_geo_coll::add(silly_line l)
{
    m_type = enum_geometry_type::egtLineString;
    m_line = l;
}

void silly_geo_coll::add(silly_multi_silly_line ml)
{
    m_type = enum_geometry_type::egtMultiLineString;
    m_m_lines = ml;
}

void silly_geo_coll::add(silly_poly p)
{
    m_type = enum_geometry_type::egtPolygon;

    m_poly = p;
}

void silly_geo_coll::add(silly_multi_poly mp)
{
    m_type = enum_geometry_type::egtMultiPolygon;

    m_m_polys = mp;
}
