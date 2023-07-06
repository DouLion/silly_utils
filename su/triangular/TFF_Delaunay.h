/********************************************************************************
	Copyright (C) 2004-2005 Sjaak Priester	

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this application; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************************/

// Delaunay
// Class to perform Delaunay triangulation on a set of vertices
//
// Version 1.2 (C) 2005, Sjaak Priester, Amsterdam.
// - Removed stupid bug in SetY; function wasn't used, so no consequences. Thanks to squat.
//
// Version 1.1 (C) 2005, Sjaak Priester, Amsterdam.
// - Removed bug which gave incorrect results for co-circular vertices.
//
// Version 1.0 (C) 2004, Sjaak Priester, Amsterdam.
// mailto:sjaak@sjaakpriester.nl

#pragma once
#include <assert.h>
#include <set>
#include <algorithm>
#include <math.h>
#include <vector>
#include "math/TFF_Point.h"

using namespace std;



//#ifndef _GDIPLUS_H

// I designed this with GDI+ in mind. However, this particular code doesn't
// use GDI+ at all, only some of it's variable types.
// These definitions are substitutes for those of GDI+. 
typedef double REALD;
struct PointFD
{
	PointFD() : X(0), Y(0)	{}
	PointFD(const PointFD& p) : X(p.X), Y(p.Y)	{}
	PointFD(REALD x, REALD y) : X(x), Y(y)	{}
	PointFD operator+(const PointFD& p) const	{ return PointFD(X + p.X, Y + p.Y); }
	PointFD operator-(const PointFD& p) const	{ return PointFD(X - p.X, Y - p.Y); }
	REALD X;
	REALD Y;
};

// = 2^-23; I've no idea why this is a good value, but GDI+ has it.
//const double double_EPSILON = 1.192092896e-07F;

class vertex;
class edge;
class triangle;
class CSimpleRect;
class Delaunay;

///////////////////
// vertex

class vertex
{
public:
	vertex()					: m_Pnt(0.0f, 0.0f, 0.0f),value(0.0F),m_Index(-1)	{}
	vertex(const vertex& v)		: m_Pnt(v.m_Pnt),value(v.value),m_Index(v.m_Index) {}
	vertex(const common::Point& pnt)	: m_Pnt(pnt)				{}
	vertex(double x, double y)		: m_Pnt(x, y, 0.0f)				{}
	vertex(double x, double y, double z)		: m_Pnt(x, y, z)				{value=z;}
	vertex(int x, int y)		: m_Pnt((float) x, (float) y, 0.0f)	{}

	bool operator>=(const vertex& v) const
	{
		if (m_Pnt.X() == v.m_Pnt.X()) return m_Pnt.Y() >= v.m_Pnt.Y();
		return m_Pnt.X() >= v.m_Pnt.X();
	}

	bool operator<(const vertex& v) const
	{
		if (m_Pnt.X() == v.m_Pnt.X()) return m_Pnt.Y() < v.m_Pnt.Y();
		return m_Pnt.X() < v.m_Pnt.X();
	}

	bool operator==(const vertex& v) const
	{
		return m_Pnt.X() == v.m_Pnt.X() && m_Pnt.Y() == v.m_Pnt.Y();
	}
	
	double GetX()	const	{ return m_Pnt.X(); }
	double GetY() const	{ return m_Pnt.Y(); }
	double GetValue()	const { return value;  }
	int GetIndex()	const { return m_Index;  }

	void SetX(double x)		{ m_Pnt.X() = x; }
	void SetY(double y)		{ m_Pnt.Y() = y; }
	void SetValue(double v)	{ value   = v; }
	void SetIndex(int Index)	{ m_Index   = Index; }

	const common::Point& GetPoint() const		{ return m_Pnt; }
protected:
	common::Point	m_Pnt;
	double	value;
	int m_Index;
};

typedef set<vertex> vertexSet;
typedef set<vertex>::iterator vIterator;
typedef set<vertex>::const_iterator cvIterator;

///////////////////
//CSimpleRect
class CSimpleRect
{

#define MAX_3(a, b, c) (a > b ? (a > c ? a : c) : (b > c ? b : c) )
#define MIN_3(a, b, c) (a < b ? (a < c ? a : c) : (b < c ? b : c) ) 

public:
	double x0, x1;
	double y0, y1;

	CSimpleRect():x0(0), x1(0), y0(0), y1(0) {}
	CSimpleRect(double _x0, double _x1, double _y0, double _y1): x0(_x0), x1(_x1), y0(_y0), y1(_y1) {}
	CSimpleRect(const vertex ** t)
	{
		x0 = MIN_3(t[0]->GetX(), t[1]->GetX(), t[2]->GetX());
		x1 = MAX_3(t[0]->GetX(), t[1]->GetX(), t[2]->GetX());
		y0 = MIN_3(t[0]->GetY(), t[1]->GetY(), t[2]->GetY());
		y1 = MAX_3(t[0]->GetY(), t[1]->GetY(), t[2]->GetY());
	}

