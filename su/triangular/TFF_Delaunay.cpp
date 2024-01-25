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
	along with Tinter; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************************/

// Delaunay
// Class to perform Delaunay triangulation on a set of vertices
//
// Version 1.1 (C) 2005, Sjaak Priester, Amsterdam.
// - Removed bug which gave incorrect results for co-circular vertices.
//
// Version 1.0 (C) 2004, Sjaak Priester, Amsterdam.
// mailto:sjaak@sjaakpriester.nl

#include "TFF_Delaunay.h"
#include "TFF_CovexHull.h"
#include <iterator>
#include <cstring>

#define sqrt3 1.732050808F
#ifndef REAL_EPSILON
#define REAL_EPSILON 1.192092896e-07F
#endif 

void triangle::SetCircumCircle()
{
	double x0 = m_Vertices[0]->GetX();
	double y0 = m_Vertices[0]->GetY();

	double x1 = m_Vertices[1]->GetX();
	double y1 = m_Vertices[1]->GetY();

	double x2 = m_Vertices[2]->GetX();
	double y2 = m_Vertices[2]->GetY();

	double y10 = y1 - y0;
	double y21 = y2 - y1;

	bool b21zero = y21 > -REAL_EPSILON && y21 < REAL_EPSILON;

	if (y10 > -REAL_EPSILON && y10 < REAL_EPSILON)
	{
		if (b21zero)	// All three vertices are on one horizontal line.
		{
			if (x1 > x0)
			{
				if (x2 > x1) x1 = x2;
			}
			else
			{
				if (x2 < x0) x0 = x2;
			}
			m_Center.X() = (x0 + x1) * .5;
			m_Center.Y() = y0;
		}
		else	// m_Vertices[0] and m_Vertices[1] are on one horizontal line.
		{
			double m1 = -(x2 - x1) / y21;

			double mx1 = (x1 + x2) * .5;
			double my1 = (y1 + y2) * .5;

			m_Center.X() = (x0 + x1) * .5;
			m_Center.Y() = m1 * (m_Center.X() - mx1) + my1;
		}
	}
	else if (b21zero)	// m_Vertices[1] and m_Vertices[2] are on one horizontal line.
	{
		double m0 = -(x1 - x0) / y10;

		double mx0 = (x0 + x1) * .5;
		double my0 = (y0 + y1) * .5;

		m_Center.X() = (x1 + x2) * .5;
		m_Center.Y() = m0 * (m_Center.X() - mx0) + my0;
	}
	else	// 'Common' cases, no multiple vertices are on one horizontal line.
	{
		double m0 = -(x1 - x0) / y10;
		double m1 = -(x2 - x1) / y21;

		double mx0 = (x0 + x1) * .5;
		double my0 = (y0 + y1) * .5;

		double mx1 = (x1 + x2) * .5;
		double my1 = (y1 + y2) * .5;

		m_Center.X() = (m0 * mx0 - m1 * mx1 + my1 - my0) / (m0 - m1);
		m_Center.Y() = m0 * (m_Center.X() - mx0) + my0;
	}

	double dx = x0 - m_Center.X();
	double dy = y0 - m_Center.Y();

	m_R2 = dx * dx + dy * dy;	// the radius of the circumcircle, squared
	m_R = (double)sqrt(m_R2);	// the proper radius

	// Version 1.1: make m_R2 slightly higher to ensure that all edges
	// of co-circular vertices will be caught.
	// Note that this is a compromise. In fact, the algorithm isn't doublely
	// suited for very many co-circular vertices.
	m_R2 *= 1.000000001;
}

// Function object to check whether a triangle has one of the vertices in SuperTriangle.
// operator() returns true if it does.
class triangleHasVertex
{
public:
	triangleHasVertex(const vertex SuperTriangle[3]) : m_pSuperTriangle(SuperTriangle) {}
	bool operator()(const triangle& tri) const
	{
		for (int i = 0; i < 3; i++)
		{
			const vertex* p = tri.GetVertex(i);
			if (p >= m_pSuperTriangle && p < (m_pSuperTriangle + 3)) return true;
		}
		return false;
	}
protected:
	const vertex* m_pSuperTriangle;
};

