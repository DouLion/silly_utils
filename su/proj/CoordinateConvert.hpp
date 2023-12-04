/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/5/19 16:27
 * @version: 1.0.1
 * @software: TzxIsoLine
 * @description:
 */
#pragma once

#ifndef COORDINATE_CONVERT
#define COORDINATE_CONVERT

#include "su_marco.h"
#include "geo/TFF_Geo.h"

class CoordinateConvert
{
public:
	static void TransFormLonLatToScreen(const double& lon, const double& lat, double& sx, double& sy, const GeoRect& geo, const double& width, const double& height, const bool& bPrecise = true)
	{
		sx = ((lon - geo.g_left) * width) / geo.Width();

		if (bPrecise)
		{
			double mapY0 = std::log(std::tan(SU_PI / 4.0 + geo.g_bottom * SU_PI / 360.0));
			double mapY1 = std::log(std::tan(SU_PI / 4.0 + geo.g_top * SU_PI / 360.0));
			double mapH = std::abs(mapY1 - mapY0);
			sy = ((mapY0 - std::log(std::tan(SU_PI / 4.0 + lat * SU_PI / 360.0))) * height) / mapH;
		}
		else
		{
			sy = ((geo.g_bottom - lat) * height) / geo.Height();
		}
	}

	static void TransFormScreenToLonLat(const double& sx, const double& sy, double& lon, double& lat, const GeoRect& geo, const int& width, const int& height)
	{
		lon = geo.g_left + sx * geo.Width() / width;
		lat = geo.g_bottom - (sy * geo.Height()) / height;
	}

	static void TransFormLonLatToScreen(const double lon, const double lat, int& sx, int& sy, const GeoRect& geo, const int width, const int height, const bool bPrecise = true)
	{
		sx = int(((lon - geo.g_left) * (double)width) / geo.Width());

		if (bPrecise)
		{
			double mapy0 = std::log(std::tan(SU_PI / 4.0 + geo.g_bottom * SU_PI / 360.0));
			double mapy1 = std::log(std::tan(SU_PI / 4.0 + geo.g_top * SU_PI / 360.0));
			double maph = std::abs(mapy1 - mapy0);
			sy = int(((mapy0 - std::log(std::tan(SU_PI / 4.0 + lat * SU_PI / 360.0))) * height) / maph);
		}
		else
		{
			sy = int(((geo.g_bottom - lat) * (double)height) / geo.Height());
		}
	}
};

#endif //COORDINATE_CONVERT