	bool IsCross(const CSimpleRect& OtherRect) const
	{
		if ((x0 > OtherRect.x1) || (y0 > OtherRect.y1) ||
			(x1 < OtherRect.x0) || (y1 < OtherRect.y0))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool IsCrossT(const CSimpleRect& aTriangle) {
		return this->IsCross(CSimpleRect(aTriangle));
	}

	bool operator()(const CSimpleRect& OtherRect) const {
		return IsCross(OtherRect);
	}
};
class triangle
{
public:
	triangle()
	{

	}

	triangle(const triangle& tri)
		: m_Center(tri.m_Center)
		, m_R(tri.m_R)
		, m_R2(tri.m_R2)
		, m_Rect(tri.m_Rect)
	{
		for (int i = 0; i < 3; i++) m_Vertices[i] = tri.m_Vertices[i];
	}
	triangle(const vertex * p0, const vertex * p1, const vertex * p2)
	{
		m_Vertices[0] = p0;
		m_Vertices[1] = p1;
		m_Vertices[2] = p2;
		SetCircumCircle();
	}
	triangle(const vertex * pV)
	{
		for (int i = 0; i < 3; i++) m_Vertices[i] = pV++;
		SetCircumCircle();
	}

	bool operator<(const triangle& tri) const
	{
		if (m_Center.X() == tri.m_Center.X()) return m_Center.Y() < tri.m_Center.Y();
		return m_Center.X() < tri.m_Center.X();
	}

	triangle& operator=(const triangle& tri) const
	{
		triangle& dstTri = (triangle&)(*this);
		triangle& trii = (triangle&)tri;
		for (int i = 0; i < 3; ++i) 
		{
			dstTri.m_Vertices[i] = trii.m_Vertices[i];
		}
		dstTri.m_Center = trii.m_Center;
		dstTri.m_R = trii.m_R;
		dstTri.m_R2 = trii.m_R2;
		dstTri.m_Rect = trii.m_Rect;
		//(*this) = dstTri;
		return (triangle&)(*this);
	}

	const vertex * GetVertex(int i) const
	{
		//ASSERT(i >= 0);
		//ASSERT(i < 3);
		return m_Vertices[i];
	}

	vertex GetTriangleCenter() const
	{
		return vertex(m_Center);
	}

	double GetCircumCircle_R()
	{
		return (this->m_R);
	}

	bool IsLeftOf(cvIterator itVertex) const
	{
		// returns true if * itVertex is to the right of the triangle's circumcircle
		return itVertex->GetPoint().X() > (m_Center.X() + m_R);
	}

	bool CCEncompasses(cvIterator itVertex) const
	{
		// Returns true if * itVertex is in the triangle's circumcircle.
		// A vertex exactly on the circle is also considered to be in the circle.

		// These next few lines look like optimisation, however in practice they are not.
		// They even seem to slow down the Algorithm somewhat.
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

		common::Point dist = itVertex->GetPoint() - m_Center;		// the distance between v and the circle center
		double dist2 = dist.X() * dist.X() + dist.Y() * dist.Y();		// squared
		return dist2 < m_R2;								// compare with squared radius
	}

	bool InArea(const vertex & point) const
	{
		double x = point.GetX();
		double y = point.GetY();
		double PointList[6] = {m_Vertices[0]->GetX(),m_Vertices[0]->GetY(),m_Vertices[1]->GetX(),m_Vertices[1]->GetY(),m_Vertices[2]->GetX(),m_Vertices[2]->GetY()};
		
		int i, j;
		bool c = false;
		for (i = 0, j = 2; i < 3; j = i++) {
			if (((((PointList[i*2+1] <= y) && (y < PointList[j*2+1])) ||
				((PointList[j*2+1] <= y) && (y < PointList[i*2+1]))) &&
				(x < (PointList[j*2] - PointList[i*2]) * (y - PointList[i*2+1]) / (PointList[j*2+1] - PointList[i*2+1]) + PointList[i*2])))
				c = !c;
		}
		
		return c;
	}

