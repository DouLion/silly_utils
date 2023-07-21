/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/18 10:42
 * @version: 1.0.1
 * @software: silly_utils
 * @description: Bowyer-Watson algorithm C++ implementation of http://paulbourke.net/papers/triangulate .
 */
#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#ifndef REAL_EPSILON
#define REAL_EPSILON        1.192092896e-07F        /* FLT_EPSILON */
#endif
#define sqrt3 1.732050808F

#if defined(IS_LINUX) || defined(IS_APPLE)
#define REAL_EPSILON 1.192092896e-07F
#endif

#include <cairo/cairo.h>

namespace delaunay
{

	/// <summary>
	/// Bowyer-Watson 算法实现的三角化,目前效率有点问题
	/// 目前采用逐点插入方式生成的Delaunay三角网的算法主要基于Bowyer-Watson算法，Bowyer-Watson算法的主要步骤如下：
	/// 1）建立初始三角网格：针对给定的点集V, 找到一个包含该点集的矩形R, 我们称R为辅助窗口。连接R的任意一条对角线，形成两个三角形，作为初始Delaunay三角网格。
	/// 2）逐点插入：假设目前已经有一个Delaunay三角网格T, 现在在它里面再插入一个点P, 需要找到该点P所在的三角形。从P所在的三角形开始，
	/// 搜索该三角形的邻近三角形，并进行空外接圆检测。找到外接圆包含点P的所有的三角形并删除这些三角形，形成一个包含P的多边形空腔，
	/// 我们称之为Delaunay空腔。然后连接P与Delaunay腔的每一个顶点，形成新的Delaunay三角网格。
	///	3）删除辅助窗口R:重复步骤2）, 当点集V中所有点都已经插入到三角形网格中后，将顶点包含辅助窗口R的三角形全部删除。
	/// 在这些步骤中，快速定位点所在的三角形、确定点的影响并构建Delaunay腔的过程是每插入一个点都会进行的。随着点数的增加，三角形数目增加很快，因此缩短这两个过程的计算时间，是提高算法效率的关键。
	/// </summary>


	struct d_point
	{
		size_t i;    // 点的索引,帮助下面快速删除重复边
		double x, y;
		double v;

		d_point() : x{ 0 }, y{ 0 }, v{ 0 }, i{ 0 }
		{
		}

		d_point(double _x, double _y, double _v , size_t _i) : x{ _x }, y{ _y }, v{ _v }, i{ _i }
		{
		}

		bool operator==(const d_point& other) const
		{
			// return (other.x == x && other.y == y);
			return other.i == i;
		}

		bool operator!=(const d_point& other) const
		{
			return !operator==(other);
		}

		d_point& operator=(const d_point& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->v = other.v;
			this->i = other.i;
			return *this;
		}
	};


	struct d_edge
	{

		d_point p0, p1;
		// i0 为 p0 和p1中的较小的那个序号
		size_t i0{ 0 }, i1{0}; 

		d_edge()
		{
		}

		d_edge(d_point const& _p0, d_point const& _p1) : p0{ _p0 }, p1{ _p1 }
		{
			if (p0.i < p1.i)
			{
				i0 = p0.i;
				i1 = p1.i;
			}
			else
			{
				i1 = p0.i;
				i0 = p1.i;
			}
		}

		bool operator==(const d_edge& other) const
		{
			return i0 == other.i0 && i1 == other.i1;
		}
	};


	struct d_circle
	{
		double x, y, radius;

		d_circle() = default;
	};


	struct d_triangle
	{
		using d_point = d_point;
		d_point p0, p1, p2;
		d_edge e0, e1, e2;
		d_circle circle;

		d_triangle(const d_point& _p0, const d_point& _p1, const d_point& _p2)
				: p0{ _p0 },
				  p1{ _p1 },
				  p2{ _p2 },
				  e0{ _p0, _p1 },
				  e1{ _p1, _p2 },
				  e2{ _p0, _p2 },
				  circle{}
		{
			/*  圆心坐标公式
				x=((C1*B2)-(C2*B1))/((A1*B2)-(A2*B1))；
　　				y=((A1*C2)-(A2*C1))/((A1*B2)-(A2*B1))；
			*/
			const auto ax = p1.x - p0.x;
			const auto ay = p1.y - p0.y;
			const auto bx = p2.x - p0.x;
			const auto by = p2.y - p0.y;
			if (ax < REAL_EPSILON || ay < REAL_EPSILON || bx < REAL_EPSILON || by < REAL_EPSILON)
			{
				circle.radius = 0;
			}
			const auto m = p1.x * p1.x - p0.x * p0.x + p1.y * p1.y - p0.y * p0.y;
			const auto u = p2.x * p2.x - p0.x * p0.x + p2.y * p2.y - p0.y * p0.y;
			const auto s = 1. / (2. * (ax * by - ay * bx));
			circle.x = ((p2.y - p0.y) * m + (p0.y - p1.y) * u) * s;
			circle.y = ((p0.x - p2.x) * m + (p1.x - p0.x) * u) * s;

			const auto dx = p0.x - circle.x;
			const auto dy = p0.y - circle.y;
			circle.radius = dx * dx + dy * dy;
			
		}
	};


