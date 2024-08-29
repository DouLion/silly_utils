/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/31 13:47
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#if I_NEED_TEST
#include <catch2/catch_test_macros.hpp>
#include "triangular/TFF_Delaunay.h"
#include "triangular/silly_delaunay.hpp"
#include "files/silly_file.h"
#include "geo/silly_geo.h"
#include "geo/silly_geo_utils.h"
#include "geo/silly_geojson.h"
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

TEST_CASE("TestTriangular")
{


SECTION("LAWSON_TRI")      // Delaunay 劳森算法
{
	std::string content;
	std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
	auto hebei_drp_data_path = data_root;
	hebei_drp_data_path.append("hebei_station.txt");
	silly_file::read(hebei_drp_data_path.string(), content);
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
	std::vector<silly_poly> polys;
	for (auto& tri : output)
	{
		silly_poly poly;
		poly.outer_ring.points.push_back({tri.GetVertex(0)->GetX(), tri.GetVertex(0)->GetY() });
		poly.outer_ring.points.push_back({ tri.GetVertex(1)->GetX(), tri.GetVertex(1)->GetY() });
		poly.outer_ring.points.push_back({ tri.GetVertex(2)->GetX(), tri.GetVertex(2)->GetY() });
		poly.outer_ring.points.push_back({ tri.GetVertex(0)->GetX(), tri.GetVertex(0)->GetY() });
		polys.push_back(poly);
	}
	std::string geojson = silly_geojson::dump_geojson(polys);
	auto tri_geojson_path = data_root;
	tri_geojson_path.append("hebei_tri.geojson");
        silly_file::write(tri_geojson_path.string(), geojson);
}

}

#endif //SILLY_UTILS_TEST_TRIANGULAR_H
