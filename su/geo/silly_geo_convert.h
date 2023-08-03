/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/3 11:28
 * @version: 1.0.1
 * @software: silly_utils
 * @description: GEO坐标点转换
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_CONVERT_H
#define SILLY_UTILS_SILLY_GEO_CONVERT_H

#include "silly_geo.h"
#include "silly_projection.h"
#include "math/silly_matrix.h"

class silly_geo_convert
{
	/// <summary>
	/// 将一个GEO组织的二维矩阵转为墨卡托组织的二维矩阵
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="src"></param>
	/// <param name="rect"></param>
	/// <param name="dst"></param>
	template <typename T>
	static bool matrix_geo_to_mercator(const silly_math::matrix_2d<T>& src, const silly_geo_rect& rect, silly_math::matrix_2d<T>& dst);

};

/////////////////////////implement/////////////////////////

template <typename T>
bool silly_geo_convert::matrix_geo_to_mercator(const silly_math::matrix_2d<T>& src, const silly_geo_rect& rect, silly_math::matrix_2d<T>& dst)
{
	if (!(src.row() && src.col() && src.get_data()))
	{
		return false;
	}
	// 防止传参数进来是src与dst是同一个对象
	silly_math::matrix_2d<T> tmp = src.copy();
	if (!dst.create(src.row(), src.col(), true))
	{
		return false;
	}
	double m_left{ 0 }, m_top{ 0 }, m_right{ 0 }, m_bottom{ 0 };
	double m_x = 0, m_y = 0;
	silly_projection::geo_to_mercator(rect.right, rect.bottom, m_right, m_bottom);
	silly_projection::geo_to_mercator(rect.left, rect.top, m_left, m_top);
	double m_width = m_right - m_left;
	double m_height = m_top - m_bottom;

	double g_width = rect.right - rect.left;
	double g_height = rect.top - rect.bottom;
	int max_r = tmp.row() - 1;
	int max_c = tmp.col() - 1;
	// 为dst即墨卡托上每个位置找到geo上对应的位置, 然后取值, 防止图片有撕裂的情况
	for (int i = 0; i < tmp.col(); ++i)
	{
		for (int j = 0; j < tmp.row(); ++j)
		{

			double m_x = i * m_width / width + m_left;	// 每个matrix网格点对应的mecator坐标
			double m_y = m_top - j * m_height / height;
			double lgtd, lttd;
			silly_projection::mercator_to_geo(m_x, m_y, lgtd, lttd);
			int dst_c = std::round((lgtd - rect.left) / g_width * width);
			int dst_r = std::round((rect.top - lttd) / g_height * height);
			// TODO: 这一步是不是有问题,是否是必须的,防止访问溢出
			dst_c = std::min(std::max(0, gI), max_c);
			dst_r = std::min(std::max(0, gJ), max_r);
			// std::cout << gI << "," << gJ << std::endl;
			dst.get_data()[j][i] = tmp.[dst_r][dst_c];
		}
	}
	tmp.destroy();
	return true;
}

#endif //SILLY_UTILS_SILLY_GEO_CONVERT_H
