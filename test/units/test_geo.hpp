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
#include <sqlite3.h>
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include <spatialite.h>
#include <spatialite/gaiageo.h>
#include "geo/silly_spatialite.h"

#include <variant>

 /// <summary>
 /// 仅作为测试查看使用
 /// 读取一个shp文件,将多个坐标点绘制在读取的shp文件中的位置,并生成一个新的shp文件
 /// </summary>
 /// <param name="points">需要绘制的坐标点</param>
 /// <param name="shpFilePath">读取SHP文件地址</param>
 /// <param name="outputShpFilePath">写入SHP文件地址</param>
static bool points_to_shp(std::vector<silly_point>& points, const char* shpFilePath, const char* outputShpFilePath);

/// <summary>
/// 打印查看gaiaGeomCollPtr类型对象的类型和坐标点
/// </summary>
/// <param name="geom"></param>
static void geometry_printout(gaiaGeomCollPtr geom)
{
#if IS_WIN32
	gaiaPointPtr pt;
	gaiaLinestringPtr ln;
	gaiaPolygonPtr pg;
	gaiaRingPtr rng;
	int n_pts = 0, n_lns = 0, n_pgs = 0;
	int cnt, iv, ir;
	double x = 0.0, y = 0.0;
	// 计算点数
	pt = geom->FirstPoint;
	while (pt) {
		n_pts++;
		pt = pt->Next;
	}
	// 计算线串数
	ln = geom->FirstLinestring;
	while (ln) {
		n_lns++;
		ln = ln->Next;
	}
	// 计算多边形数
	pg = geom->FirstPolygon;
	while (pg) {
		n_pgs++;
		pg = pg->Next;
	}
	// 打印点坐标
	if (n_pts) {
		cnt = 0;
		pt = geom->FirstPoint;
		while (pt) {
			// 扫描POINT的链接列表
			std::cout << "\t\t\tPOINT " << cnt << "/" << n_pts << " x=" << pt->X << " y=" << pt->Y << std::endl;
			cnt++;
			pt = pt->Next;
		}
	}
	// 打印线串坐标
	if (n_lns) {
		cnt = 0;
		ln = geom->FirstLinestring;
		while (ln) {
			// 扫描线串的链接列表
			std::cout << "\t\t\tLINESTRING " << cnt << "/" << n_lns << " has " << ln->Points << " vertices" << std::endl;
			for (iv = 0; iv < ln->Points; iv++) {
				// 检索每个顶点的坐标
				gaiaGetPoint(ln->Coords, iv, &x, &y);
				std::cout << "\t\t\t\tvertex " << iv << "/" << ln->Points << " x=" << x << " y=" << y << std::endl;
			}
			cnt++;
			ln = ln->Next;
		}
	}
	// 打印多边形坐标
	if (n_pgs) {
		cnt = 0;
		pg = geom->FirstPolygon;
		while (pg) {
			// 扫描多边形的链接列表
			std::cout << "\t\t\tPOLYGON " << cnt << "/" << n_pgs << " has " << pg->NumInteriors << " hole";
			if (pg->NumInteriors != 1) {
				std::cout << "s";
			}
			std::cout << std::endl;
			// 打印外环
			rng = pg->Exterior;
			std::cout << "\t\t\t\tExteriorRing has " << rng->Points << " vertices" << std::endl;
			for (iv = 0; iv < rng->Points; iv++) {
				// 检索每个顶点的坐标
				gaiaGetPoint(rng->Coords, iv, &x, &y);
				std::cout << "\t\t\t\t\tvertex " << iv << "/" << rng->Points << " x=" << x << " y=" << y << std::endl;
			}
			for (ir = 0; ir < pg->NumInteriors; ir++) {
				// 多边形可以包含任意数量的内环（包括零）
				rng = pg->Interiors + ir;
				std::cout << "\t\t\t\tInteriorRing " << ir << "/" << pg->NumInteriors << " has " << rng->Points << " vertices" << std::endl;
				for (iv = 0; iv < rng->Points; iv++) {
					// 检索每个顶点的坐标
					gaiaGetPoint(rng->Coords, iv, &x, &y);
					std::cout << "\t\t\t\t\tvertex " << iv << "/" << rng->Points << " x=" << x << " y=" << y << std::endl;
				}
			}
			cnt++;
			pg = pg->Next;
		}
	}
#endif
}

