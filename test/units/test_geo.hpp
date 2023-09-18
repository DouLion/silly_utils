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


BOOST_AUTO_TEST_CASE(GEO_INTERSECTION_CENTROID)
{
	std::cout << "\r\n\r\n****************" << "GEO_INTERSECTION_CENTROID" << "****************" << std::endl;

	// 读取湖南市区的环
	std::filesystem::path huanan(DEFAULT_DATA_DIR);
	huanan += "/read_shp/hunan_shi_boundary.shp";
	std::vector<silly_ring> shp_rings = geo_operation::read_shp_ring(huanan.string().c_str());
	if (shp_rings.empty())
	{
		std::cout << "Read huanan SHP error! " << std::endl;
	}

	// 计算预警区域外环
	std::filesystem::path risk2(DEFAULT_DATA_DIR);
	risk2 += "/read_shp/risk2.shp";
	std::vector<silly_ring> risk_rings = geo_operation::read_shp_ring(risk2.string().c_str());
	if (risk_rings.empty())
	{
		std::cout << "Read risk SHP error! " << std::endl;
	}
	int b = 0;
	// 排列组合求出相交区域
	std::vector<silly_ring> intersect_ring;
	for (int i = 0; i < risk_rings.size(); i++)
	{
		for (int j = 0; j < shp_rings.size(); j++)
		{
			// 求出相交区域
			std::vector<silly_ring> temp = geo_operation::intersect_area(risk_rings.at(i), shp_rings.at(j));
			for (int m = 0; m < temp.size(); m++)
			{
				if (!temp[m].points.empty())
				{
					std::cout << "j: " << j << std::endl;
					// 计算市的形心
					silly_point city_center = geo_operation::ring_to_center(shp_rings.at(j));
					std::cout << "city_center: " << "x:" << city_center.lgtd << "   y:" << city_center.lttd << std::endl;

					// 计算预警区域的形心
					silly_point intersect_center = geo_operation::ring_to_center(temp[m]);
					std::cout << "intersect_center: " << "x:" << intersect_center.lgtd << "   y:" << intersect_center.lttd << std::endl;

					// 测试画出相交区域和形心点(待删)
					{
						std::filesystem::path intersect(DEFAULT_DATA_DIR);
						char buff[256] = { 0 };
						sprintf(buff, "/read_shp/intersect_%d.shp", b);
						intersect += buff;
						// 画边界
						geo_operation::points_to_shp(temp[m].points, huanan.string().c_str(), intersect.string().c_str());

						std::filesystem::path intersect_center_path(DEFAULT_DATA_DIR);
						char buff2[256] = { 0 };
						sprintf(buff2, "/read_shp/intersect_center_%d.shp", b);
						b++;
						intersect_center_path += buff2;
						std::vector<silly_point> temp_inter{ intersect_center };
						//// 画形心点
						geo_operation::points_to_shp(temp_inter, huanan.string().c_str(), intersect_center_path.string().c_str());

					}


					// 计算两点夹角
					double azimuth = geo_operation::two_point_azimuth(city_center, intersect_center);

					// 判断方向
					std::cout << "azimuth: " << azimuth << std::endl;
					if (azimuth >= -15.0 && azimuth <= 15.0)
					{
						std::cout << "north" << std::endl;
					}
					else if (azimuth > 15.0 && azimuth < 75.0)
					{
						std::cout << "northeast" << std::endl;
					}
					else if (azimuth >= 75.0 && azimuth <= 105.0)
					{
						std::cout << "eastern" << std::endl;
					}
					else if (azimuth > 105.0 && azimuth < 165.0)
					{
						std::cout << "Southeast" << std::endl;
					}
					else if ((azimuth >= 165.0 && azimuth <= 180.0) || (azimuth >= -180.0 && azimuth <= -165.0))
					{
						std::cout << "south" << std::endl;
					}
					else if (azimuth > -165.0 && azimuth < -105.0)
					{
						std::cout << "southwest" << std::endl;
					}
					else if (azimuth >= -105.0 && azimuth <= -75.0)
					{
						std::cout << "west" << std::endl;
					}
					else if (azimuth > -75.0 && azimuth < -15.0)
					{
						std::cout << "Northwest" << std::endl;
					}

					//intersect_ring.push_back(temp);
					std::cout << "*********************************" << std::endl;
				}
			}


		}
	}

	int a = 0;
};

BOOST_AUTO_TEST_CASE(GEO_AZIMUTH)
{
	std::cout << "\r\n\r\n****************" << "GEO_AZIMUTH" << "****************" << std::endl;



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
	
	//bool gts = silly_geo_convert::geojson_to_shp(geojson_save.string().c_str(), shp_save.string().c_str());

	std::string geojson_2 = "D:/1_wangyingjie/code/project_data/geojson_shp/res2.geojson";
	std::filesystem::path geojson_2_save(geojson_2);
	bool gts = silly_geo_convert::shp_to_geojson(shp_save.string().c_str() , geojson_2_save.string().c_str());


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


