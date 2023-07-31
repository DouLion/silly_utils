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
#define BOOST_TEST_MODULE TestDataBase
#include "image/jpeg_utils.h"
#include <boost/test/unit_test.hpp>
#include <filesystem>

// BOOST_AUTO_TEST_SUITE(TestDataBase)


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


	BOOST_AUTO_TEST_CASE(DM8_DPI_CONNECT)      // 达梦DPI链接数据库
	{
		std::cout << "\r\n\r\n****************" << "DM8_DPI_CONNECT" << "****************" << std::endl;

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

	}

	BOOST_AUTO_TEST_CASE(DM8_DPI_MODIFY)      // 达梦DPI 修改数据
	{
		std::cout << "\r\n\r\n****************" << "DM8_DPI_MODIFY" << "****************" << std::endl;

	}

// BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_DATABASE_HPP
