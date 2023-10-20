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
#include "geo/silly_geo_utils.h"
#include "geo/silly_projection.h"
#include "geo/silly_geo_convert.h"
#include <filesystem>

#include "gdal_priv.h"
#include "ogrsf_frmts.h"

 /// <summary>
 /// 仅作为测试查看使用
 /// 读取一个shp文件,将多个坐标点绘制在读取的shp文件中的位置,并生成一个新的shp文件
 /// </summary>
 /// <param name="points">需要绘制的坐标点</param>
 /// <param name="shpFilePath">读取SHP文件地址</param>
 /// <param name="outputShpFilePath">写入SHP文件地址</param>
static bool points_to_shp(std::vector<silly_point>& points, const char* shpFilePath, const char* outputShpFilePath);

BOOST_AUTO_TEST_SUITE(TestGeo)

BOOST_AUTO_TEST_CASE(READ_VECTOR_POINT_LINE)
{
	std::cout << "\r\n\r\n****************" << "READ_VECTOR_POINT_LINE" << "****************" << std::endl;
	geo_utils::init_gdal_env();

	// 读取geojson的面
	std::filesystem::path geo_rings(DEFAULT_DATA_DIR);
	geo_rings += "/geojson/xian_poly.shp";
	// 读取geojson的线
	std::filesystem::path geo_line(DEFAULT_DATA_DIR);
	geo_line += "/geojson/river_line.geojson";
	// 读取geojson的点
	std::filesystem::path geo_point(DEFAULT_DATA_DIR);
	geo_point += "/geojson/xian_point.geojson";

	std::vector<silly_line> geo_lines_v = geo_utils::read_vector_lines(geo_line.string().c_str());


	std::vector<silly_multi_poly> geojson_r_2 = geo_utils::read_vector_polys(geo_rings.string().c_str());



	//std::vector<silly_multi_poly> geojson_r = geo_utils::read_vector_rings(geo_rings.string().c_str());





	std::vector<silly_poly> geojson_rings;
	enum_geometry_types type;
	std::map<std::string, std::string> properties;
	geo_utils::check_shp_info(geo_point.string().c_str(), type, properties);

	std::vector<silly_point> geo_points_v = geo_utils::read_vector_points(geo_point.string().c_str());


	enum_geometry_types type2;
	std::map<std::string, std::string> properties2;
	geo_utils::check_shp_info(geo_line.string().c_str(), type2, properties2);





	std::vector<silly_point> geojson_out_point;
	for (auto& poly : geojson_rings)
	{
		for (auto& point : poly.outer_ring.points)
		{
			geojson_out_point.push_back(point);
		}

		for (auto& inner_ring : poly.inner_rings)
		{
			for (auto& point : inner_ring.points)
			{
				geojson_out_point.push_back(point);
			}
		}
	}

	std::filesystem::path geojson_1013_1(DEFAULT_DATA_DIR);
	geojson_1013_1 += "/shp/1013_geojson_1.shp";
	points_to_shp(geojson_out_point, geo_line.string().c_str(), geojson_1013_1.string().c_str());


	std::filesystem::path shp_1(DEFAULT_DATA_DIR);
	shp_1 += "/shp/risk2.shp";

};




