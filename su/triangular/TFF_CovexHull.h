#pragma once

#include <vector>

#include <deque>

#include <algorithm>

#include <cstddef>

#include <cmath>

#include<functional>

using std::vector;

using std::deque;

#include "triangular/TFF_Delaunay.h"

class vertex;

class PointAndAngle
{//内部类，用于临时过程的处理（用于计算简单多边形）

public:

	vertex point;

	double angle;

	bool operator<(const PointAndAngle& p1) const
	{//小于运算符重载，用于排序

		return angle < p1.angle;

	}

	double len;

};

class covexHull
{

public:

	covexHull(void);

	~covexHull(void);


	void compute();//凸多边形计算的入口

	//void draw(CClientDC & dc );//绘制凸多边形

	void addPoint(const vertex& point);//添加计算的节点

	deque<vertex> getHull()
	{
		return m_stack;
	}


private:


	double GetDistance(vertex& A, vertex& B);

	void GetBoundary();

	std::vector<vertex>::iterator findLeftPoint();//找到最左边的点

	double computeS(const vertex& p1, const vertex& p2, const vertex& p3) const;//计算S

	void computeSimplePolygon();//计算简单多边形

	void computeCovexHull();//计算凸多边形

private:


	vector<vertex> m_points;//点集合（无序）

	deque<PointAndAngle> m_pointAndAngle;//简单多边形排序后点集合（有序）

	deque<vertex> m_stack;//凸多边形点集合（有序）



};