// Function object to check whether a triangle is 'completed', i.e. doesn't need to be checked
// again in the algorithm, i.e. it won't be changed anymore.
// Therefore it can be removed from the workset.
// A triangle is completed if the circumcircle is completely to the left of the current vertex.
// If a triangle is completed, it will be inserted in the output set, unless one or more of it's vertices
// belong to the 'super triangle'.
class triangleIsCompleted
{
public:
	triangleIsCompleted(cvIterator itVertex, triangleSet& output, const vertex SuperTriangle[3])
		: m_itVertex(itVertex)
		, m_Output(output)
		, m_pSuperTriangle(SuperTriangle)
	{}
	bool operator()(const triangle& tri) const
	{
		bool b = tri.IsLeftOf(m_itVertex);

		if (b)
		{
			triangleHasVertex thv(m_pSuperTriangle);
			if (!thv(tri)) m_Output.insert(tri);
		}
		return b;
	}

protected:
	cvIterator m_itVertex;
	triangleSet& m_Output;
	const vertex* m_pSuperTriangle;
};

// Function object to check whether vertex is in circumcircle of triangle.
// operator() returns true if it does.
// The edges of a 'hot' triangle are stored in the edgeSet edges.
class vertexIsInCircumCircle
{
public:
	vertexIsInCircumCircle(cvIterator itVertex, edgeSet& edges) : m_itVertex(itVertex), m_Edges(edges) {}
	bool operator()(const triangle& tri) const
	{
		bool b = tri.CCEncompasses(m_itVertex);

		if (b)
		{
			HandleEdge(tri.GetVertex(0), tri.GetVertex(1));
			HandleEdge(tri.GetVertex(1), tri.GetVertex(2));
			HandleEdge(tri.GetVertex(2), tri.GetVertex(0));
		}
		return b;
	}
protected:
	void HandleEdge(const vertex* p0, const vertex* p1) const
	{
		const vertex* pVertex0 = 0;
		const vertex* pVertex1 = 0;

		// Create a normalized edge, in which the smallest vertex comes first.
		if (*p0 < *p1)
		{
			pVertex0 = p0;
			pVertex1 = p1;
		}
		else
		{
			pVertex0 = p1;
			pVertex1 = p0;
		}

		edge e(pVertex0, pVertex1);

		// Check if this edge is already in the buffer
		edgeIterator found = m_Edges.find(e);

		if (found == m_Edges.end()) m_Edges.insert(e);		// no, it isn't, so insert
		else m_Edges.erase(found);							// yes, it is, so erase it to eliminate double edges
	}

	cvIterator m_itVertex;
	edgeSet& m_Edges;
};