BOOST_AUTO_TEST_CASE(READ_GEOJSON_RINGS)
{
	std::cout << "\r\n\r\n****************" << "READ_GEOJSON_RINGS" << "****************" << std::endl;
	geo_utils::init_gdal_env();
	// 读取geojson的环
 	std::filesystem::path geofile_1(DEFAULT_DATA_DIR);
	geofile_1 += "/geojson/risk2.geojson";
	std::vector<silly_poly> geojson_rings; // = geo_utils::read_vector_rings(geofile_1.string().c_str());

	// 读取湖南市区的环
	std::filesystem::path huanan(DEFAULT_DATA_DIR);
	huanan += "/shp/hunan_shi_boundary.shp";

	std::vector<silly_point> geojson_out_point;
	for (auto& poly : geojson_rings)
	{
		for (auto& point : poly.outer_ring.points)
		{
			geojson_out_point.push_back(point);
		}

		for (auto& inner_ring : poly.inner_rings)
		{
			for (auto& point : inner_ring.points)
			{
				geojson_out_point.push_back(point);
			}
		}
	}

	std::filesystem::path geojson_1013_1(DEFAULT_DATA_DIR);
	geojson_1013_1 += "/shp/1013_geojson_1.shp";
	points_to_shp(geojson_out_point, huanan.string().c_str(), geojson_1013_1.string().c_str());


	std::filesystem::path shp_1(DEFAULT_DATA_DIR);
	shp_1 += "/shp/risk2.shp";
	std::vector<silly_poly> shp_rings;// = geo_utils::read_vector_rings(shp_1.string().c_str());
	std::vector<silly_point> shp_out_point;
	for (auto& poly : shp_rings)
	{
		for (auto& point : poly.outer_ring.points)
		{
			shp_out_point.push_back(point);
		}

		for (auto& inner_ring : poly.inner_rings)
		{
			for (auto& point : inner_ring.points)
			{
				shp_out_point.push_back(point);
			}
		}
	}
	std::filesystem::path shp_1013_1(DEFAULT_DATA_DIR);
	shp_1013_1 += "/shp/1013_shp_1.shp";
	points_to_shp(shp_out_point, huanan.string().c_str(), shp_1013_1.string().c_str());


	int b = 1;
	geojson_rings.clear();
	shp_rings.clear();
	geo_utils::destory_gdal_env();
	int a = 0;
};


