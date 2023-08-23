/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 15:52
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_GEOTIFF_UTILS_H
#define SILLY_UTILS_GEOTIFF_UTILS_H

#include <iostream>
#include <string>
#include <vector>


#include "math/silly_matrix.h"
using namespace silly_math;

#include <geotiffio.h>
#include <xtiffio.h>

class tif_data
{
public:





	tif_data() = default;
	~tif_data() = default;

private:

};


class geotiff_utils
{
public:

	/// <summary>
	/// 读取tif到二维矩阵
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="tif_matrix"></param>
	static void readGeoTiff(std::string filePath, matrix_2d<unsigned char>& tif_matrix);

	/// <summary>
	/// 写入二维矩阵到tif文件
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="tif_matrix"></param>
	static void writeGeoTiff(std::string filePath, matrix_2d<unsigned char>& tif_matrix);

};

#endif //SILLY_UTILS_GEOTIFF_UTILS_H
