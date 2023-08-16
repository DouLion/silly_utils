/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/15 15:47
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_TEST_PYRAMID_HPP
#define SILLY_UTILS_TEST_PYRAMID_HPP

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>

//#include "tzx/pyramid/silly_pyramid_tools.h"
#include "image/png_utils.h"

BOOST_AUTO_TEST_SUITE(TestPyramid)

BOOST_AUTO_TEST_CASE(PYRAMID_WRITE)
{
	std::cout << "\r\n\r\n****************" << "PYRAMID_CREATE" << "****************" << std::endl;
	/*silly_pyramid sp;
	sp.open("E:/ccad", open_mode::APP_WRITE);
	sp.close();*/

	auto apdata = silly_image::png_utils::create_empty(100, 100);

	char* result = nullptr;
	size_t len = 0;
	silly_image::png_utils::encode_to_memory(apdata, &result, len);

	silly_image::png_pixel pp;
	int a = 0x12;
	int b = 0x34;
	int c = 0x56;
	int d = 0x78;
	pp.from_hex_argb("12345678");

	int ccc = 0;

};


BOOST_AUTO_TEST_SUITE_END()
#endif //SILLY_UTILS_TEST_PYRAMID_HPP
