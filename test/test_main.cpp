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
#include <weather/netcdf_utils.h>
#if IS_WIN32
#include <vld.h>
#endif
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
	std::map<int, cv::Mat> cm;
	nc_info ni;
	netcdf_utils::read_netcdf("E:/ncf_20210624_065933.nc", "RATE_HYBRID", cm, ni);
	nc_dim_info dimlat, dimlon;
	dimlat.name = "Latitude";
	dimlat.attributes["units"] = "degress_north";
	dimlat.size = 200;

	dimlon.name = "Longitude";
	dimlon.attributes["units"] = "degress_north";
	dimlon.size = 200;

	ni.dims.push_back(dimlat);
	ni.dims.push_back(dimlon);

	netcdf_utils::write_netcdf("E:/fff.nc", ni, "ggf", cm[0]);
	return 0;
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
	//// ddx, ddy, ddz  生成三角形,每个三角形 a b c 三个边, z 值为?
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