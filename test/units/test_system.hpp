/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/9 11:43
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_TEST_SYSTEM_HPP
#define SILLY_UTILS_TEST_SYSTEM_HPP

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>
#include "system/silly_uuid.hpp"

BOOST_AUTO_TEST_SUITE(TestSystem)

BOOST_AUTO_TEST_CASE(CREATE_UUID)      // BZ2Ñ¹ËõÎÄ¼þ
{
	std::cout << "\r\n\r\n****************" << "CREATE_UUID" << "****************" << std::endl;
	std::cout << silly_uuid::random_uuid() << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_SYSTEM_HPP
