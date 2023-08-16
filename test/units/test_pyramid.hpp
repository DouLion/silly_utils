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
#include "image/png_utils.h"
using namespace silly_image;

BOOST_AUTO_TEST_SUITE(TestPyramid)

BOOST_AUTO_TEST_CASE(PYRAMID_WRITE)
{
	std::cout << "\r\n\r\n****************" << "PYRAMID_CREATE" << "****************" << std::endl;

	silly_pyramid sp;
	sp.open("E:/ccad", open_mode::APP_WRITE);
	sp.close();
	size_t width = 100, height = 101;

	auto apdata = png_utils::create_empty(height, width);

	char* result = nullptr;
	png_pixel pp;
	int a = 0x12;
	int b = 0x34;
	int c = 0x56;
	int d = 0x78;
	pp.from_hex_argb("12FF0000");
	for (int r = 0; r < height; ++r)
	{
		for (int c = 0; c < width; ++c)
		{
			apdata.set_pixel(r, c, pp);
		}
		
	}
	
	size_t len = 0;
	silly_image::png_utils::encode_to_memory(apdata, &result, len);
	
	
	FileUtils::WriteAll("E:/ttpng.png", result, len);
	FileUtils::WriteAll("E:/ttpng.png", png_utils::encode_to_memory(apdata));
	apdata.release();
	free(result);
	/*silly_image::*/

	int ccc = 0;

};


BOOST_AUTO_TEST_SUITE_END()
#endif //SILLY_UTILS_TEST_PYRAMID_HPP