int  Delaunay::Triangulate(int num, double* dx, double* dy, double* dz, double*& tax, double*& tay, double*& taz, double*& tbx, double*& tby, double*& tbz, double*& tcx, double*& tcy, double*& tcz, int*& abk, int*& ack, int*& bck)
{
	vertexSet vertices;
	int i, retnum = 0;
	triangleSet output;
	for (i = 0; i < num; i++) {
		vertices.insert(vertex(dx[i], dy[i], dz[i]));
	}
	Triangulate(vertices, output, false);
	retnum = (int)output.size();
	tax = (double*)malloc(retnum * sizeof(double));
	tay = (double*)malloc(retnum * sizeof(double));
	taz = (double*)malloc(retnum * sizeof(double));
	tbx = (double*)malloc(retnum * sizeof(double));
	tby = (double*)malloc(retnum * sizeof(double));
	tbz = (double*)malloc(retnum * sizeof(double));
	tcx = (double*)malloc(retnum * sizeof(double));
	tcy = (double*)malloc(retnum * sizeof(double));
	tcz = (double*)malloc(retnum * sizeof(double));

	abk = (int*)malloc(retnum * sizeof(int));
	ack = (int*)malloc(retnum * sizeof(int));
	bck = (int*)malloc(retnum * sizeof(int));

	memset(abk, 2, sizeof(int) * retnum);
	memset(ack, 2, sizeof(int) * retnum);
	memset(bck, 2, sizeof(int) * retnum);

	int s = 0;
	for (triangleSet::iterator itr = output.begin(); itr != output.end(); ++itr)
	{
		tax[s] = (itr)->GetVertex(0)->GetX();
		tay[s] = (itr)->GetVertex(0)->GetY();
		taz[s] = (itr)->GetVertex(0)->GetValue();
		tbx[s] = (itr)->GetVertex(1)->GetX();
		tby[s] = (itr)->GetVertex(1)->GetY();
		tbz[s] = (itr)->GetVertex(1)->GetValue();
		tcx[s] = (itr)->GetVertex(2)->GetX();
		tcy[s] = (itr)->GetVertex(2)->GetY();
		tcz[s] = (itr)->GetVertex(2)->GetValue();
		s++;
	}
	return retnum;

}

int  Delaunay::Triangulate(int num, double* dx, double* dy, double* dz, double*& tax, double*& tay, double*& taz, double*& tbx, double*& tby, double*& tbz, double*& tcx, double*& tcy, double*& tcz)
{
	vertexSet vertices;
	int i, retnum = 0;
	triangleSet output;
	for (i = 0; i < num; i++) {
		vertices.insert(vertex(dx[i], dy[i], dz[i]));
	}
	Triangulate(vertices, output);
	retnum = (int)output.size();
	tax = (double*)malloc(retnum * sizeof(double));
	tay = (double*)malloc(retnum * sizeof(double));
	taz = (double*)malloc(retnum * sizeof(double));
	tbx = (double*)malloc(retnum * sizeof(double));
	tby = (double*)malloc(retnum * sizeof(double));
	tbz = (double*)malloc(retnum * sizeof(double));
	tcx = (double*)malloc(retnum * sizeof(double));
	tcy = (double*)malloc(retnum * sizeof(double));
	tcz = (double*)malloc(retnum * sizeof(double));

	int s = 0;
	for (triangleSet::iterator itr = output.begin(); itr != output.end(); ++itr)
	{
		tax[s] = (itr)->GetVertex(0)->GetX();
		tay[s] = (itr)->GetVertex(0)->GetY();
		taz[s] = (itr)->GetVertex(0)->GetValue();
		tbx[s] = (itr)->GetVertex(1)->GetX();
		tby[s] = (itr)->GetVertex(1)->GetY();
		tbz[s] = (itr)->GetVertex(1)->GetValue();
		tcx[s] = (itr)->GetVertex(2)->GetX();
		tcy[s] = (itr)->GetVertex(2)->GetY();
		tcz[s] = (itr)->GetVertex(2)->GetValue();
		s++;
	}
	return retnum;

}


