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

BOOST_AUTO_TEST_SUITE(TestGeo)

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
