/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:48
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PYRAMID_INFO_H
#define SILLY_UTILS_SILLY_PYRAMID_INFO_H

#include "tzx/pyramid/silly_pyramid_base.h"

#define TZX_IMAGE_DATA_INFO_NAME		"TzxImage.info"

 /** Datasrc info length */
#define DATASRC_INFO_LEN				512
/** Projection info length */
#define PRJ_INFO_LEN					1024
/** Bound info length */
#define BOUND_INFO_LEN					128
/** Tiling format info length */
#define TILING_FORMAT_INFO_LEN			12


class silly_pyramid_info : public silly_pyramid_base
{
public:
	silly_pyramid_info();

	bool read_info();

protected:
	/// Datasrc info.
	char									m_data_src[DATASRC_INFO_LEN];
	/// Projection info.
	char									m_prjection[PRJ_INFO_LEN];
	/// Bound info.
	char									m_bound[BOUND_INFO_LEN];
	/// Tiling format info.
	char									m_tiling_format[TILING_FORMAT_INFO_LEN];
};

#endif //SILLY_UTILS_SILLY_PYRAMID_INFO_H
