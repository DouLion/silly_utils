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

	unsigned int tif_width;
	unsigned int tif_height;

	unsigned short tif_bitsPerSample; // 每个数据占几位(8位一个byte)

	unsigned short tif_samplesPerPixel;  // 获取每个像素的样本数

	// SAMPLEFORMAT_UINT:1 uint 
	// SAMPLEFORMAT_INT:2 int 
	// SAMPLEFORMAT_IEEEFP float
	unsigned short tif_sampleFormat;  // 数据的类型(uint  int  float 无法确定)

	double tif_letf = 0.0;
	double tif_top = 0.0;

	// 以字节为将tif矩阵存入
	//matrix_2d<float> tif_matrix2;
	matrix_2d<unsigned char> tif_matrix2;

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
	static tif_data readGeoTiff(std::string filePath);

	/// <summary>
	/// 写入二维矩阵到tif文件
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="tif_matrix"></param>
	static bool writeGeoTiff(std::string filePath, tif_data tif_matrix2);

};

#endif //SILLY_UTILS_GEOTIFF_UTILS_H