/// <summary>
/// std::vector<geo_collection>假数据
/// </summary>
/// <param name="data"></param>
void createFakeData(std::vector<geo_collection>& data)
{
	// 创建多点数据
	geo_collection multiPointData;
	multiPointData.m_type = enum_geometry_types::eMultiPoint;
	multiPointData.m_m_points.push_back(silly_point(1.0, 1.0));
	multiPointData.m_m_points.push_back(silly_point(2.0, 2.0));
	multiPointData.m_m_points.push_back(silly_point(3.0, 3.0));
	multiPointData.m_m_points.push_back(silly_point(4.0, 4.0));
	data.push_back(multiPointData);

	// 创建单线数据
	geo_collection singleLineData;
	singleLineData.m_type = enum_geometry_types::eLineString;
	singleLineData.m_line.push_back(silly_point(1.0, 1.0));
	singleLineData.m_line.push_back(silly_point(2.0, 2.0));
	singleLineData.m_line.push_back(silly_point(3.0, 3.0));
	singleLineData.m_line.push_back(silly_point(4.0, 4.0));
	data.push_back(singleLineData);

	// 创建多线数据
	geo_collection multiLineData;
	multiLineData.m_type = enum_geometry_types::eMultiLineString;
	silly_line line1;
	line1.push_back(silly_point(1.0, 1.0));
	line1.push_back(silly_point(2.0, 2.0));
	line1.push_back(silly_point(3.0, 3.0));
	silly_line line2;
	line2.push_back(silly_point(4.0, 4.0));
	line2.push_back(silly_point(5.0, 5.0));
	line2.push_back(silly_point(6.0, 6.0));
	multiLineData.m_m_lines.push_back(line1);
	multiLineData.m_m_lines.push_back(line2);
	data.push_back(multiLineData);

	// 创建单面数据
	geo_collection singlePolygonData;
	singlePolygonData.m_type = enum_geometry_types::ePolygon;
	singlePolygonData.m_poly.outer_ring.points.push_back(silly_point(1.0, 1.0));
	singlePolygonData.m_poly.outer_ring.points.push_back(silly_point(2.0, 2.0));
	singlePolygonData.m_poly.outer_ring.points.push_back(silly_point(3.0, 3.0));
	singlePolygonData.m_poly.inner_rings.push_back(silly_ring());
	singlePolygonData.m_poly.inner_rings[0].points.push_back(silly_point(4.0, 4.0));
	singlePolygonData.m_poly.inner_rings[0].points.push_back(silly_point(5.0, 5.0));
	singlePolygonData.m_poly.inner_rings[0].points.push_back(silly_point(6.0, 6.0));
	data.push_back(singlePolygonData);

	// 创建多面数据
	geo_collection multiPolygonData;
	multiPolygonData.m_type = enum_geometry_types::eMultiPolygon;

	// 第一个面
	silly_poly poly1;
	poly1.outer_ring.points.push_back(silly_point(1.0, 1.0));
	poly1.outer_ring.points.push_back(silly_point(2.0, 2.0));
	poly1.outer_ring.points.push_back(silly_point(3.0, 3.0));
	poly1.inner_rings.push_back(silly_ring());  // 第一个内环
	poly1.inner_rings[0].points.push_back(silly_point(4.0, 4.0));
	poly1.inner_rings[0].points.push_back(silly_point(5.0, 5.0));
	poly1.inner_rings[0].points.push_back(silly_point(6.0, 6.0));
	poly1.inner_rings.push_back(silly_ring());  // 第二个内环
	poly1.inner_rings[1].points.push_back(silly_point(7.0, 7.0));
	poly1.inner_rings[1].points.push_back(silly_point(8.0, 8.0));
	poly1.inner_rings[1].points.push_back(silly_point(9.0, 9.0));

	// 第二个面
	silly_poly poly2;
	poly2.outer_ring.points.push_back(silly_point(10.0, 10.0));
	poly2.outer_ring.points.push_back(silly_point(11.0, 11.0));
	poly2.outer_ring.points.push_back(silly_point(12.0, 12.0));
	poly2.inner_rings.push_back(silly_ring());  // 第一个内环
	poly2.inner_rings[0].points.push_back(silly_point(13.0, 13.0));
	poly2.inner_rings[0].points.push_back(silly_point(14.0, 14.0));
	poly2.inner_rings[0].points.push_back(silly_point(15.0, 15.0));
	poly2.inner_rings.push_back(silly_ring());  // 第二个内环
	poly2.inner_rings[1].points.push_back(silly_point(16.0, 16.0));
	poly2.inner_rings[1].points.push_back(silly_point(17.0, 17.0));
	poly2.inner_rings[1].points.push_back(silly_point(18.0, 18.0));

	// 添加到多面数据中
	multiPolygonData.m_m_polys.push_back(poly1);
	multiPolygonData.m_m_polys.push_back(poly2);

	data.push_back(multiPolygonData);

}

