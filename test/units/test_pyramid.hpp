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
#include "files/TFF_FileUtils.h"
#include "tzx/pyramid/silly_pyramid_tools.h"

BOOST_AUTO_TEST_SUITE(TestPyramid)

BOOST_AUTO_TEST_CASE(PYRAMID_WRITE)
{
	std::cout << "\r\n\r\n****************" << "PYRAMID_CREATE" << "****************" << std::endl;
	std::filesystem::path pyramid_dir(DEFAULT_DATA_DIR);
	pyramid_dir.append("line_river_s5000");
	silly_pyramid sp;
	sp.open(pyramid_dir.string(), open_mode::READ);
	sp.close();

	/*silly_image::*/

	int ccc = 0;

};


BOOST_AUTO_TEST_SUITE_END()
#endif //SILLY_UTILS_TEST_PYRAMID_HPP
