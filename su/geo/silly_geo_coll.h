/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 10:23
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_GEO_COLL_H
#define SILLY_UTILS_SILLY_GEO_COLL_H
#include <geo/silly_geo.h>
#include <geo/silly_geo_prop.h>

class suGeoColl
{
    using suGeoCollType = std::variant<suPoint, suMultiPoint, suLine, suMultiLine, suPoly, suMultiPoly>;

  public:
    suGeoColl() = default;

    explicit suGeoColl(const suPoint& p) : m_geometry(p), m_type(eGeometryType::Point)
    {
    }
    explicit suGeoColl(const suMultiPoint& mp) : m_geometry(mp), m_type(eGeometryType::MultiPoint)
    {
    }
    explicit suGeoColl(const suLine& l) : m_geometry(l), m_type(eGeometryType::LineString)
    {
    }
    explicit suGeoColl(const suMultiLine& ml) : m_geometry(ml), m_type(eGeometryType::MultiLineString)
    {
    }
    explicit suGeoColl(const suPoly& p) : m_geometry(p), m_type(eGeometryType::Polygon)
    {
    }
    explicit suGeoColl(const suMultiPoly& mp) : m_geometry(mp), m_type(eGeometryType::MultiPolygon)
    {
    }

    void set(const suPoint& p)
    {
        m_geometry = p;
        m_type = eGeometryType::Point;
    }
    void set(const suMultiPoint& mp)
    {
        m_geometry = mp;
        m_type = eGeometryType::MultiPoint;
    }
    void set(const suLine& l)
    {
        m_geometry = l;
        m_type = eGeometryType::LineString;
    }
    void set(const suMultiLine& ml)
    {
        m_geometry = ml;
        m_type = eGeometryType::MultiLineString;
    }
    void set(const suPoly& p)
    {
        m_geometry = p;
        m_type = eGeometryType::Polygon;
    }
    void set(const suMultiPoly& mp)
    {
        m_geometry = mp;
        m_type = eGeometryType::MultiPolygon;
    }

    // 访问函数示例：安全地返回 suPoint&
    suPoint& point()
    {
        if (m_type != eGeometryType::Point)
            throw std::runtime_error("Not a Point!");
        return std::get<suPoint>(m_geometry);
    }

    const suPoint& point() const
    {
        if (m_type != eGeometryType::Point)
            throw std::runtime_error("Not a Point!");
        return std::get<suPoint>(m_geometry);
    }
    suMultiPoint& multiPoint()
    {
        if (m_type != eGeometryType::MultiPoint)
            throw std::runtime_error("Not a MultiPoint!");
        return std::get<suMultiPoint>(m_geometry);
    }

    const suMultiPoint& multiPoint() const
    {
        if (m_type != eGeometryType::MultiPoint)
            throw std::runtime_error("Not a MultiPoint!");
        return std::get<suMultiPoint>(m_geometry);
    }

    suLine& line()
    {
        if (m_type != eGeometryType::LineString)
        {
            throw std::runtime_error("Not a LineString!");
        }
        return std::get<suLine>(m_geometry);
    }

    const suLine& line() const
    {
        if (m_type != eGeometryType::LineString)
        {
            throw std::runtime_error("Not a LineString!");
        }
        return std::get<suLine>(m_geometry);
    }

    suMultiLine& multiLine()
    {
        if (m_type != eGeometryType::MultiLineString)
        {
            throw std::runtime_error("Not a MultiLineString!");
        }
        return std::get<suMultiLine>(m_geometry);
    }

    const suMultiLine& multiLine() const
    {
        if (m_type != eGeometryType::MultiLineString)
        {
            throw std::runtime_error("Not a MultiLineString!");
        }
        return std::get<suMultiLine>(m_geometry);
    }

    suPoly& poly()
    {
        if (m_type != eGeometryType::Polygon)
        {
            throw std::runtime_error("Not a Polygon!");
        }
        return std::get<suPoly>(m_geometry);
    }

    const suPoly& poly() const
    {
        if (m_type != eGeometryType::Polygon)
        {
            throw std::runtime_error("Not a Polygon!");
        }
        return std::get<suPoly>(m_geometry);
    }

    suMultiPoly& multiPoly()
    {
        if (m_type != eGeometryType::MultiPolygon)
        {
            throw std::runtime_error("Not a MultiPolygon!");
        }
        return std::get<suMultiPoly>(m_geometry);
    }

    const suMultiPoly& multiPoly() const
    {
        if (m_type != eGeometryType::MultiPolygon)
        {
            throw std::runtime_error("Not a MultiPolygon!");
        }
        return std::get<suMultiPoly>(m_geometry);
    }

    eGeometryType type() const
    {
        return m_type;
    }
    std::unordered_map<std::string, suGeoProp>& properties()
    {
        return m_properties;
    }

    const std::unordered_map<std::string, suGeoProp>& properties() const
    {
        return m_properties;
    }

  private:
    // 类型
    suGeoCollType m_geometry;
    eGeometryType m_type{eGeometryType::Invalid};
    // 属性列表
    std::unordered_map<std::string, suGeoProp> m_properties;
    std::map<uint16_t, std::string> m_prop_index;
};

#endif  // SILLY_UTILS_SILLY_GEO_COLL_H