class GeoJSONAttributeStorage
{
public:
	std::unordered_map<std::string, std::variant<std::string, int, double>> attributes;

	// 添加属性
	void AddAttribute(const std::string& name, const std::variant<std::string, int, double>& value)
	{
		attributes[name] = value;
	}

	// 获取属性值
	std::variant<std::string, int, double> GetAttributeValue(const std::string& name)
	{
		if (attributes.find(name) != attributes.end()) {
			return attributes[name];
		}
		return std::variant<std::string, int, double>();
	}
};

int test(std::string filename, GeoJSONAttributeStorage& attributeStorage)
{
	// 初始化GDAL
	GDALAllRegister();

	// 打开GeoJSON文件
	//const char* filename = "path_to_your_geojson_file.geojson";
	GDALDataset* dataset = (GDALDataset*)GDALOpenEx(filename.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL);

	if (dataset != NULL)
	{
		// 获取第一个图层（layer）
		OGRLayer* layer = dataset->GetLayer(0);
		if (layer != NULL)
		{
			// 创建存储对象
			//GeoJSONAttributeStorage attributeStorage;

			// 遍历所有要素（feature）
			layer->ResetReading();
			OGRFeature* feature;
			while ((feature = layer->GetNextFeature()) != NULL)
			{
				int fieldCount = feature->GetFieldCount();
				for (int i = 0; i < fieldCount; i++)
				{
					// 获取属性名和属性值
					OGRFieldDefn* fieldDef = feature->GetFieldDefnRef(i);
					std::string attributeName = fieldDef->GetNameRef();
					OGRFieldType fieldType = fieldDef->GetType();

					if (fieldType == OFTString)
					{
						const char* attributeValue = feature->GetFieldAsString(i);
						attributeStorage.AddAttribute(attributeName, attributeValue);
					}
					else if (fieldType == OFTInteger)
					{
						int attributeValue = feature->GetFieldAsInteger(i);
						attributeStorage.AddAttribute(attributeName, attributeValue);
					}
					else if (fieldType == OFTReal)
					{
						double attributeValue = feature->GetFieldAsDouble(i);
						attributeStorage.AddAttribute(attributeName, attributeValue);
					}
					// 处理其他属性类型...
				}

				OGRFeature::DestroyFeature(feature);
			}

			// 使用存储对象中的属性信息
			std::variant<std::string, int, double> attributeValue = attributeStorage.GetAttributeValue("name_of_attribute");
			if (std::holds_alternative<std::string>(attributeValue)) {
				std::string stringValue = std::get<std::string>(attributeValue);
				// 处理字符串属性值
			}
			else if (std::holds_alternative<int>(attributeValue)) {
				int intValue = std::get<int>(attributeValue);
				// 处理整型属性值
			}
			else if (std::holds_alternative<double>(attributeValue)) {
				double doubleValue = std::get<double>(attributeValue);
				// 处理浮点型属性值
			}
		}

		GDALClose(dataset);
	}
	else
	{
		printf("Failed to open GeoJSON file\n");
	}

	return 0;
}
BOOST_AUTO_TEST_SUITE(TestGeo)

