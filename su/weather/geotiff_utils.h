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
#include "geo_tiffp.h"

#define   STRIP_TIF 1
#define   TILE_TIF	2
#define   SCANLINE_TIF	3


class tif_data
{
public:

	uint32_t tif_width;
	uint32_t tif_height;

	uint16_t tif_bitsPerSample; // 每个数据占几位(8位一个byte)
	uint16_t tif_samplesPerPixel;  // 获取每个像素的样本数:通道数

	uint32_t tif_tileWidth;
	uint32_t tif_tileHeight;
	uint64_t tif_tileSize;

	uint32_t tif_type;
	uint64_t tif_lineSize;
	uint16_t tif_numChannels;

	uint16_t tif_orientation = 1;	// 图像的方向标签
	uint16_t tif_planarConfig = 1;	// 图像的平面配置标签
	uint16_t tif_photometric = 1;	// 图像的光度标签

	// SAMPLEFORMAT_UINT:1 uint 
	// SAMPLEFORMAT_INT:2 int 
	// SAMPLEFORMAT_IEEEFP float
	unsigned short tif_sampleFormat;  // 数据的类型(uint  int  float 无法确定)

	double tif_letf = 0.0;  // 左上角经纬
	double tif_top = 0.0;   //左上角纬度
	double pixelSizeX;  // 横向分辨率
	double pixelSizeY;  // 纵向分辨率

	// 以字节为将tif矩阵存入
	matrix_2d<float> tif_matrix2;
	//matrix_2d<unsigned char> tif_matrix2;

private:

};



class geotiff_utils
{
public:

	/// <summary>
	/// 读取tif到tif_data结构体
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="tif_matrix"></param>
	static tif_data readGeoTiff(std::string filePath);


	static bool get_tif_data(TIFF* tif, tif_data& res_tif);


	/// <summary>
	/// tif_data结构体写入tif文件
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="tif_matrix"></param>
	static bool writeGeoTiff(std::string filePath, tif_data tif_matrix2);

	static bool writeFourChannelTiff(std::string filePath, tif_data tif_matrix2);

};

#endif //SILLY_UTILS_GEOTIFF_UTILS_H
