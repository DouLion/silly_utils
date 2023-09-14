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

BOOST_AUTO_TEST_CASE(GEO_READ)
{
	std::cout << "\r\n\r\n****************" << "GEO_READ" << "****************" << std::endl;

	std::string geo_1 = "D:/1_wangyingjie/code/project_data/read_shp2/risk2.geojson";
	std::filesystem::path geos_1(geo_1);


	// 计算预警区域外环
	std::string geo_shp = "D:/1_wangyingjie/code/project_data/read_shp2/risk2.shp";
	std::filesystem::path geo_shp_save(geo_shp);
	std::vector<silly_ring> risk_rings = geo_operation::read_shp_ring(geo_shp_save.string().c_str());
	if (risk_rings.empty())
	{
		std::cout << "Read risk SHP error! " << std::endl;
	}


	// 读取湖南市区的环
	std::string shp_1 = "D:/1_wangyingjie/code/project_data/read_shp/hunan_shi_boundary.shp";
	std::filesystem::path shp_path(shp_1);
	std::string shp_2 = "D:/1_wangyingjie/code/project_data/read_shp2/area_center_2.shp";
	std::filesystem::path shp_path2(shp_2);
	std::vector<silly_ring> shp_rings = geo_operation::read_shp_ring(shp_path.string().c_str());
	if (shp_rings.empty())
	{
		std::cout << "Read area SHP error! " << std::endl;
	}

	// 排列组合求出相交区域
	std::vector<silly_ring> intersect_ring;
	for (int i =0 ; i < risk_rings.size(); i++)
	{
		for (int j = 0; j < shp_rings.size(); j++)
		{
			silly_ring temp = geo_operation::intersect_area2(risk_rings.at(i), shp_rings.at(j));
			if (!temp.points.empty())
			{
				silly_point city_center = geo_operation::ring_to_center(shp_rings.at(j));
				silly_point intersect_center = geo_operation::ring_to_center(temp);

				double azimuth = geo_operation::two_point_azimuth(city_center, intersect_center);
				std::cout << "azimuth: " << azimuth << std::endl;
				if (azimuth >= -15.0 && azimuth <= 15.0)
				{
					std::cout << "北部" << std::endl;
				}
				else if (azimuth > 15.0 && azimuth < 75.0)
				{
					std::cout << "东北部" << std::endl;
				}
				else if (azimuth >= 75.0 && azimuth <= 105.0)
				{
					std::cout << "东部" << std::endl;
				}
				else if (azimuth > 105.0 && azimuth < 165.0)
				{
					std::cout << "东南部" << std::endl;
				}
				else if ((azimuth >= 165.0 && azimuth <= 180.0) || (azimuth >= -180.0 && azimuth <= -165.0))
				{
					std::cout << "南部" << std::endl;
				}
				else if (azimuth > -165.0 && azimuth < -105.0)
				{
					std::cout << "西南部" << std::endl;
				}
				else if (azimuth >= -105.0 && azimuth <= -75.0)
				{
					std::cout << "西部" << std::endl;
				}
				else if (azimuth > -75.0 && azimuth < -15.0)
				{
					std::cout << "西北部" << std::endl;
				}

				intersect_ring.push_back(temp);
			}

		}
	}

	// 求出相交区域的形心点并画出
	std::string intersect = "D:/1_wangyingjie/code/project_data/read_shp2/intersect_center_new_3.shp";
	std::filesystem::path intersect_path(intersect);
	std::vector<silly_point> intersect_centers;
	for (auto ir : intersect_ring)
	{
		silly_point center = geo_operation::ring_to_center(ir);
		intersect_centers.push_back(center);
	}
	geo_operation::points_to_shp(intersect_centers, shp_path.string().c_str(), intersect_path.string().c_str());


	// 计算市的形心点并画出
	std::vector<silly_point> centers;
	for (auto rs : shp_rings)
	{
		silly_point center = geo_operation::ring_to_center(rs);
		centers.push_back(center);
	}
	//geo_operation::points_to_shp(centers, shp_path.string().c_str(), shp_path2.string().c_str());



	silly_ring square_1;
	silly_ring square_2;
	// 起始点左下角
	square_1.points.push_back(silly_point(0, 0));
	square_1.points.push_back(silly_point(3, 2));
	square_1.points.push_back(silly_point(4, 5));
	square_1.points.push_back(silly_point(1, 6));
	square_2.points.push_back(silly_point(3, 0));
	square_2.points.push_back(silly_point(7, 1));
	square_2.points.push_back(silly_point(6, 4));
	square_2.points.push_back(silly_point(3, 3));

	// 左上角坐标系
	//square_1.points.push_back(silly_point(0, -6));
	//square_1.points.push_back(silly_point(3, -4));
	//square_1.points.push_back(silly_point(4, -1));
	//square_1.points.push_back(silly_point(1, 0));
	//square_2.points.push_back(silly_point(3, -6));
	//square_2.points.push_back(silly_point(7, -5));
	//square_2.points.push_back(silly_point(6, -2));
	//square_2.points.push_back(silly_point(3, -3));

	silly_point center_1 = geo_operation::ring_to_center(square_1);
	silly_point center_2 = geo_operation::ring_to_center(square_2);
	silly_point from(0,0);
	silly_point to(-1, 1.73);



	double azimuth = geo_operation::two_point_azimuth(from, to);
	std::cout << "azimuth: " << azimuth << std::endl;
	if (azimuth >= -15.0 && azimuth <= 15.0)
	{
		std::cout<< "北部";
	}
	else if (azimuth > 15.0 && azimuth < 75.0)
	{
		std::cout<< "东北部";
	}
	else if (azimuth >= 75.0 && azimuth <= 105.0)
	{
		std::cout<< "东部";
	}
	else if (azimuth > 105.0 && azimuth < 165.0)
	{
		std::cout<< "东南部";
	}
	else if ((azimuth >= 165.0 && azimuth <= 180.0) || (azimuth >= -180.0 && azimuth <= -165.0))
	{
		std::cout<< "南部";
	}
	else if (azimuth > -165.0 && azimuth < -105.0)
	{
		std::cout<< "西南部";
	}
	else if (azimuth >= -105.0 && azimuth <= -75.0)
	{
		std::cout<< "西部";
	}
	else if (azimuth > -75.0 && azimuth < -15.0)
	{
		std::cout<< "西北部";
	}

	int a = 0;
};

BOOST_AUTO_TEST_CASE(GEO_SHP_GEOJSON)
{
	std::cout << "\r\n\r\n****************" << "GEO_SHP_GEOJSON" << "****************" << std::endl;

	std::string geojson_1 = "D:/1_wangyingjie/code/project_data/geojson_shp/100KM.geojson";
	std::filesystem::path geojson_save(geojson_1);
	std::string shp_1 = "D:/1_wangyingjie/code/project_data/geojson_shp/100KM.shp";
	std::filesystem::path shp_save(shp_1);
	
	//bool gts = geo_operation::geojson_to_shp(geojson_save.string().c_str(), shp_save.string().c_str());

	std::string geojson_2 = "D:/1_wangyingjie/code/project_data/geojson_shp/res2.geojson";
	std::filesystem::path geojson_2_save(geojson_2);
	bool gts = geo_operation::shp_to_geojson(shp_save.string().c_str() , geojson_2_save.string().c_str());


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
