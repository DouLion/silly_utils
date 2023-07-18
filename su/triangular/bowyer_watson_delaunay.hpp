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

namespace delaunay {

	constexpr double eps = 1e-6;

	template <typename T>
	struct Point {
		T x, y;
		T v;

		Point() : x{ 0 }, y{ 0 }, v{ 0 } {}
		Point(T _x, T _y, T _v) : x{ _x }, y{ _y }, v{ _v } {}

		template <typename U>
		Point(U _x, U _y, T _v) : x{ static_cast<T>(_x) }, y{ static_cast<T>(_y) }, v{ _v }
		{
		}

		friend std::ostream& operator<<(std::ostream& os, const Point<T>& p)
		{
			os << "x=" << p.x << "  y=" << p.y;
			return os;
		}

		bool operator==(const Point<T>& other) const
		{
			return (other.x == x && other.y == y);
		}

		bool operator!=(const Point<T>& other) const { return !operator==(other); }

		bool operator=(const Point<T>& other)
		{
			this->x = other.x;
			this->y = other.y;
			this->v = other.v;
			return *this;
		}
	};

	template <typename T>
	struct Edge {
		using Node = Point<T>;
		Node p0, p1;

		Edge(Node const& _p0, Node const& _p1) : p0{ _p0 }, p1{ _p1 } {}

		friend std::ostream& operator<<(std::ostream& os, const Edge& e)
		{
			os << "p0: [" << e.p0 << " ] p1: [" << e.p1 << "]";
			return os;
		}

		bool operator==(const Edge& other) const
		{
			return ((other.p0 == p0 && other.p1 == p1) ||
				(other.p0 == p1 && other.p1 == p0));
		}
	};

	template <typename T>
	struct Circle {
		T x, y, radius;
		Circle() = default;
	};

	template <typename T>
	struct Triangle {
		using Node = Point<T>;
		Node p0, p1, p2;
		Edge<T> e0, e1, e2;
		Circle<T> circle;