//BOOST_AUTO_TEST_CASE(GEO_INTERSECTION_CENTROID)
//{
//	std::cout << "\r\n\r\n****************" << "GEO_INTERSECTION_CENTROID" << "****************" << std::endl;
//
//	// 读取湖南市区的环
//	std::filesystem::path huanan(DEFAULT_DATA_DIR);
//	huanan += "/read_shp/hunan_shi_boundary.shp";
//	std::vector<silly_ring> shp_rings = geo_operation::read_shp_ring(huanan.string().c_str());
//	if (shp_rings.empty())
//	{
//		std::cout << "Read huanan SHP error! " << std::endl;
//	}
//
//	// 计算预警区域外环
//	std::filesystem::path risk2(DEFAULT_DATA_DIR);
//	risk2 += "/read_shp/risk2.shp";
//	std::vector<silly_ring> risk_rings = geo_operation::read_shp_ring(risk2.string().c_str());
//	if (risk_rings.empty())
//	{
//		std::cout << "Read risk SHP error! " << std::endl;
//	}
//	int b = 0;
//	// 排列组合求出相交区域
//	std::vector<silly_ring> intersect_ring;
//	for (int i = 0; i < risk_rings.size(); i++)
//	{
//		for (int j = 0; j < shp_rings.size(); j++)
//		{
//			// 求出相交区域
//			std::vector<silly_ring> temp = geo_operation::intersect_area(risk_rings.at(i), shp_rings.at(j));
//			for (int m = 0; m < temp.size(); m++)
//			{
//				if (!temp[m].points.empty())
//				{
//					std::cout << "j: " << j << std::endl;
//					// 计算市的形心
//					silly_point city_center = geo_operation::ring_to_center(shp_rings.at(j));
//					std::cout << "city_center: " << "x:" << city_center.lgtd << "   y:" << city_center.lttd << std::endl;
//
//					// 计算预警区域的形心
//					silly_point intersect_center = geo_operation::ring_to_center(temp[m]);
//					std::cout << "intersect_center: " << "x:" << intersect_center.lgtd << "   y:" << intersect_center.lttd << std::endl;
//
//					// 测试画出相交区域和形心点(待删)
//					{
//						std::filesystem::path intersect(DEFAULT_DATA_DIR);
//						char buff[256] = { 0 };
//						sprintf(buff, "/read_shp/intersect_%d.shp", b);
//						intersect += buff;
//						// 画边界
//						points_to_shp(temp[m].points, huanan.string().c_str(), intersect.string().c_str());
//
//						std::filesystem::path intersect_center_path(DEFAULT_DATA_DIR);
//						char buff2[256] = { 0 };
//						sprintf(buff2, "/read_shp/intersect_center_%d.shp", b);
//						b++;
//						intersect_center_path += buff2;
//						std::vector<silly_point> temp_inter{ intersect_center };
//						//// 画形心点
//						points_to_shp(temp_inter, huanan.string().c_str(), intersect_center_path.string().c_str());
//
//					}
//
//
//					// 计算两点夹角
//					double azimuth = geo_operation::two_point_azimuth(city_center, intersect_center);
//
//					// 判断方向
//					std::cout << "azimuth: " << azimuth << std::endl;
//					if (azimuth >= -15.0 && azimuth <= 15.0)
//					{
//						std::cout << "north" << std::endl;
//					}
//					else if (azimuth > 15.0 && azimuth < 75.0)
//					{
//						std::cout << "northeast" << std::endl;
//					}
//					else if (azimuth >= 75.0 && azimuth <= 105.0)
//					{
//						std::cout << "eastern" << std::endl;
//					}
//					else if (azimuth > 105.0 && azimuth < 165.0)
//					{
//						std::cout << "Southeast" << std::endl;
//					}
//					else if ((azimuth >= 165.0 && azimuth <= 180.0) || (azimuth >= -180.0 && azimuth <= -165.0))
//					{
//						std::cout << "south" << std::endl;
//					}
//					else if (azimuth > -165.0 && azimuth < -105.0)
//					{
//						std::cout << "southwest" << std::endl;
//					}
//					else if (azimuth >= -105.0 && azimuth <= -75.0)
//					{
//						std::cout << "west" << std::endl;
//					}
//					else if (azimuth > -75.0 && azimuth < -15.0)
//					{
//						std::cout << "Northwest" << std::endl;
//					}
//
//					//intersect_ring.push_back(temp);
//					std::cout << "*********************************" << std::endl;
//				}
//			}
//
//
//		}
//	}
//
//	int a = 0;
//};
//
//BOOST_AUTO_TEST_CASE(GEO_AZIMUTH)
//{
//	std::cout << "\r\n\r\n****************" << "GEO_AZIMUTH" << "****************" << std::endl;
//
//
//
//	silly_ring square_1;
//	silly_ring square_2;
//	// 起始点左下角
//	square_1.points.push_back(silly_point(0, 0));
//	square_1.points.push_back(silly_point(3, 2));
//	square_1.points.push_back(silly_point(4, 5));
//	square_1.points.push_back(silly_point(1, 6));
//	square_2.points.push_back(silly_point(3, 0));
//	square_2.points.push_back(silly_point(7, 1));
//	square_2.points.push_back(silly_point(6, 4));
//	square_2.points.push_back(silly_point(3, 3));
//
//	// 左上角坐标系
//	//square_1.points.push_back(silly_point(0, -6));
//	//square_1.points.push_back(silly_point(3, -4));
//	//square_1.points.push_back(silly_point(4, -1));
//	//square_1.points.push_back(silly_point(1, 0));
//	//square_2.points.push_back(silly_point(3, -6));
//	//square_2.points.push_back(silly_point(7, -5));
//	//square_2.points.push_back(silly_point(6, -2));
//	//square_2.points.push_back(silly_point(3, -3));
//
//	silly_point center_1 = geo_operation::ring_to_center(square_1);
//	silly_point center_2 = geo_operation::ring_to_center(square_2);
//	silly_point from(0,0);
//	silly_point to(-1, 1.73);
//
//
//
//	double azimuth = geo_operation::two_point_azimuth(from, to);
//	std::cout << "azimuth: " << azimuth << std::endl;
//	if (azimuth >= -15.0 && azimuth <= 15.0)
//	{
//		std::cout<< "北部";
//	}
//	else if (azimuth > 15.0 && azimuth < 75.0)
//	{
//		std::cout<< "东北部";
//	}
//	else if (azimuth >= 75.0 && azimuth <= 105.0)
//	{
//		std::cout<< "东部";
//	}
//	else if (azimuth > 105.0 && azimuth < 165.0)
//	{
//		std::cout<< "东南部";
//	}
//	else if ((azimuth >= 165.0 && azimuth <= 180.0) || (azimuth >= -180.0 && azimuth <= -165.0))
//	{
//		std::cout<< "南部";
//	}
//	else if (azimuth > -165.0 && azimuth < -105.0)
//	{
//		std::cout<< "西南部";
//	}
//	else if (azimuth >= -105.0 && azimuth <= -75.0)
//	{
//		std::cout<< "西部";
//	}
//	else if (azimuth > -75.0 && azimuth < -15.0)
//	{
//		std::cout<< "西北部";
//	}
//
//	int a = 0;
//};

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


