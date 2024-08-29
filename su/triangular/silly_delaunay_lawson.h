/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 15:37
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_DELAUNAY_LAWSON_H
#define SILLY_UTILS_SILLY_DELAUNAY_LAWSON_H

#include <su_marco.h>
#include <set>
//
// changes to remove dependencies on Microsoft GDI+
//
// Change REAL to double everywhere.
//
// Change to use Qt's d_l_point instead of PointF
//
#ifndef REAL_EPSILON
#define REAL_EPSILON 1.192092896e-07F
#endif
///////////////////
// vertex

struct d_l_point
{
    d_l_point() = default;
    d_l_point(double x, double y) : m_x(x), m_y(y)
    {
    }
    bool operator==(const d_l_point& t) const
    {
        return abs(t.m_x - m_x) < REAL_EPSILON && abs(t.m_y - m_y);
    }

    double x() const
    {
        return m_x;
    }
    double y() const
    {
        return m_y;
    }
    double m_x;
    double m_y;
};

class vertex
{
  public:
    vertex() : m_Pnt(0.0, 0.0)
    {
    }
    vertex(const vertex& v) : m_Pnt(v.m_Pnt)
    {
    }
    vertex(const d_l_point& pnt) : m_Pnt(pnt)
    {
    }
    vertex(double x, double y) : m_Pnt(x, y)
    {
    }
    vertex(int x, int y) : m_Pnt((double)x, (double)y)
    {
    }

    bool operator<(const vertex& v) const
    {
        if (m_Pnt.x() == v.m_Pnt.x())
            return m_Pnt.y() < v.m_Pnt.y();
        return m_Pnt.x() < v.m_Pnt.x();
    }

    bool operator==(const vertex& v) const
    {
        return m_Pnt == v.m_Pnt;
    }

    double GetX() const
    {
        return m_Pnt.x();
    }
    double GetY() const
    {
        return m_Pnt.y();
    }

    void SetX(double x)
    {
        m_Pnt.m_x = x;
    }
    void SetY(double y)
    {
        m_Pnt.m_y = y;
    }

    const d_l_point& GetPoint() const
    {
        return m_Pnt;
    }

  protected:
    d_l_point m_Pnt;
};

typedef std::set<vertex> vertexSet;
typedef std::set<vertex>::iterator vIterator;
typedef std::set<vertex>::const_iterator cvIterator;

///////////////////
// triangle

class triangle
{
  public:
    triangle(const triangle& tri) : m_Center(tri.m_Center), m_R(tri.m_R), m_R2(tri.m_R2)
    {
        for (int i = 0; i < 3; i++)
            m_Vertices[i] = tri.m_Vertices[i];
    }
    triangle(const vertex* p0, const vertex* p1, const vertex* p2)
    {
        m_Vertices[0] = p0;
        m_Vertices[1] = p1;
        m_Vertices[2] = p2;
        SetCircumCircle();
    }
    triangle(const vertex* pV)
    {
        for (int i = 0; i < 3; i++)
            m_Vertices[i] = pV++;
        SetCircumCircle();
    }

    triangle& operator=(const triangle& tri)
    {
        m_Center = tri.m_Center;
        m_R = tri.m_R;
        m_R2 = tri.m_R2;
        for (int i = 0; i < 3; i++)
            m_Vertices[i] = tri.m_Vertices[i];

        return (*this);
    };

    bool operator<(const triangle& tri) const
    {
        if (m_Center.x() == tri.m_Center.x())
            return m_Center.y() < tri.m_Center.y();
        return m_Center.x() < tri.m_Center.x();
    }

    const vertex* GetVertex(int i) const
    {
        return m_Vertices[i];
    }

    bool IsLeftOf(cvIterator itVertex) const
    {
        // returns true if * itVertex is to the right of the triangle's circumcircle
        return itVertex->GetPoint().x() > (m_Center.x() + m_R);
    }

    bool CCEncompasses(cvIterator itVertex) const
    {
        // Returns true if * itVertex is in the triangle's circumcircle.
        // A vertex exactly on the circle is also considered to be in the circle.

        // These next few lines look like optimisation, however in practice they are not.
        // They even seem to slow down the algorithm somewhat.
        // Therefore, I've commented them out.

        // First check boundary box.
        //		double x = itVertex->GetPoint().X;
        //
        //		if (x > (m_Center.X + m_R)) return false;
        //		if (x < (m_Center.X - m_R)) return false;
        //
        //		double y = itVertex->GetPoint().Y;
        //
        //		if (y > (m_Center.Y + m_R)) return false;
        //		if (y < (m_Center.Y - m_R)) return false;

        d_l_point dist = itVertex->GetPoint() - m_Center;          // the distance between v and the circle center
        double dist2 = dist.x() * dist.x() + dist.y() * dist.y();  // squared
        return dist2 <= m_R2;                                      // compare with squared radius
    }

  protected:
    const vertex* m_Vertices[3];  // the three triangle vertices
    d_l_point m_Center;           // center of circumcircle
    double m_R;                   // radius of circumcircle
    double m_R2;                  // radius of circumcircle, squared

    void SetCircumCircle();
};

// Changed in verion 1.1: collect triangles in a multiset.
// In version 1.0, I used a set, preventing the creation of multiple
// triangles with identical center points. Therefore, more than three
// co-circular vertices yielded incorrect results. Thanks to Roger Labbe.
typedef std::multiset<triangle> triangleSet;
typedef std::multiset<triangle>::iterator tIterator;
typedef std::multiset<triangle>::const_iterator ctIterator;

///////////////////
// edge

class edge
{
  public:
    edge(const edge& e) : m_pV0(e.m_pV0), m_pV1(e.m_pV1)
    {
    }
    edge(const vertex* pV0, const vertex* pV1) : m_pV0(pV0), m_pV1(pV1)
    {
    }

    bool operator<(const edge& e) const
    {
        if (m_pV0 == e.m_pV0)
            return *m_pV1 < *e.m_pV1;
        return *m_pV0 < *e.m_pV0;
    }

    const vertex* m_pV0;
    const vertex* m_pV1;
};

typedef std::set<edge> edgeSet;
typedef std::set<edge>::iterator edgeIterator;
typedef std::set<edge>::const_iterator cedgeIterator;

///////////////////
// Delaunay

class silly_delaunay_lawson
{
  public:
    // Calculate the Delaunay triangulation for the given set of vertices.
    void Triangulate(const vertexSet& vertices, triangleSet& output);

    // Put the edges of the triangles in an edgeSet, eliminating double edges.
    // This comes in useful for drawing the triangulation.
    void TrianglesToEdges(const triangleSet& triangles, edgeSet& edges);

  protected:
    void HandleEdge(const vertex* p0, const vertex* p1, edgeSet& edges);
};

#endif  // SILLY_UTILS_SILLY_DELAUNAY_LAWSON_H