void Delaunay::Triangulate(const vertexSet& vertices, triangleSet& output, bool bCreateHull)
{
	if (vertices.size() < 3) return;	// nothing to handle

	// Determine the bounding box.
	cvIterator itVertex = vertices.begin();

	double xMin = itVertex->GetX();
	double yMin = itVertex->GetY();
	double xMax = xMin;
	double yMax = yMin;

	++itVertex;		// If we're here, we know that vertices is not empty.
	for (; itVertex != vertices.end(); itVertex++)
	{
		double x = itVertex->GetX();
		double y = itVertex->GetY();
		if (x < xMin) xMin = x;
		if (x > xMax) xMax = x;
		if (y < yMin) yMin = y;
		if (y > yMax) yMax = y;
	}

	double dx = xMax - xMin;
	double dy = yMax - yMin;

	// Make the bounding box slightly bigger, just to feel safe.
	double ddx = dx * 0.2;
	double ddy = dy * 0.2;

	xMin -= ddx;
	xMax += ddx;
	dx += 2 * ddx;

	yMin -= ddy;
	yMax += ddy;
	dy += 2 * ddy;

	// 创建一个超级三角形,外圆周包含所有的点.这里选择一个水平方向为基准的等边三角形;
	// 我们可以建一个非常大的超级三角形,但是算法对四舍五入的错误敏感,所以最好建一个足够大的超级三角形,例如象我在这里做的.

	// Create a 'super triangle', encompassing all the vertices. We choose an equilateral triangle with horizontal base.
	// We could have made the 'super triangle' simply very big. However, the algorithm is quite sensitive to
	// rounding errors, so it's better to make the 'super triangle' just big enough, like we do here.
	vertex vSuper[3];

	vSuper[0] = vertex(xMin - dy * sqrt3 / 3.0F, yMin);	// Simple highschool geometry, believe me.
	vSuper[1] = vertex(xMax + dy * sqrt3 / 3.0F, yMin);
	vSuper[2] = vertex((xMin + xMax) * 0.5F, yMax + dx * sqrt3 * 0.5F);

	triangleSet workset;
	workset.insert(triangle(vSuper));

	covexHull CH;

	for (itVertex = vertices.begin(); itVertex != vertices.end(); itVertex++)
	{
		CH.addPoint(*itVertex);
		// 首先,从workset中移除所有完成的三角形.
		// 所谓完成的三角形就是它的外圆周完全在当前点的左边.
		// 点集是按X方向排序(由set容器自动排序)
		// 除非是超级三角形的部分, 否则复制完成三角形到output
		// 没有这个步骤这个算法也能工作, 但是对于大量点的时候,这是很重要的.

		// First, remove all 'completed' triangles from the workset.
		// A triangle is 'completed' if its circumcircle is entirely to the left of the current vertex.
		// Vertices are sorted in x-direction (the set container does this automagically).
		// Unless they are part of the 'super triangle', copy the 'completed' triangles to the output.
		// The algorithm also works without this step, but it is an important optimalization for bigger numbers of vertices.
		// It makes the algorithm about five times faster for 2000 vertices, and for 10000 vertices,
		// it's thirty times faster. For smaller numbers, the difference is negligible.
		tIterator itEnd = remove_if(workset.begin(), workset.end(), triangleIsCompleted(itVertex, output, vSuper));

		edgeSet edges;

		// 如果当前点在这个三角形圆周内的时候,该三角形是热点.
		// 删除所有的热点三角形,但是保留他们的圆周.
		// A triangle is 'hot' if the current vertex v is inside the circumcircle.
		// Remove all hot triangles, but keep their edges.
		itEnd = remove_if(workset.begin(), itEnd, vertexIsInCircumCircle(itVertex, edges));
		workset.erase(itEnd, workset.end());	// remove_if doesn't actually remove; we have to do this explicitly.

		// 根据当前点和圆周创建新的三角形
		// Create new triangles from the edges and the current vertex.
		for (edgeIterator it = edges.begin(); it != edges.end(); it++)
			workset.insert(triangle(it->m_pV0, it->m_pV1, &(*itVertex)));
	}


	// 最后,删除所有属于超级三角形的三角形, 移动剩余的三角形到output;
	// Finally, remove all the triangles belonging to the 'super triangle' and move the remaining
	// triangles tot the output; remove_copy_if lets us do that in one go.
	tIterator where = output.begin();
	remove_copy_if(workset.begin(), workset.end(), std::inserter(output, where), triangleHasVertex(vSuper));

	if (!bCreateHull)
	{
		return;
	}

	CH.compute();
	deque<vertex> Hull = CH.getHull();
	int len = (int)Hull.size();

	bool aIn, bIn;
	int i, j;

	vertex* A;
	vertex* B;


	//CFile File;
	//if(File.Open(".\\Hull.csv", CFile::modeCreate|CFile::modeWrite))
	//{
	//	CString text;
	//	for(i = 0 ; i < len; i++)
	//	{
	//		text.Format("%d, %f, %f, %f\r\n",i, Hull[i].GetX(), Hull[i].GetY(), Hull[i].GetValue());
	//		File.Write(text, text.GetLength());
	//	}

	//}
	//File.Close();

	for (i = 0; i < len; i++)
	{
		for (itVertex = vertices.begin(); itVertex != vertices.end(); itVertex++)
		{
			if (itVertex->GetIndex() == Hull[i].GetIndex())
			{
				A = (vertex*)(&(*itVertex));
			}
			if (itVertex->GetIndex() == Hull[(i + 1) % len].GetIndex())
			{
				B = (vertex*)(&(*itVertex));
			}
		}

		vector<int> indexA;
		vector<int> indexB;

		int CIndex = -1;

		bool bHave = false;

		tIterator itt = output.begin();
		for (; itt != output.end(); itt++)
		{
			aIn = false;
			bIn = false;
			for (j = 0; j < 3; j++)
			{
				if (itt->GetVertex(j)->GetIndex() == A->GetIndex())
				{
					aIn = true;
				}
				if (itt->GetVertex(j)->GetIndex() == B->GetIndex())
				{
					bIn = true;
				}
			}

			if (aIn && bIn)
			{
				bHave = true;
				break;
			}


			for (j = 0; j < 3; j++)
			{
				if (aIn && itt->GetVertex(j)->GetIndex() != A->GetIndex())
				{
					indexA.push_back(itt->GetVertex(j)->GetIndex());
				}
				if (bIn && itt->GetVertex(j)->GetIndex() != B->GetIndex())
				{
					indexB.push_back(itt->GetVertex(j)->GetIndex());
				}
			}

		}

		if (bHave)
		{
			continue;
		}

		bHave = false;
		for (vector<int>::iterator ia = indexA.begin(); ia != indexA.end(); ia++)
		{
			for (vector<int>::iterator ib = indexB.begin(); ib != indexB.end(); ib++)
			{
				if (*ia == *ib)
				{
					bHave = true;
					CIndex = *ia;
					break;
				}

			}
			if (bHave)
			{
				break;
			}

		}

		if (!bHave)
		{
			continue;
		}

		for (itVertex = vertices.begin(); itVertex != vertices.end(); itVertex++)
		{
			if (itVertex->GetIndex() == CIndex)
			{
				output.insert(triangle(A, B, &(*itVertex)));
			}
		}


	}

}