bool points_to_shp(std::vector<silly_point>& points, const char* shpFilePath, const char* outputShpFilePath)
{
#if IS_WIN32

	// 打开现有 shp 文件
	GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(shpFilePath, GDAL_OF_VECTOR | GDAL_OF_UPDATE, nullptr, nullptr, nullptr));
	if (dataset == nullptr)
	{
		// 处理文件打开失败的情况
		std::cout << "Failed to open shapefile." << std::endl;
		return false;
	}

	// 获取第一个图层
	OGRLayer* layer = dataset->GetLayer(0);
	if (layer == nullptr)
	{
		// 处理图层获取失败的情况
		std::cout << "Failed to get layer." << std::endl;
		GDALClose(dataset);
		return false;
	}

	// 创建新的输出 shp 文件
	GDALDriver* outDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
	GDALDataset* outputDataset = outDriver->Create(outputShpFilePath, 0, 0, 0, GDT_Unknown, nullptr);
	if (outputDataset == nullptr)
	{
		// 处理输出文件创建失败的情况
		std::cout << "Failed to create output shapefile." << std::endl;
		GDALClose(dataset);
		return false;
	}

	// 创建新的图层
	OGRLayer* outputLayer = outputDataset->CreateLayer("points", layer->GetSpatialRef(), wkbPoint, nullptr);
	if (outputLayer == nullptr)
	{
		// 处理图层创建失败的情况
		std::cout << "Failed to create output layer." << std::endl;
		GDALClose(dataset);
		GDALClose(outputDataset);
		return false;
	}

	// 定义并创建字段
	OGRFieldDefn fieldSize("Size", OFTReal);
	if (outputLayer->CreateField(&fieldSize) != OGRERR_NONE)
	{
		// 处理字段创建失败的情况
		std::cout << "Failed to create size field." << std::endl;
		GDALClose(dataset);
		GDALClose(outputDataset);
		return false;
	}

	OGRFieldDefn fieldColor("Color", OFTString);
	if (outputLayer->CreateField(&fieldColor) != OGRERR_NONE)
	{
		// 处理字段创建失败的情况
		std::cout << "Failed to create color field." << std::endl;
		GDALClose(dataset);
		GDALClose(outputDataset);
		return false;
	}

	// 创建要素并进行设置
	for (const auto& point : points)
	{
		OGRFeature* feature = OGRFeature::CreateFeature(outputLayer->GetLayerDefn());

		// 创建点对象
		OGRPoint ogrPoint;
		ogrPoint.setX(point.lgtd);
		ogrPoint.setY(point.lttd);

		// 设置点要素的几何对象
		feature->SetGeometry(&ogrPoint);


		// 将要素添加到图层
		if (outputLayer->CreateFeature(feature) != OGRERR_NONE)
		{
			// 处理要素添加失败的情况
			std::cout << "Failed to add feature." << std::endl;
			OGRFeature::DestroyFeature(feature);
			GDALClose(dataset);
			GDALClose(outputDataset);
			return false;
		}

		// 释放要素
		OGRFeature::DestroyFeature(feature);
	}

	// 关闭数据集
	GDALClose(dataset);
	GDALClose(outputDataset);

	std::cout << "Points added to shapefile and saved successfully." << std::endl;
#endif

	return true;
}