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

#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>
#include "geo/silly_geo_utils.h"
#include "geo/silly_projection.h"
#include "geo/silly_geo_convert.h"
#include <sqlite3.h>
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include <spatialite.h>
#include <spatialite/gaiageo.h>
#include "geo/silly_spatialite.h"
#include "geo/silly_geo.h"
#include <variant>
#include <fstream>
#include "geo/silly_vector_to_raster.h"

TEST_CASE("TestGEO")
{

SECTION("SCANNING_LINE")
{
    std::cout << "\r\n\r\n****************"
              << "SCANNING_LINE"
              << "****************" << std::endl;
    geo_utils::init_gdal_env();
    std::filesystem::path geo_path(DEFAULT_SU_DATA_DIR);
    std::filesystem::path grid = geo_path;
    std::filesystem::path m_m_lines = geo_path;
    std::filesystem::path point_one = geo_path;
    std::filesystem::path point_more = geo_path;
    std::filesystem::path geo_line1 = geo_path;
    std::filesystem::path geo_line2 = geo_path;
    std::filesystem::path geo_poly1 = geo_path;
    std::filesystem::path geo_poly2 = geo_path;
    grid += "/geojson/grid.geojson";
    m_m_lines+="/geojson/src_mmlines.geojson";
    point_one += "/geojson/src_point.geojson";
    point_more += "/geojson/src_points.geojson";
    geo_line1 += "/geojson/src_line1.geojson";
    geo_line2 += "/geojson/src_line2.geojson";
    geo_poly1 += "/geojson/src_poly1.geojson";
    geo_poly2 += "/geojson/src_poly2.geojson";
    silly_geo_coll multiPolygonData;


    silly_multi_poly multi_poly;
    std::vector<silly_geo_coll> multiPolygonDataVec;

    // ============ 单点 ==================
    silly_point point0 = silly_point(113.32007, 29.51516);
    //multiPolygonData.m_type = enum_geometry_type::egtPoint;
    multiPolygonData.m_point = point0;

    // ============ 多点 ==================
    silly_point point1 = silly_point(113.34245, 29.51446);
    silly_point point2 = silly_point(113.38534, 29.49290);
    silly_point point3 = silly_point(113.38909, 29.46805);
    silly_point point4 = silly_point(113.36683, 29.45751);
    silly_point point5 = silly_point(113.33683, 29.46102);
    silly_point point6 = silly_point(113.28784, 29.49055);
    silly_point point7 = silly_point(113.28784, 29.51540);
    silly_point point8 = silly_point(113.41417, 29.46360);
    silly_point point9 = silly_point(113.38745, 29.51329);
    silly_point point10 = silly_point(113.41417, 29.51212);
    //multiPolygonData.m_type = enum_geometry_type::egtMultiPoint;
    std::vector<silly_point> ppxa {point1, point2, point3, point4, point5, point6, point7, point8, point9, point10};
    multiPolygonData.m_m_points = silly_multi_point(ppxa);
    
    // 创建第一条线
    silly_line line1 = silly_line(
    {
        silly_point(113.31176,29.51434), 
        silly_point(113.35944,29.51494), 
        silly_point(113.36510,29.49211), 
        silly_point(113.36409,29.48140), 
        silly_point(113.38672,29.46403)
    });

    // 创建第二条线
    silly_line line2(
    {
        silly_point(113.28711,29.43998), 
        silly_point(113.31236,29.43958), 
        silly_point(113.41096,29.48747), 
        silly_point(113.41319,29.44140) 
    });

    // ============ 单线 ==================
    multiPolygonData.m_line = line2;

    // ============ 多线 ==================
    multiPolygonData.m_m_lines.push_back(line1);
    multiPolygonData.m_m_lines.push_back(line2);


    // 第一个面
    silly_poly first_poly;
    first_poly.outer_ring.points = 
    {
        silly_point(113.30624, 29.48780), 
        silly_point(113.36182, 29.51534), 
        silly_point(113.41794, 29.49356), 
        silly_point(113.36582, 29.43773),
        silly_point(113.30624, 29.48780)
    };
    first_poly.outer_ring.is_outer = 1;

    // 第二个面
    silly_poly second_poly;
    second_poly.outer_ring.points = 
    {
        silly_point(113.28985, 29.44375), 
        silly_point(113.41716, 29.43845), 
        silly_point(113.43422, 29.41111), 
        silly_point(113.29779, 29.40876), 
        silly_point(113.28985, 29.44375)
    };
    second_poly.outer_ring.is_outer = 1;

    // ============ 单面 ==================
    multiPolygonData.m_poly = first_poly;

    // ============ 多面 ==================
     multiPolygonData.m_m_polys.push_back(first_poly);
     multiPolygonData.m_m_polys.push_back(second_poly);


    multiPolygonData.m_type = enum_geometry_type::egtMultiLineString;
    

    multiPolygonDataVec.push_back(multiPolygonData);
    //silly_geo_utils::write_geo_coll(m_m_lines.string(), multiPolygonDataVec);

    xscan_line_raster xlr;
 /*   xlr.top = 29.54244;
    xlr.bottom = 29.40021;
    xlr.left = 113.25921;
    xlr.right = 113.44758;
    xlr.cell_size = 0.025;*/
    xlr.rasterization(multiPolygonData);



    int mml = 0;

    //generateGridAndSave(grid.string().c_str(), xlr_mp.top, xlr_mp.bottom, xlr_mp.left, xlr_mp.right, xlr_mp.cell_size);

    geo_utils::destroy_gdal_env();

    int e = 0;
    int f = 9;
    int g = 8;
};


SECTION("READ_TEST_GEO_COLL")
{
    std::cout << "\r\n\r\n****************"
              << "READ_TEST_GEO_COLL"
              << "****************" << std::endl;
    geo_utils::init_gdal_env();

    // // 读点
    std::filesystem::path geo_points(DEFAULT_SU_DATA_DIR);
    geo_points += "/geojson/xian_point.geojson";
    std::filesystem::path geo_pPoint(DEFAULT_SU_DATA_DIR);
    geo_pPoint += "/geojson/ppoint.geojson";

    // // 读线
    std::filesystem::path geo_lines(DEFAULT_SU_DATA_DIR);
    geo_lines += "/geojson/river_line.geojson";
    std::filesystem::path geo_pLine(DEFAULT_SU_DATA_DIR);
    geo_pLine += "/geojson/plines.geojson";

    // // 读面
    std::filesystem::path geo_polys(DEFAULT_SU_DATA_DIR);
    geo_polys += "/geojson/xian_poly.geojson";
    std::filesystem::path geo_ppolys(DEFAULT_SU_DATA_DIR);
    geo_ppolys += "/geojson/ppoly.geojson";

    std::vector<silly_geo_coll> collection_xian;
    geo_utils::read_geo_coll(geo_polys.string().c_str(), collection_xian);

    // geo_utils::write_geo_coll(writ_coll_Polygon.string().c_str(), collection_xian);

    geo_utils::destroy_gdal_env();

    int e = 0;
    int f = 9;
    int g = 8;
};













//SECTION("INTERSECTION_AREA)
//{
//    std::cout << "\r\n\r\n****************"
//              << "INTERSECTION_AREA"
//              << "****************" << std::endl;
//    geo_utils::init_gdal_env();
//
//    std::vector<silly_geo_coll> area;
//    std::vector<silly_geo_coll> intersects;
//    silly_geo_coll main;
//    silly_geo_coll deputy;
//    silly_geo_coll temp;
//    silly_geo_coll inte;
//    silly_geo_coll inte2;
//
//     // 创建四个点
//    silly_point point1(110.471936116999998, 29.127132289799999);
//    silly_point point2(110.643753739999994, 27.814949836800000);
//    silly_point point3(111.243302495999998, 28.911672899199999);
//    silly_point point4(112.015486134000000, 28.868794475200001);
//    silly_poly myPoly;
//    myPoly.outer_ring.points.push_back(point1);
//    myPoly.outer_ring.points.push_back(point2);
//    myPoly.outer_ring.points.push_back(point3);
//    myPoly.outer_ring.points.push_back(point4);
//    main.m_poly = myPoly;
//    main.m_type = enum_geometry_type::egtPolygon;
//    area.push_back(main);
//
//    silly_point point5(110.240719904000002, 27.896483680300001);
//    silly_point point6(111.364970111999995, 28.297547021200000);
//    silly_point point7(111.895851020999999, 29.484422094100001);
//    silly_point point8(112.332973569999993, 27.773595216699999);
//    silly_poly myPoly2;
//    myPoly2.outer_ring.points.push_back(point5);
//    myPoly2.outer_ring.points.push_back(point6);
//    myPoly2.outer_ring.points.push_back(point7);
//    myPoly2.outer_ring.points.push_back(point8);
//    deputy.m_poly = myPoly2;
//    deputy.m_type = enum_geometry_type::egtPolygon;
//    area.push_back(deputy);
//
//    silly_point point9(112.347873938000006, 28.156742433800002);
//    silly_point point10(111.788640903000001, 27.742213901600000);
//    silly_point point11(112.332973569999993, 27.773595216699999);
//    silly_poly myPoly3;
//    myPoly3.outer_ring.points.push_back(point9);
//    myPoly3.outer_ring.points.push_back(point10);
//    myPoly3.outer_ring.points.push_back(point11);
//    temp.m_poly = myPoly3;
//    temp.m_type = enum_geometry_type::egtPolygon;
//    area.push_back(temp);
//    std::filesystem::path writ_coll_area(DEFAULT_SU_DATA_DIR);
//    writ_coll_area += "/shp/area_i.shp";
//    geo_utils::write_geo_coll(writ_coll_area.string().c_str(), area);
//
//    //inte = geo_utils::intersection_area(main.m_poly, deputy.m_poly);
//    inte2 = geo_utils::intersection_area(myPoly2, myPoly3);
//    //intersects.push_back(inte);
//    intersects.push_back(inte2);
//
//    std::filesystem::path writ_coll_inter(DEFAULT_SU_DATA_DIR);
//    writ_coll_inter += "/shp/inter2.shp";
//    //geo_utils::write_geo_coll(writ_coll_inter.string().c_str(), intersects);
//    geo_utils::destroy_gdal_env();
//
//    int e = 0;
//    int f = 9;
//    int g = 8;
//};

SECTION("READ_WRITE_SHP_SILLY_GEO_COLL")
{
    std::cout << "\r\n\r\n****************"
              << "READ_WRITE_SHP_SILLY_GEO_COLL"
              << "****************" << std::endl;
    geo_utils::init_gdal_env();

    // AA a;
    // SU_INFO_PRINT("Construct");
    // AA b = a;
    // SU_INFO_PRINT("=");
    // b = a;

    // silly_geo_prop ttt;
    // ttt.data=new  unsigned char[10];

    // 读取点  //测试完成
    std::filesystem::path point(DEFAULT_SU_DATA_DIR);
    point += "/shp/Point.shp";
    // 写入点
    std::filesystem::path outpoint(DEFAULT_SU_DATA_DIR);
    outpoint += "/shp/Point_out.shp";
    // 读取多面
    std::filesystem::path MultiPolygons(DEFAULT_SU_DATA_DIR);
    MultiPolygons += "/shp/risk2.shp";
    std::filesystem::path Mult(DEFAULT_SU_DATA_DIR);
    Mult += "/shp/multi.shp";

    // // 读线
    std::filesystem::path lines(DEFAULT_SU_DATA_DIR);
    lines += "/shp/lines_1.shp";
    std::filesystem::path pLine(DEFAULT_SU_DATA_DIR);
    pLine += "/shp/plines.shp";

    // 写面  已处理
    // 读取单面(包含单面和多面)  已处理可以完成
    std::filesystem::path Polygon(DEFAULT_SU_DATA_DIR);
    Polygon += "/shp/xian_poly.shp";

    std::filesystem::path writ_coll_Polygon(DEFAULT_SU_DATA_DIR);
    writ_coll_Polygon += "/shp/Polygons.shp";

    // // 读点
    std::filesystem::path geo_points(DEFAULT_SU_DATA_DIR);
    geo_points += "/geojson/xian_point.geojson";
    std::filesystem::path geo_pPoint(DEFAULT_SU_DATA_DIR);
    geo_pPoint += "/geojson/ppoint.geojson";

    // // 读线
    std::filesystem::path geo_lines(DEFAULT_SU_DATA_DIR);
    geo_lines += "/geojson/river_line.geojson";
    std::filesystem::path geo_pLine(DEFAULT_SU_DATA_DIR);
    geo_pLine += "/geojson/plines.geojson";

    // // 读面
    std::filesystem::path geo_polys(DEFAULT_SU_DATA_DIR);
    geo_polys += "/geojson/xian_poly.geojson";
    std::filesystem::path geo_ppolys(DEFAULT_SU_DATA_DIR);
    geo_ppolys += "/geojson/ppoly.geojson";

    std::vector<silly_geo_coll> collection_xian;
    geo_utils::read_geo_coll(Polygon.string().c_str(), collection_xian);

    //geo_utils::write_geo_coll(writ_coll_Polygon.string().c_str(), collection_xian);

    geo_utils::destroy_gdal_env();

    int e = 0;
    int f = 9;
    int g = 8;
};


SECTION("CENTER")
{
    std::cout << "\r\n\r\n****************"
              << "CENTER"
              << "****************" << std::endl;
    geo_utils::init_gdal_env();


    // 创建 OGRPolygon 对象
    OGRPolygon polygon;
    OGRLinearRing ring1;
    ring1.addPoint(112.572619199000002, 28.950052814700001);
    ring1.addPoint(112.704340303999999, 27.252274233300000);
    ring1.addPoint(112.601348185000006, 26.837947896900001);
    ring1.addPoint(112.536208962000003, 26.882303666999999);
    ring1.addPoint(111.223881457000005, 27.877093939400002);
    ring1.closeRings();
    // 添加第一个线性环到多边形
    polygon.addRing(&ring1);
    silly_geo_coll sgc_creat;
    sgc_creat.m_poly = geo_utils::silly_poly_from_ogr(&polygon);
    sgc_creat.m_type = enum_geometry_type::egtPolygon;
    std::filesystem::path creat_poly(DEFAULT_SU_DATA_DIR);
    creat_poly += "/shp/creat.shp";
    std::vector<silly_geo_coll> jia;
    jia.push_back(sgc_creat);
    geo_utils::write_geo_coll(creat_poly.string().c_str(), jia);



    std::filesystem::path Polygon(DEFAULT_SU_DATA_DIR);
    Polygon += "/shp/xian_poly.shp";
    std::vector<silly_geo_coll> collection_xian;
    geo_utils::read_geo_coll(Polygon.string().c_str(), collection_xian);
    std::vector<silly_geo_coll> out_center;
    std::vector<silly_geo_coll> out_xian;
    std::vector<silly_geo_coll> intersects;

    int e = 0;
    int n = 0;
    for (auto& xian : collection_xian)
    {
        if (xian.m_poly.inner_rings.size() > 0)
        {
           /* std::cout << "n:" << n << "  size: " << xian.m_poly.inner_rings.size() << std::endl;
            silly_point temp_point = geo_utils::poly_centroid(xian.m_poly);
            OGRPolygon onePoly = geo_utils::silly_poly_to_ogr(xian.m_poly);
            OGRPoint te_point;
            onePoly.Centroid(&te_point);
            OGRGeometry* inter = IntersectArea(onePoly, &polygon);
            {
                OGRwkbGeometryType geometryType = inter->getGeometryType();
                switch (geometryType)
                {
                        // 单环多边形
                    case wkbPolygon:
                    case wkbPolygon25D:
                    {
                        OGRPolygon* intersect = dynamic_cast<OGRPolygon*>(inter);
                        silly_geo_coll temp_poly;
                        temp_poly.m_poly = geo_utils::OGRPolyToSillyPoly(intersect);
                        temp_poly.m_type = enum_geometry_type::egtPolygon;
                        intersects.push_back(temp_poly);
                    }
                    break;

                    // 多个多边形
                    case wkbMultiPolygon:
                    case wkbMultiPolygon25D:
                    {
                        OGRMultiPolygon* intersectMulti = dynamic_cast<OGRMultiPolygon*>(inter);
                        silly_geo_coll temp_Multipoly;
                        temp_Multipoly.m_m_polys = geo_utils::OGRMulPolyToSillyMulPoly(intersectMulti);
                        temp_Multipoly.m_type = enum_geometry_type::egtMultiPolygon;
                        intersects.push_back(temp_Multipoly);
                    }
                    break;

                    default:
                        std::cout << "getGeometryType() : " << geometryType << "unable to process ";
                        break;
                }
            }
            OGRPoint point;
            inter->Centroid(&point);
            silly_geo_coll s_point;
            s_point.m_type = enum_geometry_type::egtPoint;
            s_point.m_point.lgtd = point.getX();
            s_point.m_point.lttd = point.getY();
            out_xian.push_back(xian);
            out_center.push_back(s_point);
            break;*/
        }
        n++;
    }


    std::filesystem::path writ_coll_point(DEFAULT_SU_DATA_DIR);
    writ_coll_point += "/shp/center_inter_point.shp";
    geo_utils::write_geo_coll(writ_coll_point.string().c_str(), out_center);

    std::filesystem::path writ_coll_Polygon(DEFAULT_SU_DATA_DIR);
    writ_coll_Polygon += "/shp/center_xian.shp";
    geo_utils::write_geo_coll(writ_coll_Polygon.string().c_str(), out_xian);

    std::filesystem::path writ_coll_inter(DEFAULT_SU_DATA_DIR);
    writ_coll_inter += "/shp/center_inter.shp";
    geo_utils::write_geo_coll(writ_coll_inter.string().c_str(), intersects);


    geo_utils::destroy_gdal_env();

    int f = 9;
    int g = 8;
};



class AA
{
  public:
    AA() = default;
    AA(const AA& other)
    {
        SU_DEBUG_PRINT("Construct");
        a = other.a;
        b = other.b;
    }

    AA& operator=(const AA& other)
    {
        SU_DEBUG_PRINT("operator=");
        this->a = other.a;
        this->b = other.b;
        return *this;
    }

  public:
    int a;
    int b;
};




SECTION("READ_WIRTE_JSON_GEO_COLL")
{
    std::cout << "\r\n\r\n****************"
              << "READ_WIRTE_JSON_GEO_COLL"
              << "****************" << std::endl;
    geo_utils::init_gdal_env();
    // std::vector<tem> temp;
    // otl_connect db; // 创建 otl_connect 对象，并连接到数据库
    // try
    //{
    //	db.rlogon("driver=sql server;server=192.168.0.151;port=1433;uid=sa;pwd=3edc9ijn~;database=TZX_RADAR");
    //	// 查询 WarningObject 表数据
    //	otl_stream warningObjectStream;
    //	warningObjectStream.open(512, "SELECT top 5 code, name, geojson FROM TZX_ANALYSE_VECTOR_R", db);
    //	for (auto& warn : warningObjectStream)
    //	{
    //		otl_value<std::string> code;
    //		otl_value<std::string> name;
    //		otl_value<otl_long_string> geojson;
    //		otl_read_row(warn, code, name, geojson);
    //		auto length = geojson.v.len();
    //		//std::string text(geojson., geojson.len());
    //		//std::cout << "Long Text: " << text << std::endl;
    //		tem ttt;
    //		ttt.code = code.v;
    //		ttt.name = name.v;
    //		//ttt.geojson = geojson.v;
    //		if (!ttt.geojson.empty())
    //		{
    //			//std::cout << "geojson: " << geojson.v << std::endl;
    //			temp.push_back(ttt);
    //		}
    //	}
    //	warningObjectStream.close(); // 关闭查询流
    //	db.logoff();
    // }
    // catch (otl_exception& e)
    //{
    //	std::cout << "select error" << std::endl;
    // }
    // catch (const std::exception& p)
    //{
    //	db.logoff();
    // }
    // db.logoff();

    // point  nulti_point   line  mulit_line  polygon muliti_polygon collection
    // 文件路径
    std::filesystem::path filePath(DEFAULT_SU_DATA_DIR);
    filePath += "/txt/mulit_line.txt";
    // 打开文件
    std::ifstream inputFile(filePath);
    // 检查文件是否成功打开
    if (!inputFile.is_open())
    {
        std::cout << "无法打开文件：" << filePath << std::endl;
    }
    // 从文件中读取数据并存储到std::string中
    std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    // 关闭文件
    inputFile.close();

    // 输出读取到的数据
    // std::cout << "文件内容：" << std::endl;
    // std::cout << fileContent << std::endl;

    std::string geo;

    silly_geo_coll temp_coll;
    // temp_coll = silly_geo::load_geo_coll(fileContent);

    // std::string result = silly_geo::dump_geo_coll(temp_coll);
    //  临时查看
    // std::cout << result << std::endl;

    geo_utils::destroy_gdal_env();

    int e = 0;
    int f = 9;
    int g = 8;
};

SECTION("READ_VECTOR_POINT_LINE")
{
    std::cout << "\r\n\r\n****************"
              << "READ_VECTOR_POINT_LINE"
              << "****************" << std::endl;
    geo_utils::init_gdal_env();

    // 读取geojson的面
    std::filesystem::path geo_rings(DEFAULT_SU_DATA_DIR);
    geo_rings += "/geojson/xian_poly.shp";
    // 读取geojson的线
    std::filesystem::path geo_line(DEFAULT_SU_DATA_DIR);
    geo_line += "/geojson/river_line.geojson";
    // 读取geojson的点
    std::filesystem::path geo_point(DEFAULT_SU_DATA_DIR);
    geo_point += "/geojson/xian_point.geojson";

    enum_geometry_type type;
    std::map<std::string, std::string> properties;
    // geo_utils::check_shp_info(geo_point.string().c_str(), type, properties);

    // std::vector<silly_line> geo_lines_v = geo_utils::read_vector_lines(geo_line.string().c_str());

    // std::vector<silly_multi_poly> geojson_r_2 = geo_utils::read_vector_polys(geo_rings.string().c_str());

    // std::vector<silly_poly> geojson_rings;

    // std::vector<silly_point> geo_points_v = geo_utils::read_vector_points(geo_point.string().c_str());

    enum_geometry_type type2;
    std::map<std::string, std::string> properties2;
    // geo_utils::check_shp_info(geo_line.string().c_str(), type2, properties2);

    silly_point point1(111.968714032999998, 28.454693591900000);
    silly_point point2(110.597357514999999, 28.566656802200001);
    silly_point point3(110.944020641999998, 27.341329958599999);
    silly_point point4(112.183755619999999, 27.461293052100000);
    std::vector<silly_point> geojson_out_point_1{point1, point2, point3, point4};

    silly_point point5(112.356701048999994, 27.115445539900001);
    silly_point point6(111.922501862999994, 26.480037323600001);
    silly_point point7(112.586770721999997, 25.864427972100000);
    silly_point point8(113.352719886000003, 26.569850828900002);
    std::vector<silly_point> geojson_out_point_2{point5, point6, point7, point8};
    std::vector<std::vector<silly_point>> rings{geojson_out_point_1, geojson_out_point_2};
    std::filesystem::path geojson_1013_1(DEFAULT_SU_DATA_DIR);
    geojson_1013_1 += "/shp/draw_line_1.shp";
    std::filesystem::path draw_rings(DEFAULT_SU_DATA_DIR);
    draw_rings += "/shp/draw_rings_1.shp";
    std::filesystem::path draw_points(DEFAULT_SU_DATA_DIR);
    draw_points += "/shp/points_1.shp";
    std::filesystem::path draw_lines(DEFAULT_SU_DATA_DIR);
    draw_lines += "/shp/lines_1.shp";

    std::vector<silly_line> lines{geojson_out_point_1, geojson_out_point_2};

    // lines_to_shp(lines, draw_lines.string().c_str());

    // points_to_shp(geojson_out_point_1, draw_points.string().c_str());

    // rings_to_shp(rings, draw_rings.string().c_str());

    // std::filesystem::path shp_1(DEFAULT_SU_DATA_DIR);
    // shp_1 += "/shp/risk2.shp";
    geo_utils::destroy_gdal_env();

    int a = 0;
    int b = 9;
    int c = 8;
};




SECTION("ATTRIBUTE_STORAGE")
{
    std::cout << "\r\n\r\n****************"
              << "ATTRIBUTE_STORAGE"
              << "****************" << std::endl;

    // 读取geojson的线
    std::filesystem::path geo_line(DEFAULT_SU_DATA_DIR);
    geo_line += "/geojson/river_line.geojson";

    // GeoJSONAttributeStorage attributeStorage;

    // int res = test(geo_line.string(), attributeStorage);

    int a = 0;
};

// SECTION("SPATIALITE_DB")
//{
//	std::cout << "\r\n\r\n****************" << "SPATIALITE" << "****************" << std::endl;
//
//	std::filesystem::path geo_db(DEFAULT_SU_DATA_DIR);
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
// };

SECTION("SILLY_TO_SPATIALITE")
{
    std::cout << "\r\n\r\n****************"
              << "SILLY_TO_SPATIALITE"
              << "****************" << std::endl;

    //// 创建silly中点线面的假数据
    //// 五个点-------------
    // std::vector<silly_point> milt_point;
    // milt_point.push_back(silly_point(1.0, 2.0));
    // milt_point.push_back(silly_point(3.0, 4.0));
    // milt_point.push_back(silly_point(5.0, 6.0));
    // milt_point.push_back(silly_point(7.0, 8.0));
    // milt_point.push_back(silly_point(9.0, 10.0));
    // gaiaGeomCollPtr gaia_point ;
    ////bool aa = silly_point_to_gaiageo(milt_point[1], gaia_point);
    // geometry_printout(gaia_point);
    // silly_point re_point;
    ////bool bb = gaiageo_to_silly_point(gaia_point, re_point);
    // std::cout << "=================================" << std::endl;

    // gaiaGeomCollPtr gaia_mult_point;
    ////bool cc = silly_multi_point_to_gaiageo(milt_point, gaia_mult_point);
    // geometry_printout(gaia_mult_point);
    // silly_multi_point re_mul_point;
    ////bool dd = gaiageo_to_silly_multi_point(gaia_mult_point, re_mul_point);
    // std::cout << "=================================" << std::endl;

    //// 一条线包含四个点--------------------
    // silly_line single_line;
    // single_line.push_back(silly_point(1.0, 2.0));
    // single_line.push_back(silly_point(3.0, 4.0));
    // single_line.push_back(silly_point(5.0, 6.0));
    // single_line.push_back(silly_point(7.0, 8.0));
    // gaiaGeomCollPtr gaia_single_line;
    ////bool ee = silly_line_to_gaiageo(single_line, gaia_single_line);
    // geometry_printout(gaia_single_line);
    // silly_line res_single_line;
    ////bool ff = gaiageo_to_silly_line(gaia_single_line, res_single_line);
    // std::cout << "=================================" << std::endl;

    //// 两条线，每条线包含四个点-----------------
    // silly_multi_silly_line multi_lines;
    // silly_line line1;
    // line1.push_back(silly_point(1.0, 2.0));
    // line1.push_back(silly_point(3.0, 4.0));
    // line1.push_back(silly_point(5.0, 6.0));
    // line1.push_back(silly_point(7.0, 8.0));
    // silly_line line2;
    // line2.push_back(silly_point(10.0, 20.0));
    // line2.push_back(silly_point(30.0, 40.0));
    // line2.push_back(silly_point(50.0, 60.0));
    // line2.push_back(silly_point(70.0, 80.0));
    // multi_lines.push_back(line1);
    // multi_lines.push_back(line2);
    // gaiaGeomCollPtr gaia_multi_line;
    ////bool gg = silly_multi_silly_line_to_gaiageo(multi_lines, gaia_multi_line);
    // geometry_printout(gaia_multi_line);
    // silly_multi_silly_line re_multi_line;
    ////bool hh = gaiageo_to_silly_multi_line(gaia_multi_line, re_multi_line);
    // std::cout << "=================================" << std::endl;

    //// 两个面----------------------------------
    // silly_multi_poly multi_polygons;
    //// 创建第一个面
    // silly_poly poly1;
    // poly1.outer_ring.points.push_back(silly_point(0, 0));
    // poly1.outer_ring.points.push_back(silly_point(10, 0));
    // poly1.outer_ring.points.push_back(silly_point(10, 10));
    // silly_ring innerRing1;
    // innerRing1.points.push_back(silly_point(2, 2));
    // innerRing1.points.push_back(silly_point(4, 2));
    // innerRing1.points.push_back(silly_point(3, 4));
    // silly_ring innerRing2;
    // innerRing2.points.push_back(silly_point(5, 5));
    // innerRing2.points.push_back(silly_point(7, 5));
    // innerRing2.points.push_back(silly_point(6, 7));
    // poly1.inner_rings.push_back(innerRing1);
    // poly1.inner_rings.push_back(innerRing2);
    // multi_polygons.push_back(poly1);
    //// 创建第二个面
    // silly_poly poly2;
    // poly2.outer_ring.points.push_back(silly_point(15, 15));
    // poly2.outer_ring.points.push_back(silly_point(25, 15));
    // poly2.outer_ring.points.push_back(silly_point(25, 25));
    // silly_ring innerRing3;
    // innerRing3.points.push_back(silly_point(17, 17));
    // innerRing3.points.push_back(silly_point(20, 17));
    // innerRing3.points.push_back(silly_point(19, 19));
    // silly_ring innerRing4;
    // innerRing4.points.push_back(silly_point(21, 21));
    // innerRing4.points.push_back(silly_point(23, 21));
    // innerRing4.points.push_back(silly_point(22, 23));
    // poly2.inner_rings.push_back(innerRing3);
    // poly2.inner_rings.push_back(innerRing4);
    // multi_polygons.push_back(poly2);

    // gaiaGeomCollPtr gaia_single_polygons;
    ////bool ii = silly_poly_to_gaiageo(multi_polygons[0], gaia_single_polygons);
    // geometry_printout(gaia_single_polygons);
    // silly_poly res_single_polygons;
    ////bool jj = gaiageo_to_silly_poly(gaia_single_polygons, res_single_polygons);
    // std::cout << "=================================" << std::endl;

    // gaiaGeomCollPtr gaia_multi_polygons;
    ////bool kk = silly_multi_poly_to_gaiageo(multi_polygons, gaia_multi_polygons);
    // geometry_printout(gaia_multi_polygons);
    // silly_multi_poly res_multi_polygons;
    ////bool ll = gaiageo_to_silly_multi_poly(gaia_multi_polygons, res_multi_polygons);
    // std::cout << "=================================" << std::endl;

    // std::cout << "0000000000000000000000000000000000000000000000000" << std::endl;

    // std::vector<gaiaGeomCollPtr> more_GeomColl;
    // more_GeomColl.push_back(gaia_milt_point);
    // more_GeomColl.push_back(gaia_single_line);
    // more_GeomColl.push_back(gaia_multi_line);
    // more_GeomColl.push_back(gaia_multi_polygons);

    // std::filesystem::path geo_db(DEFAULT_SU_DATA_DIR);
    // geo_db += "/geo_db/example6.db";
    ////silly_spatialite::insertSpatialData(geo_db.string().c_str(), more_GeomColl);

    // std::vector<gaiaGeomCollPtr> geo_res;
    // if (silly_spatialite::selectSpatialData(geo_db.string().c_str(), geo_res))
    //{
    //	std::cout << "Spatial data retrieved successfully!" << std::endl;
    // }
    // for (size_t i = 0; i < geo_res.size(); i++) {
    //	geometry_printout(geo_res[i]);
    //	std::cout << "11111111111111111111111111111111111111111111" << std::endl;
    // }
    // for (size_t i = 0; i < geo_res.size(); i++) {
    //	gaiaFreeGeomColl(geo_res[i]);
    // }

    // gaiaFreeGeomColl(gaia_milt_point);
    // gaiaFreeGeomColl(gaia_single_line);
    // gaiaFreeGeomColl(gaia_multi_line);
    // gaiaFreeGeomColl(gaia_multi_polygons);

    // int e = 0;
}

SECTION("READ_GEOJSON_RINGS")
{
    std::cout << "\r\n\r\n****************"
              << "READ_GEOJSON_RINGS"
              << "****************" << std::endl;
    // geo_utils::init_gdal_env();
    //// 读取geojson的环
    //	std::filesystem::path geofile_1(DEFAULT_SU_DATA_DIR);
    // geofile_1 += "/geojson/risk2.geojson";
    // std::vector<silly_poly> geojson_rings; // = geo_utils::read_vector_rings(geofile_1.string().c_str());

    //// 读取湖南市区的环
    // std::filesystem::path huanan(DEFAULT_SU_DATA_DIR);
    // huanan += "/shp/hunan_shi_boundary.shp";

    // std::vector<silly_point> geojson_out_point;
    // for (auto& poly : geojson_rings)
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

    // std::filesystem::path geojson_1013_1(DEFAULT_SU_DATA_DIR);
    // geojson_1013_1 += "/shp/1013_geojson_1.shp";
    ////points_to_shp(geojson_out_point, huanan.string().c_str(), geojson_1013_1.string().c_str());

    // std::filesystem::path shp_1(DEFAULT_SU_DATA_DIR);
    // shp_1 += "/shp/risk2.shp";
    // std::vector<silly_poly> shp_rings;// = geo_utils::read_vector_rings(shp_1.string().c_str());
    // std::vector<silly_point> shp_out_point;
    // for (auto& poly : shp_rings)
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
    // std::filesystem::path shp_1013_1(DEFAULT_SU_DATA_DIR);
    // shp_1013_1 += "/shp/1013_shp_1.shp";
    ////points_to_shp(shp_out_point, huanan.string().c_str(), shp_1013_1.string().c_str());

    // int b = 1;
    // geojson_rings.clear();
    // shp_rings.clear();
    // geo_utils::destory_gdal_env();
    // int a = 0;
};

// SECTION("GEO_INTERSECTION_CENTROID)
//{
//	std::cout << "\r\n\r\n****************" << "GEO_INTERSECTION_CENTROID" << "****************" << std::endl;
//
//	// 读取湖南市区的环
//	std::filesystem::path huanan(DEFAULT_SU_DATA_DIR);
//	huanan += "/read_shp/hunan_shi_boundary.shp";
//	std::vector<silly_ring> shp_rings = geo_operation::read_shp_ring(huanan.string().c_str());
//	if (shp_rings.empty())
//	{
//		std::cout << "Read huanan SHP error! " << std::endl;
//	}
//
//	// 计算预警区域外环
//	std::filesystem::path risk2(DEFAULT_SU_DATA_DIR);
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
//						std::filesystem::path intersect(DEFAULT_SU_DATA_DIR);
//						char buff[256] = { 0 };
//						sprintf(buff, "/read_shp/intersect_%d.shp", b);
//						intersect += buff;
//						// 画边界
//						points_to_shp(temp[m].points, huanan.string().c_str(), intersect.string().c_str());
//
//						std::filesystem::path intersect_center_path(DEFAULT_SU_DATA_DIR);
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
// };
//
// SECTION("GEO_AZIMUTH)
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
// };

SECTION("GEO_SHP_GEOJSON")
{
    std::cout << "\r\n\r\n****************"
              << "GEO_SHP_GEOJSON"
              << "****************" << std::endl;

    std::string geojson_1 = "D:/1_wangyingjie/code/project_data/geojson_shp/100KM.geojson";
    std::filesystem::path geojson_save(geojson_1);
    std::string shp_1 = "D:/1_wangyingjie/code/project_data/geojson_shp/100KM.shp";
    std::filesystem::path shp_save(shp_1);

    // bool gts = silly_geo_convert::geojson_to_shp(geojson_save.string().c_str(), shp_save.string().c_str());

    // std::string geojson_2 = "D:/1_wangyingjie/code/project_data/geojson_shp/res2.geojson";
    // std::filesystem::path geojson_2_save(geojson_2);
    // bool gts2 = silly_geo_convert::shp_to_geojson(shp_save.string().c_str() , geojson_2_save.string().c_str());

    int a = 0;
}

SECTION("GEO_PROJECTION")
{
    std::cout << "\r\n\r\n****************"
              << "GEO_PROJECTION"
              << "****************" << std::endl;
    // double x1 = 109.98, x2 = 112.82, y1 = 29.88, y2 = 27.19;
    // double mx1, my1, mx2, my2;
    // silly_projection::geo_to_mercator(x1, y1, mx1, my1);
    // silly_projection::geo_to_mercator(x2, y2, mx2, my2);
    // std::cout << "Distance: " << sqrt((mx1 - mx2) * (mx1 - mx2) + (my1 - my2) * (my1 - my2)) / 1000.0 << "km" << std::endl;
}

}

#endif