BOOST_AUTO_TEST_CASE(ATTRIBUTE_STORAGE)
{
	std::cout << "\r\n\r\n****************" << "ATTRIBUTE_STORAGE" << "****************" << std::endl;

	// 读取geojson的线
	std::filesystem::path geo_line(DEFAULT_DATA_DIR);
	geo_line += "/geojson/river_line.geojson";
	
	GeoJSONAttributeStorage attributeStorage;

	int res = test(geo_line.string(), attributeStorage);
		
	int a = 0;



};

//BOOST_AUTO_TEST_CASE(SPATIALITE_DB)
//{
//	std::cout << "\r\n\r\n****************" << "SPATIALITE" << "****************" << std::endl;
//
//	std::filesystem::path geo_db(DEFAULT_DATA_DIR);
//	geo_db += "/geo_db/example6.db";
//
//	// // 假数据
//	std::vector<geo_collection> insert_data;
//	createFakeData(insert_data);
//
//		silly_spatialite ss;
//		ss.initialize(geo_db.string());
//
//		// 测试创建一个新表
//		std::string creatt = "CREATE TABLE IF NOT EXISTS test_date (id INTEGER PRIMARY KEY, geom BLOB);";
//		ss.create_table(creatt);
//		std::string selsql = "SELECT geom FROM test_date;";
//		std::vector<geo_collection> sel_geo0;
//		int selNum0 = ss.select_geo(sel_geo0, selsql);
//		std::cout << "插前: " << selNum0 << std::endl;
//
//		std::string inssql = "INSERT INTO test_date (geom) VALUES (?);";
//		int insNum = ss.insert_geo(insert_data, inssql);
//		std::cout << "插入:" << insNum << std::endl;
//
//		std::vector<geo_collection> sel_geo;
//		int selNum = ss.select_geo(sel_geo, selsql);
//		std::cout << "插后: " << selNum << std::endl;
//
//		std::string modifysql = "UPDATE test_date SET geom = ? WHERE id = 9 OR id = 10 OR id = 11";
//		int modNum = ss.modify_geo(insert_data[0], modifysql);
//		std::cout << "修改: " << modNum << std::endl;
//		std::vector<geo_collection> sel_geo2;
//		int selNum2 = ss.select_geo(sel_geo2, selsql);
//		std::cout << "改后: " << selNum2 << std::endl;
//
//
//		std::string remsql = "DELETE FROM test_date WHERE id = 14 OR id = 16;";
//		int remNum = ss.remove_geo(remsql);
//		std::cout << "删除: " << remNum << std::endl;
//		std::vector<geo_collection> sel_geo3;
//		int selNum3 = ss.select_geo(sel_geo3, selsql);
//		std::cout << "删后: " << selNum3 << std::endl;
//
//
//	int amm = 0;
//
//};


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

	enum_geometry_types type;
	std::map<std::string, std::string> properties;
	//geo_utils::check_shp_info(geo_point.string().c_str(), type, properties);


	int a = 0;

	//std::vector<silly_line> geo_lines_v = geo_utils::read_vector_lines(geo_line.string().c_str());


	//std::vector<silly_multi_poly> geojson_r_2 = geo_utils::read_vector_polys(geo_rings.string().c_str());

	//std::vector<silly_poly> geojson_rings;


	//int a = 0;
	//std::vector<silly_point> geo_points_v = geo_utils::read_vector_points(geo_point.string().c_str());


	//enum_geometry_types type2;
	//std::map<std::string, std::string> properties2;
	////geo_utils::check_shp_info(geo_line.string().c_str(), type2, properties2);

	//std::vector<silly_point> geojson_out_point;

	//std::filesystem::path geojson_1013_1(DEFAULT_DATA_DIR);
	//geojson_1013_1 += "/shp/1013_geojson_1.shp";
	////points_to_shp(geojson_out_point, geo_line.string().c_str(), geojson_1013_1.string().c_str());


	//std::filesystem::path shp_1(DEFAULT_DATA_DIR);
	//shp_1 += "/shp/risk2.shp";
	geo_utils::destory_gdal_env();
};



