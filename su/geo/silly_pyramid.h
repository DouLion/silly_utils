/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/11 18:25
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PYRAMID_H
#define SILLY_UTILS_SILLY_PYRAMID_H

#include "geo/silly_geo.h"

#define GLOBAL_LEFT  	-180.0
#define GLOBAL_RIGHT	 180.0
#define GLOBAL_LNG_LEN 	360.0
#define GLOBAL_TOP 		90.0
#define GLOBAL_BOTTOM 	-90.0

class google_pyramid
{
public:

	size_t layer {0};
	size_t col{0};
	size_t row{0};

	static silly_geo_rect get_block_rect(size_t l, size_t c, size_t r)
	{
		silly_geo_rect ret_rect;
		int ceil_num = pow(2, l);
		// ret_rect.left = ()
	}



};

#endif //SILLY_UTILS_SILLY_PYRAMID_H
