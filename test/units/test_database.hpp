/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/31 11:26
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_TEST_DATABASE_HPP
#define SILLY_UTILS_TEST_DATABASE_HPP

#define BOOST_TEST_INCLUDED

#include "database/dm8/dm8_dpi.h"
#include <boost/test/unit_test.hpp>
#include <filesystem>


BOOST_AUTO_TEST_SUITE(TestDataBase)


	BOOST_AUTO_TEST_CASE(OTL_CONNECT)      // OTL链接数据库
	{
		std::cout << "\r\n\r\n****************" << "OTL_CONNECT" << "****************" << std::endl;

	};


	BOOST_AUTO_TEST_CASE(OTL_INSERT)      // OTL 添加数据
	{
		std::cout << "\r\n\r\n****************" << "OTL_INSERT" << "****************" << std::endl;

	};

	BOOST_AUTO_TEST_CASE(OTL_DELETE)      // OTL 删除数据
	{
		std::cout << "\r\n\r\n****************" << "OTL_DELETE" << "****************" << std::endl;

	};

	BOOST_AUTO_TEST_CASE(OTL_QUERY)      // OTL 查询数据
	{
		std::cout << "\r\n\r\n****************" << "OTL_QUERY" << "****************" << std::endl;

	};

	BOOST_AUTO_TEST_CASE(OTL_MODIFY)      // OTL 修改数据
	{

		std::cout << "\r\n\r\n****************" << "OTL_MODIFY" << "****************" << std::endl;

	};

#if DM8_ENABLED

	BOOST_AUTO_TEST_CASE(DM8_DPI_CONNECT)      // 达梦DPI链接数据库
	{

		std::cout << "\r\n\r\n****************" << "DM8_DPI_CONNECT" << "****************" << std::endl;
		dm8_dpi dpi;
		BOOST_CHECK(dpi.login("192.168.0.201:5236", "TZX", "3edc9ijn~"));
		dpi.logout();
	}


	BOOST_AUTO_TEST_CASE(DM8_DPI_INSERT)      // 达梦DPI 添加数据
	{
		std::cout << "\r\n\r\n****************" << "DM8_DPI_INSERT" << "****************" << std::endl;

	}

	BOOST_AUTO_TEST_CASE(DM8_DPI_DELETE)      // 达梦DPI 删除数据
	{
		std::cout << "\r\n\r\n****************" << "DM8_DPI_DELETE" << "****************" << std::endl;

	}

	BOOST_AUTO_TEST_CASE(DM8_DPI_QUERY)      // 达梦DPI 查询数据
	{
		std::cout << "\r\n\r\n****************" << "DM8_DPI_QUERY" << "****************" << std::endl;

		dm8_dpi dpi;
		BOOST_CHECK(dpi.login("192.168.0.201:5236", "TZX", "3edc9ijn~"));
		sdint4 region_id = 0;
		sdbyte city_id[3] = { 0 };
		sdbyte city_name[40] = { 0 };
		slength city_id_ind = 0;
		slength city_name_ind = 0;
		slength region_id_ind = 0;
		ulength row_num;
		dpi_exec_direct(dpi.h_stmt, (sdbyte*)"select * from CITY");
		dpi_bind_col(dpi.h_stmt, 1, DSQL_C_NCHAR, &city_id, sizeof(city_id), &city_id_ind);
		dpi_bind_col(dpi.h_stmt, 2, DSQL_C_NCHAR, &city_name, sizeof(city_name), &city_name_ind);
		dpi_bind_col(dpi.h_stmt, 3, DSQL_C_SLONG, &region_id, sizeof(region_id), &region_id_ind);

		printf("dpi: select from table...\n");
		while (dpi_fetch(dpi.h_stmt, &row_num) != DSQL_NO_DATA)
		{
			printf("city_id = %s, city_name = %s , region_id=%d\n", city_id, city_name, region_id);
		}
		printf("dpi: select success\n");

		dpi.logout();
	}

	BOOST_AUTO_TEST_CASE(DM8_DPI_MODIFY)      // 达梦DPI 修改数据
	{
		std::cout << "\r\n\r\n****************" << "DM8_DPI_MODIFY" << "****************" << std::endl;

	}

#endif

BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_DATABASE_HPP
