#pragma once 
#define BOOST_TEST_MAIN                 // 必须定义主测试套件，必须位于头文件之前
#define BOOST_TEST_STATIC_LINK

#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
using boost::unit_test_framework::test_suite;

#if IS_WIN32
#include <vld.h>
#endif

#include "test_jpeg.hpp"



//#include <iostream>
//#include <database/otl/otl_tools.h>
//#include <compress/bz2_wrapper.h>
//#include <files/TFF_FileUtils.h>
//#include <assert.h>
//#include <bzlib.h>
//#include "marco.h"
//#include "geo/silly_geo.h"
//#include <triangular/silly_delaunay.hpp>
//#include <triangular/TFF_Delaunay.h>
//#include <weather/netcdf_utils.h>
//#include <loguru/loguru.hpp>
//#include <database/otl/otl_header.hpp>
//
//#include <image/png_utils.h>
//#include <image/jpeg_utils.h>
//using namespace silly_image;
//
//
//static char* GetDouble(char* q, double& v)
//{
//	char d[100] = { 0 };
//
//	int i = 0;
//	while ((*q) != ',')
//	{
//		d[i++] = *q;
//		q++;
//	}
//	q++;
//	d[i] = '\0';
//	v = std::stod(d);
//	return q;
//}
//
//std::vector<delaunay::d_point> get_points(const std::string& data)
//{
//	std::vector<delaunay::d_point> ret;
//	std::vector<double> cvts;
//	char* p = (char*)data.c_str();
//	char* q = p;
//	while (*q)
//	{
//		double tova;
//		q = GetDouble(q, tova);
//		cvts.push_back(tova);
//	}
//
//	int valNum = cvts.size();
//	int nPNum = valNum / 3;
//
//	for (size_t i = 0; i < nPNum; ++i)
//	{
//		ret.push_back({cvts[3 * i], cvts[3 * i + 1],cvts[3 * i + 2], i});
//
//	}
//
//	return ret;
//}
//
//
//void dm8connect();
//
//int main()
//{
//	dm8connect();
//	return 0;
//	jpeg_utils ju;
//
//	// 创建空白灰色
//	{
//		jpeg_data temp_jpeg = ju.creat_empty_jpeg(300, 300, 1, JCS_GRAYSCALE);
//		std::string ru_1 = "./empty_gray.jpeg";
//		ju.write_jpeg_data(ru_1.c_str(), temp_jpeg);
//		temp_jpeg.release();
//
//	}
//
//	{
//		// 改变灰色某像素点坐标
//		std::string ru_4 = "./empty_gray.jpeg";
//		jpeg_data temp_jpeg_3 = ju.read_jpeg(ru_4.c_str());
//		std::string ru_5 = "./one_blace_gray.jpeg";
//		int row = 50;
//		int col = 30;
//		jpeg_pixel jp(0);
//		temp_jpeg_3.set_pixel(row, col, jp);
//		ju.write_jpeg_data(ru_5.c_str(), temp_jpeg_3);
//		temp_jpeg_3.release();
//
//	}
//
//	{
//		// 创建空白，并写出来
//		jpeg_data temp_jpeg_2 = ju.creat_empty_jpeg(300, 300, 3, JCS_RGB);
//		std::string ru_6 = "./empty_while.jpeg";
//		ju.write_jpeg_data(ru_6.c_str(), temp_jpeg_2);
//		temp_jpeg_2.release();
//	}
//
//
//	{
//		// 对边读一张图片，并另存
//		std::string ru_1 = "./1.jpeg";
//		jpeg_data temp_jpeg_2 = ju.read_jpeg(ru_1.c_str());
//		std::string ru_2 = "./color_2.jpeg";
//		ju.write_jpeg_data(ru_2.c_str(), temp_jpeg_2);
//		temp_jpeg_2.release();
//
//	}
//
//	{
//		// 改变某像素点坐标
//		std::string ru_4 = "./empty_while.jpeg";
//		jpeg_data temp_jpeg_3 = ju.read_jpeg(ru_4.c_str());
//		std::string ru_5 = "./one_blace.jpeg";
//		int row = 50;
//		int col = 30;
//		jpeg_pixel jp(0, 0, 0);
//		temp_jpeg_3.set_pixel(row, col, jp);
//		ju.write_jpeg_data(ru_5.c_str(), temp_jpeg_3);
//		temp_jpeg_3.release();
//
//	}
//
//	return 0;
//
//
//
//	png_data data = png_utils::create_empty(100, 100, png_type::eptRGBA);
//	for (int i = 0; i < data.width; ++i)
//	{
//		data.set_pixel(i, i,png_pixel(0xff, 0,0, 0xff));
//	}
//	data.set_pixel(200, 200, png_pixel(0xff, 0, 0, 0xff));
//	png_utils::write("./test_empty.png", data);
//	data.release();
//	SU_RETURN_OK
//	std::map<int, cv::Mat> cm;
//	nc_info ni;
//	netcdf_utils::read_netcdf("E:/ncf_20210624_065933.nc", "RATE_HYBRID", cm, ni);
//	nc_dim_info dimlat, dimlon;
//	dimlat.name = "Latitude";
//	dimlat.attributes["units"] = "degress_north";
//	dimlat.size = 200;
//
//	dimlon.name = "Longitude";
//	dimlon.attributes["units"] = "degress_north";
//	dimlon.size = 200;
//
//	ni.dims.push_back(dimlat);
//	ni.dims.push_back(dimlon);
//
//	netcdf_utils::write_netcdf("E:/fff.nc", ni, "ggf", cm[0]);
//	return 0;
//	/*delaunay::d_point p0{0, 0, 0, 1}, p1{0,0, 0, 2}, p2{1,1, 0,3};
//	delaunay::d_triangle tt{ p0, p1, p2 };
//	double z1 = 10, z2 = 10;
//
//	double a = 1 / (z1 - z2);
//	bool c = std::isinf(a);*/
//
//	std::string content;
//	FileUtils::ReadAll("E:/hebei_station.txt", content);
//	content.append(",\0");
//
//	auto points = get_points(content);
//	Delaunay DelaunayTri;
//	//// ddx, ddy, ddz  生成三角形,每个三角形 a b c 三个边, z 值为?
//	vertexSet vertices;
//	for (auto& p:  points) {
//		vertices.insert(vertex(p.x, p.y, p.v));
//	}
//	triangleSet output;
//	DelaunayTri.Triangulate(vertices, output);
//	delaunay::d_delaunay dln;// =  delaunay::triangulate(points);
//	std::vector<silly_poly> polys;
//	for (auto& tri : output)
//	{
//		silly_poly poly;
//		poly.outer_ring.points.push_back({tri.GetVertex(0)->GetX(), tri.GetVertex(0)->GetY() });
//		poly.outer_ring.points.push_back({ tri.GetVertex(1)->GetX(), tri.GetVertex(1)->GetY() });
//		poly.outer_ring.points.push_back({ tri.GetVertex(2)->GetX(), tri.GetVertex(2)->GetY() });
//		poly.outer_ring.points.push_back({ tri.GetVertex(0)->GetX(), tri.GetVertex(0)->GetY() });
//		polys.push_back(poly);
//		/*if (i == 15000)
//		{
//			break;
//		}
//		++i;
//		if (i == 15421)
//		{
//			int a = 0;
//		}*/
//	}
//	std::string geojson = silly_geo::dump_geojson(polys);
//	FileUtils::WriteAll("E:/hebei_tri.geojson", geojson);
//
//	return 0;
//}
//
//
//void dm8connect()
//{
//	otl_connect db;
//	otl_stream query_stream;
//	try
//	{
//
//		db.rlogon("Driver={DM8 ODBC DRIVER};Server=192.168.0.201;Port=5236;Database=TzxTest;Uid=TZX;Pwd=3edc9ijn~;", true);
//		//db.rlogon("UID=TZX;PWD=3edc9ijn~;DSN=dmtest;");
//
//		query_stream.open(2048, "select CITY_ID,CITY_NAME,REGION_ID from CITY", db);
//		for (auto& ts : query_stream)
//		{
//
//			otl_value<std::string> city_id, city_name;
//			otl_value<int> region_id;
//			otl_read_row(ts, city_id, city_name, region_id);
//			std::cout << city_id.v << "," << city_name.v << "," << region_id.v << std::endl;
//
//		}
//	}
//	catch (otl_exception& e)
//	{
//		LOG_F(ERROR, "%s失败, \nsql:%s \nmessage:%s \n state:%s", __FUNCTION__, e.stm_text, e.msg, e.sqlstate);
//	}
//	catch (const std::exception& p)
//	{
//		LOG_F(ERROR, "%s", p.what());
//	}
//	query_stream.close();
//	db.logoff();
//
//}