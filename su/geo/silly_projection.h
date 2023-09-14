/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/3 11:27
 * @version: 1.0.1
 * @software: silly_utils
 * @description: GEO坐标点投影方法
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PROJECTION_H
#define SILLY_UTILS_SILLY_PROJECTION_H

#include "marco.h"
#include <cmath>

class silly_projection
{
public:

	/// <summary>
	/// 经纬度坐标转墨卡托
	/// </summary>
	/// <param name="lgtd"></param>
	/// <param name="lttd"></param>
	/// <param name="x"></param>
	/// <param name="y"></param>
	static void geo_to_mercator(const double& lgtd, const double& lttd, double& x, double& y)
	{
		x = lgtd * 20037508.34 / 180;
		y = std::log(std::tan((90 + lttd) * SU_PI / 360)) / (SU_PI / 180);
		y = y * 20037508.34 / 180;
	}

	/// <summary>
	/// 墨卡托坐标转经纬度
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="lgtd"></param>
	/// <param name="lttd"></param>
	static void mercator_to_geo(const double& x, const double& y, double& lgtd, double& lttd)
	{
		lgtd = x / 20037508.34 * 180;
		lttd = y / 20037508.34 * 180;
		lttd = 180 / SU_PI * (2 * std::atan(std::exp(lttd * SU_PI / 180)) - SU_PI / 2);

	}
};

#endif //SILLY_UTILS_SILLY_PROJECTION_H
