#include <iostream>
#include <database/otl/otl_tools.h>
#include <compress/bz2_wrapper.h>
#include <files/TFF_FileUtils.h>
#include <assert.h>
#include <bzlib.h>
#include "marco.h"
#include "geo/silly_geo.h"
#include <triangular/silly_delaunay.hpp>
#include <triangular/TFF_Delaunay.h>
//#include <vld.h>
static char* GetDouble(char* q, double& v)
{
	char d[100] = { 0 };

	int i = 0;
	while ((*q) != ',')
	{
		d[i++] = *q;
		q++;
	}
	q++;
	d[i] = '\0';
	v = std::stod(d);
	return q;
}

std::vector<delaunay::d_point> get_points(const std::string& data)
{
	std::vector<delaunay::d_point> ret;
	std::vector<double> cvts;
	char* p = (char*)data.c_str();
	char* q = p;
	while (*q)
	{
		double tova;
		q = GetDouble(q, tova);
		cvts.push_back(tova);
	}

	int valNum = cvts.size();
	int nPNum = valNum / 3;

	for (size_t i = 0; i < nPNum; ++i)
	{
		ret.push_back({cvts[3 * i], cvts[3 * i + 1],cvts[3 * i + 2], i});

	}

	return ret;
}


int main()
{
	/*delaunay::d_point p0{0, 0, 0, 1}, p1{0,0, 0, 2}, p2{1,1, 0,3};
	delaunay::d_triangle tt{ p0, p1, p2 };
	double z1 = 10, z2 = 10;

	double a = 1 / (z1 - z2);
	bool c = std::isinf(a);*/

	std::string content;
	FileUtils::ReadAll("E:/hebei_station.txt", content);
	content.append(",\0");

	auto points = get_points(content);
	Delaunay DelaunayTri;
	//// ddx, ddy, ddz  ����������,ÿ�������� a b c ������, z ֵΪ?
	vertexSet vertices;
	for (auto& p:  points) {
		vertices.insert(vertex(p.x, p.y, p.v));
	}
	triangleSet output;
	DelaunayTri.Triangulate(vertices, output);
	delaunay::d_delaunay dln;// =  delaunay::triangulate(points);
	std::vector<silly_poly> polys;
	for (auto& tri : output)
	{
		silly_poly poly;
		poly.outer_ring.points.push_back({tri.GetVertex(0)->GetX(), tri.GetVertex(0)->GetY() });
		poly.outer_ring.points.push_back({ tri.GetVertex(1)->GetX(), tri.GetVertex(1)->GetY() });
		poly.outer_ring.points.push_back({ tri.GetVertex(2)->GetX(), tri.GetVertex(2)->GetY() });
		poly.outer_ring.points.push_back({ tri.GetVertex(0)->GetX(), tri.GetVertex(0)->GetY() });
		polys.push_back(poly);
		/*if (i == 15000)
		{
			break;
		}
		++i;
		if (i == 15421)
		{
			int a = 0;
		}*/
	}
	std::string geojson = silly_geo::dump_geojson(polys);
	FileUtils::WriteAll("E:/hebei_tri.geojson", geojson);

	return 0;
}