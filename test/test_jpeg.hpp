/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/31 10:31
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_TEST_JPEG_HPP
#define SILLY_UTILS_TEST_JPEG_HPP

#define BOOST_TEST_INCLUDED
#include "image/jpeg_utils.h"
#include <boost/test/unit_test.hpp>
#include <filesystem>

BOOST_AUTO_TEST_SUITE(TestJPEG)


BOOST_AUTO_TEST_CASE(JPEG_READ)      // 读取JPEG
{
	std::cout << "\r\n\r\n****************" << "JPEG_READ" << "****************" << std::endl;

}


BOOST_AUTO_TEST_CASE(WRITE_JPEG)      // 写入 JPEG
{
	std::cout << "\r\n\r\n****************" << "WRITE_JPEG" << "****************" << std::endl;

}

BOOST_AUTO_TEST_SUITE_END()


#endif //SILLY_UTILS_TEST_JPEG_HPP
