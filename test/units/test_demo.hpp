/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 18:35
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_TEST_DEMO_HPP
#define SILLY_UTILS_TEST_DEMO_HPP

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestDemo)

BOOST_AUTO_TEST_CASE(DEMO)
{
	std::cout << "\r\n\r\n****************" << "DEMO" << "****************" << std::endl;
};


BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_DEMO_HPP