	bool Encompasses(const vertex & Vertex) const
	{
		
		common::Point dist = Vertex.GetPoint() - m_Center;
		double x = fabs(dist.X());
		double y = fabs(dist.Y());

		// First check boundary box.
		if (x >  m_R) return false;
		if (x < -m_R) return false;
		if (y >  m_R) return false;
		if (y < -m_R) return false;
		// Second check circle
		double dist2 = dist.X() * dist.X() + dist.Y() * dist.Y();
		if(dist2 > m_R2)
		{
			return false;
		}
		// third check is in the triangle
		bool rightOrder = GetArea()>0;
		if (GetArea()==0){return true;}
		int i,j;
		for (i=0;i<3;i++){
			j = (i + 1) % 3;
			triangle tt(GetVertex(i),GetVertex(j),&Vertex);
			double s =tt.GetArea();
			if ((rightOrder && s>=0) || (rightOrder && s<=0)){
				return false;
			}
		}
		return true;
	}

	double GetArea()const
	{
		int i,j;
		double area = 0;
		for (i=0;i<3;i++) {
		  j = (i + 1) % 3;
		  area += GetVertex(i)->GetX() * GetVertex(j)->GetY();
		  area -= GetVertex(i)->GetY() * GetVertex(j)->GetX();
		}
		return area /= 2;
	}

	bool CreatRect(){
		m_Rect = CSimpleRect(m_Vertices);
		return true;
	}

	const CSimpleRect & GetRect() const
	{
		return m_Rect;
	}

	double m_zz[3];				// 存放中间结果
	void BeforehandCaculate(){	// 计算中间结果
		double zz[9];
		zz[0] = m_Vertices[0]->GetX();
		zz[1] = m_Vertices[0]->GetY();
		zz[2] = m_Vertices[0]->GetValue();
		zz[3] = m_Vertices[1]->GetX();
		zz[4] = m_Vertices[1]->GetY();
		zz[5] = m_Vertices[1]->GetValue();
		zz[6] = m_Vertices[2]->GetX();
		zz[7] = m_Vertices[2]->GetY();
		zz[8] = m_Vertices[2]->GetValue();
		//------------------------------------------------------------------------------
		zz[3] -= zz[0];
		zz[4] -= zz[1];
		zz[5] -= zz[2];
		zz[6] -= zz[0];
		zz[7] -= zz[1];
		zz[8] -= zz[2];
		//
		m_zz[0]=zz[4]*zz[8] - zz[5]*zz[7];
		m_zz[1]=zz[5]*zz[6] - zz[3]*zz[8];
		m_zz[2]=zz[3]*zz[7] - zz[4]*zz[6];
	}
protected:
	const vertex * m_Vertices[3];	// the three triangle vertices
	common::Point m_Center;				// center of circumcircle
	double m_R;			// radius of circumcircle
	double m_R2;			// radius of circumcircle, squared
	CSimpleRect m_Rect;				// bound

	void SetCircumCircle();
};

// Changed in verion 1.1: collect triangles in a multiset.
// In version 1.0, I used a set, preventing the creation of multiple
// triangles with identical center points. Therefore, more than three
// co-circular vertices yielded incorrect results. Thanks to Roger Labbe.
typedef multiset<triangle> triangleSet;
typedef multiset<triangle>::iterator tIterator;
typedef multiset<triangle>::const_iterator ctIterator;

///////////////////
// edge

class edge
{
public:
	edge(const edge& e)	: m_pV0(e.m_pV0), m_pV1(e.m_pV1)	{}
	edge(const vertex * pV0, const vertex * pV1)
		: m_pV0(pV0), m_pV1(pV1)
	{
	}

	bool operator<(const edge& e) const
	{
		if (m_pV0 == e.m_pV0) return * m_pV1 < * e.m_pV1;
		return * m_pV0 < * e.m_pV0;
	}

	const vertex * m_pV0;
	const vertex * m_pV1;
};

typedef set<edge> edgeSet;
typedef set<edge>::iterator edgeIterator;
typedef set<edge>::const_iterator cedgeIterator;

///////////////////
// Delaunay

class Delaunay
{
public:
	// Calculate the Delaunay triangulation for the given set of vertices.
	void Triangulate(const vertexSet& vertices, triangleSet& output, bool bCreateHull);
	int  Triangulate(int num, double *dx,double *dy,double *dz, double *&tax,double *&tay,double *&taz,double *&tbx,double *&tby,double *&tbz,double *&tcx,double *&tcy,double *&tcz, int *&abk, int *&ack, int *&bck);

	void Triangulate(const vertexSet& vertices, triangleSet& output);
	int  Triangulate(int num, double *dx,double *dy,double *dz, double *&tax,double *&tay,double *&taz,double *&tbx,double *&tby,double *&tbz,double *&tcx,double *&tcy,double *&tcz);


	// Put the edges of the triangles in an edgeSet, eliminating double edges.
	// This comes in useful for drawing the triangulation.
	void TrianglesToEdges(const triangleSet& triangles, edgeSet& edges);
protected:
	void HandleEdge(const vertex * p0, const vertex * p1, edgeSet& edges);
};