		Triangle(const Node& _p0, const Node& _p1, const Node& _p2)
			: p0{ _p0 },
			p1{ _p1 },
			p2{ _p2 },
			e0{ _p0, _p1 },
			e1{ _p1, _p2 },
			e2{ _p0, _p2 },
			circle{}
		{
			const auto ax = p1.x - p0.x;
			const auto ay = p1.y - p0.y;
			const auto bx = p2.x - p0.x;
			const auto by = p2.y - p0.y;

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

	template <typename T>
	struct Delaunay {
		std::vector<Triangle<T>> triangles;
		std::vector<Edge<T>> edges;
	};

	template <
		typename T,
		typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
	Delaunay<T> triangulate(const std::vector<Point<T>>& points)
	{
		using Node = Point<T>;
		if (points.size() < 3) {
			return Delaunay<T>{};
		}
		auto xmin = points[0].x;
		auto xmax = xmin;
		auto ymin = points[0].y;
		auto ymax = ymin;
		for (auto const& pt : points) {
			xmin = std::min(xmin, pt.x);
			xmax = std::max(xmax, pt.x);
			ymin = std::min(ymin, pt.y);
			ymax = std::max(ymax, pt.y);
		}

		const auto dx = xmax - xmin;
		const auto dy = ymax - ymin;
		const auto dmax = std::max(dx, dy);
		const auto midx = (xmin + xmax) / static_cast<T>(2.);
		const auto midy = (ymin + ymax) / static_cast<T>(2.);

		/* Init Delaunay triangulation. */
		auto d = Delaunay<T>{};

		const auto p0 = Node{ midx - 20 * dmax, midy - dmax };
		const auto p1 = Node{ midx, midy + 20 * dmax };
		const auto p2 = Node{ midx + 20 * dmax, midy - dmax };
		d.triangles.emplace_back(Triangle<T>{p0, p1, p2});

		for (auto const& pt : points) {
			std::vector<Edge<T>> edges;
			std::vector<Triangle<T>> tmps;
			for (auto const& tri : d.triangles) {
				/* Check if the point is inside the triangle circumcircle. */
				const auto dist = (tri.circle.x - pt.x) * (tri.circle.x - pt.x) +
					(tri.circle.y - pt.y) * (tri.circle.y - pt.y);
				if ((dist - tri.circle.radius) <= eps) {
					edges.push_back(tri.e0);
					edges.push_back(tri.e1);
					edges.push_back(tri.e2);
				}
				else {
					tmps.push_back(tri);
				}
			}

			/* Delete duplicate edges. */
			std::vector<bool> remove(edges.size(), false);
			for (auto it1 = edges.begin(); it1 != edges.end(); ++it1) {
				for (auto it2 = edges.begin(); it2 != edges.end(); ++it2) {
					if (it1 == it2) {
						continue;
					}
					if (*it1 == *it2) {
						remove[std::distance(edges.begin(), it1)] = true;
						remove[std::distance(edges.begin(), it2)] = true;
					}
				}
			}

			edges.erase(
				std::remove_if(edges.begin(), edges.end(),
					[&](auto const& e) { return remove[&e - &edges[0]]; }),
				edges.end());

			/* Update triangulation. */
			for (auto const& e : edges) {
				tmps.push_back({ e.p0, e.p1, {pt.x, pt.y} });
			}
			d.triangles = tmps;
		}

		/* Remove original super triangle. */
		d.triangles.erase(
			std::remove_if(d.triangles.begin(), d.triangles.end(),
				[&](auto const& tri) {
					return ((tri.p0 == p0 || tri.p1 == p0 || tri.p2 == p0) ||
						(tri.p0 == p1 || tri.p1 == p1 || tri.p2 == p1) ||
						(tri.p0 == p2 || tri.p1 == p2 || tri.p2 == p2));
				}),
			d.triangles.end());

		/* Add edges. */
		for (auto const& tri : d.triangles) {
			d.edges.push_back(tri.e0);
			d.edges.push_back(tri.e1);
			d.edges.push_back(tri.e2);
		}
		return d;
	}

	template <typename T>
	int  triangulate(int num, T* dx, T* dy, double* dz, T*& tax, T*& tay, double*& taz, T*& tbx, T*& tby, double*& tbz, T*& tcx, T*& tcy, double*& tcz)
	{
		std::vector<Point<T>> points;
		for (size_t i = 0; i < num; i++)
		{
			points.push_back(Point(dx[i], dy[i], dz[i]));
		}
		Delaunay<T> delaunay = triangulate(points);
		if (delaunay.triangles.empty())
		{
			return 0;
		}
		int retnum = delaunay.triangles.size();
		tax = (T*)malloc(retnum * sizeof(T));
		tay = (T*)malloc(retnum * sizeof(T));
		taz = (T*)malloc(retnum * sizeof(T));
		tbx = (T*)malloc(retnum * sizeof(T));
		tby = (T*)malloc(retnum * sizeof(T));
		tbz = (T*)malloc(retnum * sizeof(T));
		tcx = (T*)malloc(retnum * sizeof(T));
		tcy = (T*)malloc(retnum * sizeof(T));
		tcz = (T*)malloc(retnum * sizeof(T));
		for (size_t i = 0; i < delaunay.triangles.size(); i++)
		{
			tax[i] = delaunay.triangles[i].p0.x;
			tay[i] = delaunay.triangles[i].p0.y;
			taz[i] = delaunay.triangles[i].p0.v;
			tbx[i] = delaunay.triangles[i].p1.x;
			tby[i] = delaunay.triangles[i].p1.y;
			tbz[i] = delaunay.triangles[i].p1.v;
			tcx[i] = delaunay.triangles[i].p2.x;
			tcy[i] = delaunay.triangles[i].p2.y;
			tcz[i] = delaunay.triangles[i].p2.v;
		}

		return retnum;
	}

} /* namespace delaunay */