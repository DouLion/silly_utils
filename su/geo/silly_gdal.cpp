//
// Created by dell on 2025/9/23.
//

#include "silly_gdal.h"

#if SU_THIRD_SUPPORT_GDAL
// 将 silly_ring 转换为 OGRPolygon
OGRLinearRing silly_gdal::silly_ring_to_ogr(const silly_ring& ring)
{
    OGRLinearRing result;
    std::vector<double> xs(ring.points.size());
    std::vector<double> ys(ring.points.size());
    for (int i = 0; i < ring.points.size(); i++)
    {
        xs[i] = ring.points[i].x;
        ys[i] = ring.points[i].y;
    }
    result.setPoints(static_cast<int>(ring.points.size()), &xs[0], &ys[0]);
    result.closeRings();
    return result;
}

// 环OGRLinearRing对象，将其转换为silly_ring对象  (环)
silly_ring silly_gdal::silly_ring_from_ogr(const OGRLinearRing* ring)
{
    silly_ring result;
    int pointCount = ring->getNumPoints();
    for (int i = 0; i < pointCount; i++)
    {
        double x = ring->getX(i);
        double y = ring->getY(i);
        result.points.push_back({x, y});
    }
    return result;
}

// 将 OGRPoint(单点) 转换为 silly_point(单点) 类型
silly_point silly_gdal::silly_point_from_ogr(const OGRPoint* ogrPoint)
{
    silly_point result(ogrPoint->getX(), ogrPoint->getY());
    return result;
}

// 将 silly_point(单点) 转换为 OGRPoint(单点) 类型
OGRPoint silly_gdal::silly_point_to_ogr(const silly_point& point)
{
    OGRPoint ogrPoint(point.x, point.y);
    return ogrPoint;
}

// 将 OGRMultiPoint(多点) 转换为 silly_multi_point(多点) 类型
silly_multi_point silly_gdal::silly_multi_point_from_ogr(const OGRMultiPoint* ogrMultiPoint)
{
    silly_multi_point multiPoint;
    int pointCount = ogrMultiPoint->getNumGeometries();
    for (int i = 0; i < pointCount; i++)
    {
        silly_point sillyPoint = silly_point_from_ogr(ogrMultiPoint->getGeometryRef(i));
        multiPoint.push_back(sillyPoint);
    }
    return multiPoint;
}

// 将 silly_multi_point(多点) 转换为 OGRMultiPoint(多点) 类型
OGRMultiPoint silly_gdal::silly_multi_point_to_ogr(const silly_multi_point& mulitPoint)
{
    OGRMultiPoint orgMultiPoint;
    for (const silly_point& point : mulitPoint)
    {
        OGRPoint ogrPoint = silly_point_to_ogr(point);
        // orgMultiPoint.addGeometryDirectly(ogrPoint.clone());
        orgMultiPoint.addGeometry(&ogrPoint);
    }
    return orgMultiPoint;
}

// OGRLineString(线)类型转为silly_line(线)类型
silly_line silly_gdal::silly_line_from_ogr(const OGRLineString* lineString)
{
    silly_line line;
    int num_points = lineString->getNumPoints();
    for (int j = 0; j < num_points; j++)
    {
        line.push_back({lineString->getX(j), lineString->getY(j)});
    }
    return line;
}

// 将 silly_line(线) 转换为 OGRLineString(线)类型
OGRLineString silly_gdal::silly_line_to_ogr(const silly_line& line)
{
    OGRLineString ogrLineString;
    std::vector<double> xs(line.size());
    std::vector<double> ys(line.size());
    for (int i = 0; i < line.size(); i++)
    {
        xs[i] = line[i].x;
        ys[i] = line[i].y;
    }
    ogrLineString.setPoints((int)line.size(), &xs[0], &ys[0]);
    return ogrLineString;
}

// OGRMultiLineString(多线)类型转为 silly_multiline(多线)类型
silly_multi_line silly_gdal::silly_multi_line_from_ogr(const OGRMultiLineString* multiLineString)
{
    silly_multi_line multiLine;
    int numLines = multiLineString->getNumGeometries();
    for (int i = 0; i < numLines; i++)
    {
        auto lineString = (OGRLineString*)multiLineString->getGeometryRef(i);
        if (lineString != nullptr)
        {
            silly_line line = silly_line_from_ogr(lineString);
            multiLine.push_back(line);
        }
    }
    return multiLine;
}

// 将 silly_multiline(多线) 转换为 OGRMultiLineString(多线)类型
OGRMultiLineString silly_gdal::silly_multi_line_to_ogr(const silly_multi_line& multiLine)
{
    OGRMultiLineString ogrMultiLineString;

    for (const silly_line& line : multiLine)
    {
        OGRLineString ogrLineString = silly_line_to_ogr(line);
        ogrMultiLineString.addGeometry(&ogrLineString);
    }

    return ogrMultiLineString;
}

