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

#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>
#include <datetime/silly_timer.h>
// #include "triangular/TFF_Delaunay.h"
// #include "triangular/silly_delaunay.hpp"
#include "triangular/silly_delaunay_sweep_line.h"
#include "triangular/silly_delaunay_bowyer.h"
#include "files/silly_file.h"
#include "geo/silly_geo.h"
#include "geo/silly_geo_utils.h"
#include "geo/silly_geojson.h"
std::vector<silly_dt_point>  points;
TEST_CASE("TestTriangular")
{

    SECTION("READ POINTS")
    {
        silly_timer timer;
        std::filesystem::path data_root(DEFAULT_SU_DATA_DIR);
        auto hebei_drp_data_path = data_root;
        hebei_drp_data_path.append("hebei_station.txt");
        points = silly_delaunay_utils::read(hebei_drp_data_path.string());
        SILLY_TIMER_COST_SEC(timer)
    }
    SECTION("SWEEP LINE")      // Delaunay 扫描线算法
    {

        silly_timer timer;
        /*silly_delaunay_sweep_line sdsl;
        sdsl.points(points);*/

        SILLY_TIMER_COST_SEC(timer)

    }

    SECTION("LAWSON BOWYER")      // Delaunay 劳森算法
    {

        silly_timer timer;
        silly_delaunay_bowyer sdd;
        sdd.points(points);
        sdd.triangulate();
        SILLY_TIMER_COST_SEC(timer)
        sdd.draw("LAWSON BOWYER.png");
        SILLY_TIMER_COST_SEC(timer)



    }


SECTION("LAWSON_TRI")      // Delaunay 劳森算法
{
	/*std::string content;
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
        silly_file::write(tri_geojson_path.string(), geojson);*/
}

}

#endif //SILLY_UTILS_TEST_TRIANGULAR_H
