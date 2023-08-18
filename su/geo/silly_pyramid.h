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
#include <cmath>
#include "geo/silly_geo.h"

#define SILLY_GLOBAL_LEFT  		-180.0
#define SILLY_GLOBAL_RIGHT		180.0
#define SILLY_GLOBAL_LNG_LEN 	360.0
#define SILLY_GLOBAL_TOP 		90.0
#define SILLY_GLOBAL_BOTTOM 	-90.0

/// <summary>
/// �����ͼ���Ͻ�Ϊԭ��, ���½�Ϊ (max, max)
/// </summary>
class google_pyramid_block
{
public:
	google_pyramid_block() = default;
	google_pyramid_block(size_t& l, size_t& r, size_t& c)
	{
		layer = l;
		col = c;
		row = r;
	}
	// ���,���к� ��0 ��ʼ
	size_t layer{ 0 };
	size_t col{ 0 };
	size_t row{ 0 };
	// TODO: �ò��ͼ��Ŀ��, �и����㹫ʽ
	size_t width;
	size_t height;
	// ��γ��Χ
	silly_geo_rect rect;
};

class google_pyramid_util
{
public:
	/// <summary>
	/// ��ȡָ����ľ�γ��Χ
	/// </summary>
	/// <param name="l"></param>
	/// <param name="c"></param>
	/// <param name="r"></param>
	/// <returns></returns>
	static google_pyramid_block get_block_rect(size_t l, size_t c, size_t r);
	

	/// <summary>
	/// �������ָ�������ڵĿ���Ϣ
	/// </summary>
	/// <param name="layer"></param>
	/// <param name="point"></param>
	/// <returns></returns>
	static google_pyramid_block get_point_block(const size_t layer, const silly_point& point);

};

#endif //SILLY_UTILS_SILLY_PYRAMID_H