void Delaunay::Triangulate(const vertexSet& vertices, triangleSet& output)
{
	if (vertices.size() < 3) return;	// nothing to handle

	// Determine the bounding box.
	cvIterator itVertex = vertices.begin();

	REALD xMin = itVertex->GetX();
	REALD yMin = itVertex->GetY();
	REALD xMax = xMin;
	REALD yMax = yMin;

	++itVertex;		// If we're here, we know that vertices is not empty.
	for (; itVertex != vertices.end(); itVertex++)
	{
		REALD x = itVertex->GetX();
		REALD y = itVertex->GetY();
		if (x < xMin) xMin = x;
		if (x > xMax) xMax = x;
		if (y < yMin) yMin = y;
		if (y > yMax) yMax = y;
	}

	REALD dx = xMax - xMin;
	REALD dy = yMax - yMin;

	// Make the bounding box slightly bigger, just to feel safe.
	REALD ddx = dx * 0.2;
	REALD ddy = dy * 0.2;

	xMin -= ddx;
	xMax += ddx;
	dx += 2 * ddx;

	yMin -= ddy;
	yMax += ddy;
	dy += 2 * ddy;

	// 创建一个超级三角形,外圆周包含所有的点.这里选择一个水平方向为基准的等边三角形;
	// 我们可以建一个非常大的超级三角形,但是算法对四舍五入的错误敏感,所以最好建一个足够大的超级三角形,例如象我在这里做的.

	// Create a 'super triangle', encompassing all the vertices. We choose an equilateral triangle with horizontal base.
	// We could have made the 'super triangle' simply very big. However, the algorithm is quite sensitive to
	// rounding errors, so it's better to make the 'super triangle' just big enough, like we do here.
	vertex vSuper[3];

	vSuper[0] = vertex(xMin - dy * sqrt3 / 3.0F, yMin);	// Simple highschool geometry, believe me.
	vSuper[1] = vertex(xMax + dy * sqrt3 / 3.0F, yMin);
	vSuper[2] = vertex((xMin + xMax) * 0.5F, yMax + dx * sqrt3 * 0.5F);

	triangleSet workset;
	workset.insert(triangle(vSuper));

	for (itVertex = vertices.begin(); itVertex != vertices.end(); itVertex++)
	{
		// 首先,从workset中移除所有完成的三角形.
		// 所谓完成的三角形就是它的外圆周完全在当前点的左边.
		// 点集是按X方向排序(由set容器自动排序)
		// 除非是超级三角形的部分, 否则复制完成三角形到output
		// 没有这个步骤这个算法也能工作, 但是对于大量点的时候,这是很重要的.

		// First, remove all 'completed' triangles from the workset.
		// A triangle is 'completed' if its circumcircle is entirely to the left of the current vertex.
		// Vertices are sorted in x-direction (the set container does this automagically).
		// Unless they are part of the 'super triangle', copy the 'completed' triangles to the output.
		// The algorithm also works without this step, but it is an important optimalization for bigger numbers of vertices.
		// It makes the algorithm about five times faster for 2000 vertices, and for 10000 vertices,
		// it's thirty times faster. For smaller numbers, the difference is negligible.
		tIterator itEnd = remove_if(workset.begin(), workset.end(), triangleIsCompleted(itVertex, output, vSuper));

		edgeSet edges;

		// 如果当前点在这个三角形圆周内的时候,该三角形是热点.
		// 删除所有的热点三角形,但是保留他们的圆周.
		// A triangle is 'hot' if the current vertex v is inside the circumcircle.
		// Remove all hot triangles, but keep their edges.
		itEnd = remove_if(workset.begin(), itEnd, vertexIsInCircumCircle(itVertex, edges));
		workset.erase(itEnd, workset.end());	// remove_if doesn't actually remove; we have to do this explicitly.

		// 根据当前点和圆周创建新的三角形
		// Create new triangles from the edges and the current vertex.
		for (edgeIterator it = edges.begin(); it != edges.end(); it++)
			workset.insert(triangle(it->m_pV0, it->m_pV1, &(*itVertex)));
	}

	// 最后,删除所有属于超级三角形的三角形, 移动剩余的三角形到output;
	// Finally, remove all the triangles belonging to the 'super triangle' and move the remaining
	// triangles tot the output; remove_copy_if lets us do that in one go.
	tIterator where = output.begin();
	remove_copy_if(workset.begin(), workset.end(), std::inserter(output, where), triangleHasVertex(vSuper));
}


void Delaunay::TrianglesToEdges(const triangleSet& triangles, edgeSet& edges)
{
	for (ctIterator it = triangles.begin(); it != triangles.end(); ++it)
	{
		HandleEdge(it->GetVertex(0), it->GetVertex(1), edges);
		HandleEdge(it->GetVertex(1), it->GetVertex(2), edges);
		HandleEdge(it->GetVertex(2), it->GetVertex(0), edges);
	}
}

void Delaunay::HandleEdge(const vertex* p0, const vertex* p1, edgeSet& edges)
{
	const vertex* pV0 = 0;
	const vertex* pV1 = 0;

	if (*p0 < *p1)
	{
		pV0 = p0;
		pV1 = p1;
	}
	else
	{
		pV0 = p1;
		pV1 = p0;
	}

	// Insert a normalized edge. If it's already in edges, insertion will fail,
	// thus leaving only unique edges.
	edges.insert(edge(pV0, pV1));
}