	struct d_delaunay
	{
		std::vector<d_triangle> triangles;
		std::vector<d_edge> edges;
	};

	d_delaunay triangulate(const std::vector<d_point>& points)
	{
		auto d = d_delaunay{};
		size_t pnum = points.size();
		if (pnum < 3)
		{
			return d;
		}
		auto xmin = points[0].x;
		auto xmax = xmin;
		auto ymin = points[0].y;
		auto ymax = ymin;
		for (auto const& pt: points)
		{

			xmin = std::min(xmin, pt.x);
			xmax = std::max(xmax, pt.x);
			ymin = std::min(ymin, pt.y);
			ymax = std::max(ymax, pt.y);
		}

		const auto dx = xmax - xmin;
		const auto dy = ymax - ymin;
		/*const auto dmax = std::max(dx, dy);
		const auto midx = (xmin + xmax) / 2.0;
		const auto midy = (ymin + ymax) / 2.0;*/

		xmin -= 0.1;    // 稍微外扩一点
		ymin -= 0.1;
		xmax += 0.1;
		ymax += 0.1;

		/* Init d_delaunay triangulation. */

		const auto p0 = d_point{ xmin, ymin, 0, pnum };    // 左下
		const auto p1 = d_point{ xmin, ymax, 0, pnum + 1 }; // 左上
		const auto p2 = d_point{ xmax, ymin, 0, pnum + 2 };    // 右下
		const auto p3 = d_point{ xmax, ymax, 0, pnum + 3 };    // 右上
		std::vector<d_point> n_points = points;
		n_points.push_back(p0);
		n_points.push_back(p1);
		n_points.push_back(p2);
		n_points.push_back(p3);

		// 左上 右下 以及其中一条对角线构建成的2个初始三角形
		d.triangles.emplace_back(d_triangle{ p0, p1, p2 });
		d.triangles.emplace_back(d_triangle{ p3, p1, p2 });
		// int iii = 0;
		for (auto const& pt: points)
		{
			// std::vector<d_edge> edges;
			std::map<int, std::map<int, int>> bad_edge_records;
			std::vector<d_triangle> tmps;
			int edge_num = 0;
			// 当前点在现有三角形外接圆的位置关系
			for (auto const& tri: d.triangles)
			{
				/* Check if the point is inside the triangle circum circle. */
				const double dist = (tri.circle.x - pt.x) * (tri.circle.x - pt.x) + (tri.circle.y - pt.y) * (tri.circle.y - pt.y);
				if (dist < tri.circle.radius)
				{ // 四点共圆, 不符合准则
					bad_edge_records[tri.e0.i0][tri.e0.i1]++;
					bad_edge_records[tri.e1.i0][tri.e1.i1]++;
					bad_edge_records[tri.e2.i0][tri.e2.i1]++;
					//// p1 p2和 p2 p1 是同一个边
					//edge_records[tri.e0.]
				}
				else
				{
					tmps.push_back(tri);
				}
			}
			int tri_num = tmps.size();

			int ignore_radius_cnt = 0;
			for (auto [i0, i1_c]: bad_edge_records)
			{
				for (auto [i1, c]: i1_c)
				{
					if (1 == c)
					{
						edge_num++;
						d_triangle ttt{ n_points[i0], n_points[i1], { pt.x, pt.y, pt.v, pt.i }};
						if (ttt.circle.radius==0)// 外接圆半径过小的就不处理了,可能点坐标就有问题
						{
							continue;
							ignore_radius_cnt++;
						}
						tmps.push_back({ ttt });
					}
				}
			}
			std::cout << pt.i << "," << tri_num << "," << edge_num << std::endl;
			d.triangles = tmps;
		}
		return d;
	}

} /* namespace delaunay */