// OGRPolygon对象转换为silly_poly(多环:外环+内环)对象  (单面)
silly_poly silly_gdal::silly_poly_from_ogr(const OGRPolygon* polygon)
{
    silly_poly poly;
    // 处理OGRPolygon外环
    auto outerRing = (OGRLinearRing*)polygon->getExteriorRing();
    poly.outer = silly_ring_from_ogr(outerRing);
    // 处理OGRPolygon内环
    int innerRingCount = polygon->getNumInteriorRings();
    for (int k = 0; k < innerRingCount; k++)
    {
        auto ring = (OGRLinearRing*)polygon->getInteriorRing(k);
        silly_ring innerRing = silly_ring_from_ogr(ring);
        poly.holes.push_back(innerRing);
    }
    return poly;
}

// 将 silly_poly 转换为 OGRPolygon(单面)
OGRPolygon silly_gdal::silly_poly_to_ogr(const silly_poly& poly)
{
    OGRPolygon org;
    // 设置外环
    OGRLinearRing outerRing = silly_ring_to_ogr(poly.outer);
    // double a = outerRing.get_Area();
    org.addRing(&outerRing);
    // 设置内环
    for (const silly_ring& innerRing : poly.holes)
    {
        OGRLinearRing innerOGRRing = silly_ring_to_ogr(innerRing);
        org.addRing(&innerOGRRing);
        org.closeRings();
    }
    return org;
}

// 多面的OGRMultiPolygon对象转换为silly_multi_poly(多面)
silly_multi_poly silly_gdal::silly_multi_poly_from_ogr(const OGRMultiPolygon* multiPolygon)
{
    silly_multi_poly multi_poly;
    int polygonCount = multiPolygon->getNumGeometries();
    for (int i = 0; i < polygonCount; i++)
    {
        silly_poly tmp_poly;
        auto polygon = (OGRPolygon*)multiPolygon->getGeometryRef(i);
        tmp_poly = silly_poly_from_ogr(polygon);
        multi_poly.push_back(tmp_poly);
    }
    return multi_poly;
}

// 将silly_multi_poly对象转换为OGRMultiPolygon对象(多面)
OGRMultiPolygon silly_gdal::silly_multi_poly_to_ogr(const silly_multi_poly& multiPoly)
{
    OGRMultiPolygon ogrMultiPolygon;
    for (const silly_poly& poly : multiPoly)
    {
        OGRPolygon ogrPolygon = silly_poly_to_ogr(poly);
        ogrMultiPolygon.addGeometry(&ogrPolygon);
    }

    return ogrMultiPolygon;
}

OGRGeometry* silly_gdal::silly_geo_coll_to_ogr(const silly_geo_coll& coll)
{
    switch (coll.m_type)
    {
        case eGeometryType::Point:
            return new OGRPoint(silly_gdal::silly_point_to_ogr(coll.m_point));

        case eGeometryType::MultiPoint:
            return new OGRMultiPoint(silly_gdal::silly_multi_point_to_ogr(coll.m_m_points));

        case eGeometryType::LineString:
            return new OGRLineString(silly_gdal::silly_line_to_ogr(coll.m_line));

        case eGeometryType::MultiLineString:
            return new OGRMultiLineString(silly_gdal::silly_multi_line_to_ogr(coll.m_m_lines));

        case eGeometryType::Polygon:
            return new OGRPolygon(silly_gdal::silly_poly_to_ogr(coll.m_poly));

        case eGeometryType::MultiPolygon:
            return new OGRMultiPolygon(silly_gdal::silly_multi_poly_to_ogr(coll.m_m_polys));

        default:
            SLOG_ERROR("Error: Unsupported type: {}");
            return nullptr;
    }
}

silly_geo_coll silly_gdal::silly_geo_coll_from_ogr(const OGRGeometry* geometry)
{
    switch (geometry->getGeometryType())
    {
        case wkbPoint:
            return silly_point_from_ogr(static_cast<const OGRPoint*>(geometry));
        case wkbMultiPoint:
            return silly_multi_point_from_ogr(static_cast<const OGRMultiPoint*>(geometry));
        case wkbLineString:
            return silly_line_from_ogr(static_cast<const OGRLineString*>(geometry));
        case wkbMultiLineString:
            return silly_multi_line_from_ogr(static_cast<const OGRMultiLineString*>(geometry));
        case wkbPolygon:
            return silly_poly_from_ogr(static_cast<const OGRPolygon*>(geometry));
        case wkbMultiPolygon:
            return silly_multi_poly_from_ogr(static_cast<const OGRMultiPolygon*>(geometry));
        default:
            SLOG_ERROR("Error: Unsupported type: {}");
            return silly_geo_coll();
    }
}
#endif