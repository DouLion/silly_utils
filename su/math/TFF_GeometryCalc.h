#pragma once

#include "triangular/TFF_Delaunay.h"
#include "marco.h"

class PointN
{
public:
	double x{ 0 };
	double y{ 0 };

	PointN(double ax, double ay)
	{
		x = ax;
		y = ay;
	}

	void setCoord(double ax, double ay)
	{
		x = ax;
		y = ay;
	}

	PointN() = default;
};


class vertex;

class segcrossCalc
{
public:

	int dblcmp(double d)
	{
		if (fabs(d) < SU_EPS)
		{
			return 0;
		}

		return (d > 0) ? 1 : -1;
	}

	double det(double x1, double y1, double x2, double y2)
	{
		return x1 * y2 - x2 * y1;
	}

	double cross(PointN a, PointN b, PointN c)
	{
		return det(b.x - a.x, b.y - a.y, c.x - a.x, c.y - a.y);
	}

	double dotdet(double x1, double y1, double x2, double y2)
	{
		return x1 * x2 + y1 * y2;
	}

	double dot(PointN a, PointN b, PointN c)
	{
		return dotdet(b.x - a.x, b.y - a.y, c.x - a.x, c.y - a.y);
	}

	int betweenCmp(PointN a, PointN b, PointN c)
	{
		return dblcmp(dot(a, b, c));
	}

	bool coincident(PointN a, PointN b)
	{
		return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.x - b.x) < SU_EPS;
	}

	/*
	参数:	
	两条线段各自的两个端点
	一个指向点PointN的指针

	返回值:
	返回值为0时不相交
	返回值为1时规范相交,交点在r中 
	返回值为2时不规范相交
	*/
	int seg_cross(PointN& a, PointN& b, PointN& c, PointN& d, PointN* r)
	{
		double s1, s2, s3, s4;
		int d1, d2, d3, d4;

		d1 = dblcmp(s1 = cross(a, b, c));
		d2 = dblcmp(s2 = cross(a, b, d));
		d3 = dblcmp(s3 = cross(c, d, a));
		d4 = dblcmp(s4 = cross(c, d, b));

		if ((d1 ^ d2) == -2 && (d3 ^ d4) == -2)
		{
			r->x = (c.x * s2 - d.x * s1) / (s2 - s1);
			r->y = (c.y * s2 - d.y * s1) / (s2 - s1);
			return 1;
		}

		if ((d1 == 0 && betweenCmp(c, a, b) <= 0) ||
			(d2 == 0 && betweenCmp(d, a, b) <= 0) ||
			(d3 == 0 && betweenCmp(a, c, d) <= 0) ||
			(d4 == 0 && betweenCmp(b, c, d) <= 0))
		{
			if (d1 == 0)
			{
				r->x = c.x;
				r->y = c.y;
			}
			else if (d2 == 0)
			{
				r->x = d.x;
				r->y = d.y;
			}
			else if (d3 == 0)
			{
				r->x = a.x;
				r->y = a.y;
			}
			else if (d4 == 0)
			{
				r->x = b.x;
				r->y = b.y;
			}
			return 2;
		}

		return 0;
	}

	int seg_cross(const vertex& va, const vertex& vb, const vertex& vc, const vertex& vd, vertex& r)
	{
		PointN a, b, c, d, e;
		a.x = va.GetX();
		a.y = va.GetY();
		b.x = vb.GetX();
		b.y = vb.GetY();
		c.x = vc.GetX();
		c.y = vc.GetY();
		d.x = vd.GetX();
		d.y = vd.GetY();
		int iResult = seg_cross(a, b, c, d, &e);
		r.SetX(e.x);
		r.SetY(e.y);
		return iResult;
	}

};


class GeoArea
{
public:
	/// <summary>
	/// 此处两个接口从不返回绝对值,执行外环逆时针,内环顺时针的规定,方便内外环关系的判断以及带洞面的面积计算
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="N"></param>
	/// <returns></returns>
	static double PolyAreaX2(const double* x, const double* y, const int& N)
	{
		int i, j;
		double area = 0;

		for (i = 0; i < N; i++)
		{
			j = (i + 1) % N;
			area += x[i] * y[j];
			area -= y[i] * x[j];
		}

		return area;

	}

	static float PolyAreaX2(const float* x, const float* y, const int& N)
	{
		int i, j;
		float area = 0;

		for (i = 0; i < N; i++)
		{
			j = (i + 1) % N;
			area += x[i] * y[j];
			area -= y[i] * x[j];
		}

		return area;

	}
};


