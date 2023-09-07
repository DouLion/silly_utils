/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 18:06
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_TEST_GEO_HPP
#define SILLY_UTILS_TEST_GEO_HPP

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>
#include <iostream>
#include "geo/silly_projection.h"
#include "geo/silly_geo_operation.h"
#include "geo/silly_geo_convert.h"
#include <filesystem>

BOOST_AUTO_TEST_SUITE(TestGeo)


BOOST_AUTO_TEST_CASE(GEO_SHP_GEOJSON)
{
	std::cout << "\r\n\r\n****************" << "GEO_SHP_GEOJSON" << "****************" << std::endl;

	std::string geojson_1 = "D:/1_wangyingjie/code/project_data/geojson_shp/100KM.geojson";
	std::filesystem::path geojson_save(geojson_1);
	std::string shp_1 = "D:/1_wangyingjie/code/project_data/geojson_shp/100KM.shp";
	std::filesystem::path shp_save(shp_1);
	
	//bool gts = geo_operation::geojson_to_shp(geojson_save.string().c_str(), shp_save.string().c_str());

	std::string geojson_2 = "D:/1_wangyingjie/code/project_data/geojson_shp/res.geojson";
	std::filesystem::path geojson_2_save(geojson_2);
	bool gts = geo_operation::shp_to_geojson(shp_save.string().c_str() , geojson_2_save.string().c_str());


	int a = 0;
};



BOOST_AUTO_TEST_CASE(GEO_AZIMUTH)
{
	std::cout << "\r\n\r\n****************" << "GEO_AZIMUTH" << "****************" << std::endl;

	silly_ring square_1;
	silly_ring square_2;


	square_1.points.push_back(silly_point(0, 0));
	square_1.points.push_back(silly_point(3, 0));
	square_1.points.push_back(silly_point(3, 3));
	square_1.points.push_back(silly_point(0, 3));

	square_2.points.push_back(silly_point(3, 0));
	square_2.points.push_back(silly_point(6, 0));
	square_2.points.push_back(silly_point(6, 3));
	square_2.points.push_back(silly_point(3, 3));


	silly_point center_1 = geo_operation::ring_to_center(square_1);
	silly_point center_2 = geo_operation::ring_to_center(square_2);

	double azimuth = geo_operation::two_point_azimuth(center_1, center_2);
	std::cout << "azimuth: " << azimuth << std::endl;

	int a = 0;
};



BOOST_AUTO_TEST_CASE(GEO_PROJECTION)
{
	std::cout << "\r\n\r\n****************" << "GEO_PROJECTION" << "****************" << std::endl;
	double x1 = 109.98, x2 = 112.82, y1 = 29.88, y2 = 27.19;
	double mx1, my1, mx2, my2;
	silly_projection::geo_to_mercator(x1, y1, mx1, my1);
	silly_projection::geo_to_mercator(x2, y2, mx2, my2);
	std::cout << "Distance: " << sqrt((mx1 - mx2) * (mx1 - mx2) + (my1 - my2) * (my1 - my2)) / 1000.0 << "km" << std::endl;

};

BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_GEO_HPP