BOOST_AUTO_TEST_CASE(SILLY_TO_SPATIALITE)
{
	std::cout << "\r\n\r\n****************" << "SILLY_TO_SPATIALITE" << "****************" << std::endl;


	//// 创建silly中点线面的假数据
	//// 五个点-------------
	//std::vector<silly_point> milt_point;
	//milt_point.push_back(silly_point(1.0, 2.0));
	//milt_point.push_back(silly_point(3.0, 4.0));
	//milt_point.push_back(silly_point(5.0, 6.0));
	//milt_point.push_back(silly_point(7.0, 8.0));
	//milt_point.push_back(silly_point(9.0, 10.0));
	//gaiaGeomCollPtr gaia_point ;
	////bool aa = silly_point_to_gaiageo(milt_point[1], gaia_point);
	//geometry_printout(gaia_point);
	//silly_point re_point;
	////bool bb = gaiageo_to_silly_point(gaia_point, re_point);
	//std::cout << "=================================" << std::endl;

	//gaiaGeomCollPtr gaia_mult_point;
	////bool cc = silly_multi_point_to_gaiageo(milt_point, gaia_mult_point);
	//geometry_printout(gaia_mult_point);
	//silly_multi_point re_mul_point;
	////bool dd = gaiageo_to_silly_multi_point(gaia_mult_point, re_mul_point);
	//std::cout << "=================================" << std::endl;


	//// 一条线包含四个点--------------------
	//silly_line single_line;
	//single_line.push_back(silly_point(1.0, 2.0));
	//single_line.push_back(silly_point(3.0, 4.0));
	//single_line.push_back(silly_point(5.0, 6.0));
	//single_line.push_back(silly_point(7.0, 8.0));
	//gaiaGeomCollPtr gaia_single_line;
	////bool ee = silly_line_to_gaiageo(single_line, gaia_single_line);
	//geometry_printout(gaia_single_line);
	//silly_line res_single_line;
	////bool ff = gaiageo_to_silly_line(gaia_single_line, res_single_line);
	//std::cout << "=================================" << std::endl;


	//// 两条线，每条线包含四个点-----------------
	//silly_multi_silly_line multi_lines;
	//silly_line line1;
	//line1.push_back(silly_point(1.0, 2.0));
	//line1.push_back(silly_point(3.0, 4.0));
	//line1.push_back(silly_point(5.0, 6.0));
	//line1.push_back(silly_point(7.0, 8.0));
	//silly_line line2;
	//line2.push_back(silly_point(10.0, 20.0));
	//line2.push_back(silly_point(30.0, 40.0));
	//line2.push_back(silly_point(50.0, 60.0));
	//line2.push_back(silly_point(70.0, 80.0));
	//multi_lines.push_back(line1);
	//multi_lines.push_back(line2);
	//gaiaGeomCollPtr gaia_multi_line;
	////bool gg = silly_multi_silly_line_to_gaiageo(multi_lines, gaia_multi_line);
	//geometry_printout(gaia_multi_line);
	//silly_multi_silly_line re_multi_line;
	////bool hh = gaiageo_to_silly_multi_line(gaia_multi_line, re_multi_line);
	//std::cout << "=================================" << std::endl;

	//// 两个面----------------------------------
	//silly_multi_poly multi_polygons;
	//// 创建第一个面
	//silly_poly poly1;
	//poly1.outer_ring.points.push_back(silly_point(0, 0));
	//poly1.outer_ring.points.push_back(silly_point(10, 0));
	//poly1.outer_ring.points.push_back(silly_point(10, 10));
	//silly_ring innerRing1;
	//innerRing1.points.push_back(silly_point(2, 2));
	//innerRing1.points.push_back(silly_point(4, 2));
	//innerRing1.points.push_back(silly_point(3, 4));
	//silly_ring innerRing2;
	//innerRing2.points.push_back(silly_point(5, 5));
	//innerRing2.points.push_back(silly_point(7, 5));
	//innerRing2.points.push_back(silly_point(6, 7));
	//poly1.inner_rings.push_back(innerRing1);
	//poly1.inner_rings.push_back(innerRing2);
	//multi_polygons.push_back(poly1);
	//// 创建第二个面
	//silly_poly poly2;
	//poly2.outer_ring.points.push_back(silly_point(15, 15));
	//poly2.outer_ring.points.push_back(silly_point(25, 15));
	//poly2.outer_ring.points.push_back(silly_point(25, 25));
	//silly_ring innerRing3;
	//innerRing3.points.push_back(silly_point(17, 17));
	//innerRing3.points.push_back(silly_point(20, 17));
	//innerRing3.points.push_back(silly_point(19, 19));
	//silly_ring innerRing4;
	//innerRing4.points.push_back(silly_point(21, 21));
	//innerRing4.points.push_back(silly_point(23, 21));
	//innerRing4.points.push_back(silly_point(22, 23));
	//poly2.inner_rings.push_back(innerRing3);
	//poly2.inner_rings.push_back(innerRing4);
	//multi_polygons.push_back(poly2);

	//gaiaGeomCollPtr gaia_single_polygons;
	////bool ii = silly_poly_to_gaiageo(multi_polygons[0], gaia_single_polygons);
	//geometry_printout(gaia_single_polygons);
	//silly_poly res_single_polygons;
	////bool jj = gaiageo_to_silly_poly(gaia_single_polygons, res_single_polygons);
	//std::cout << "=================================" << std::endl;

	//gaiaGeomCollPtr gaia_multi_polygons;
	////bool kk = silly_multi_poly_to_gaiageo(multi_polygons, gaia_multi_polygons);
	//geometry_printout(gaia_multi_polygons);
	//silly_multi_poly res_multi_polygons;
	////bool ll = gaiageo_to_silly_multi_poly(gaia_multi_polygons, res_multi_polygons);
	//std::cout << "=================================" << std::endl;


	//std::cout << "0000000000000000000000000000000000000000000000000" << std::endl;




	//std::vector<gaiaGeomCollPtr> more_GeomColl;
	//more_GeomColl.push_back(gaia_milt_point);
	//more_GeomColl.push_back(gaia_single_line);
	//more_GeomColl.push_back(gaia_multi_line);
	//more_GeomColl.push_back(gaia_multi_polygons);

	//std::filesystem::path geo_db(DEFAULT_DATA_DIR);
	//geo_db += "/geo_db/example6.db";
	////silly_spatialite::insertSpatialData(geo_db.string().c_str(), more_GeomColl);

	//std::vector<gaiaGeomCollPtr> geo_res;
	//if (silly_spatialite::selectSpatialData(geo_db.string().c_str(), geo_res))
	//{
	//	std::cout << "Spatial data retrieved successfully!" << std::endl;
	//}
	//for (size_t i = 0; i < geo_res.size(); i++) {
	//	geometry_printout(geo_res[i]);
	//	std::cout << "11111111111111111111111111111111111111111111" << std::endl;
	//}
	//for (size_t i = 0; i < geo_res.size(); i++) {
	//	gaiaFreeGeomColl(geo_res[i]);
	//}





	//gaiaFreeGeomColl(gaia_milt_point);
	//gaiaFreeGeomColl(gaia_single_line);
	//gaiaFreeGeomColl(gaia_multi_line);
	//gaiaFreeGeomColl(gaia_multi_polygons);

	//int e = 0;


};




BOOST_AUTO_TEST_CASE(READ_GEOJSON_RINGS)
{
	std::cout << "\r\n\r\n****************" << "READ_GEOJSON_RINGS" << "****************" << std::endl;
	//geo_utils::init_gdal_env();
	//// 读取geojson的环
 //	std::filesystem::path geofile_1(DEFAULT_DATA_DIR);
	//geofile_1 += "/geojson/risk2.geojson";
	//std::vector<silly_poly> geojson_rings; // = geo_utils::read_vector_rings(geofile_1.string().c_str());

	//// 读取湖南市区的环
	//std::filesystem::path huanan(DEFAULT_DATA_DIR);
	//huanan += "/shp/hunan_shi_boundary.shp";

	//std::vector<silly_point> geojson_out_point;
	//for (auto& poly : geojson_rings)
	//{
	//	for (auto& point : poly.outer_ring.points)
	//	{
	//		geojson_out_point.push_back(point);
	//	}

	//	for (auto& inner_ring : poly.inner_rings)
	//	{
	//		for (auto& point : inner_ring.points)
	//		{
	//			geojson_out_point.push_back(point);
	//		}
	//	}
	//}

	//std::filesystem::path geojson_1013_1(DEFAULT_DATA_DIR);
	//geojson_1013_1 += "/shp/1013_geojson_1.shp";
	////points_to_shp(geojson_out_point, huanan.string().c_str(), geojson_1013_1.string().c_str());


	//std::filesystem::path shp_1(DEFAULT_DATA_DIR);
	//shp_1 += "/shp/risk2.shp";
	//std::vector<silly_poly> shp_rings;// = geo_utils::read_vector_rings(shp_1.string().c_str());
	//std::vector<silly_point> shp_out_point;
	//for (auto& poly : shp_rings)
	//{
	//	for (auto& point : poly.outer_ring.points)
	//	{
	//		shp_out_point.push_back(point);
	//	}

	//	for (auto& inner_ring : poly.inner_rings)
	//	{
	//		for (auto& point : inner_ring.points)
	//		{
	//			shp_out_point.push_back(point);
	//		}
	//	}
	//}
	//std::filesystem::path shp_1013_1(DEFAULT_DATA_DIR);
	//shp_1013_1 += "/shp/1013_shp_1.shp";
	////points_to_shp(shp_out_point, huanan.string().c_str(), shp_1013_1.string().c_str());


	//int b = 1;
	//geojson_rings.clear();
	//shp_rings.clear();
	//geo_utils::destory_gdal_env();
	//int a = 0;
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
	
	bool gts = silly_geo_convert::geojson_to_shp(geojson_save.string().c_str(), shp_save.string().c_str());

	std::string geojson_2 = "D:/1_wangyingjie/code/project_data/geojson_shp/res2.geojson";
	std::filesystem::path geojson_2_save(geojson_2);
	bool gts2 = silly_geo_convert::shp_to_geojson(shp_save.string().c_str() , geojson_2_save.string().c_str());


	int a = 0;
};



BOOST_AUTO_TEST_CASE(GEO_PROJECTION)
{
	std::cout << "\r\n\r\n****************" << "GEO_PROJECTION" << "****************" << std::endl;
	//double x1 = 109.98, x2 = 112.82, y1 = 29.88, y2 = 27.19;
	//double mx1, my1, mx2, my2;
	//silly_projection::geo_to_mercator(x1, y1, mx1, my1);
	//silly_projection::geo_to_mercator(x2, y2, mx2, my2);
	//std::cout << "Distance: " << sqrt((mx1 - mx2) * (mx1 - mx2) + (my1 - my2) * (my1 - my2)) / 1000.0 << "km" << std::endl;

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