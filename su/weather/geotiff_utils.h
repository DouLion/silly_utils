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

	uint16_t tif_bitsPerSample; // ÿ������ռ��λ(8λһ��byte)
	uint16_t tif_samplesPerPixel;  // ��ȡÿ�����ص�������:ͨ����

	uint32_t tif_tileWidth;
	uint32_t tif_tileHeight;
	uint64_t tif_tileSize;

	uint32_t tif_type;
	uint64_t tif_lineSize;
	uint16_t tif_numChannels;

	uint16_t tif_orientation = 1;	// ͼ��ķ����ǩ
	uint16_t tif_planarConfig = 1;	// ͼ���ƽ�����ñ�ǩ
	uint16_t tif_photometric = 1;	// ͼ��Ĺ�ȱ�ǩ

	// SAMPLEFORMAT_UINT:1 uint 
	// SAMPLEFORMAT_INT:2 int 
	// SAMPLEFORMAT_IEEEFP float
	unsigned short tif_sampleFormat;  // ���ݵ�����(uint  int  float �޷�ȷ��)

	double tif_letf = 0.0;  // ���ϽǾ�γ
	double tif_top = 0.0;   //���Ͻ�γ��
	double pixelSizeX;  // ����ֱ���
	double pixelSizeY;  // ����ֱ���

	// ���ֽ�Ϊ��tif�������
	matrix_2d<float> tif_matrix2;
	//matrix_2d<unsigned char> tif_matrix2;

private:

};



class geotiff_utils
{
public:

	/// <summary>
	/// ��ȡtif��tif_data�ṹ��
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="tif_matrix"></param>
	static tif_data readGeoTiff(std::string filePath);


	static bool get_tif_data(TIFF* tif, tif_data& res_tif);


	/// <summary>
	/// tif_data�ṹ��д��tif�ļ�
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="tif_matrix"></param>
	static bool writeGeoTiff(std::string filePath, tif_data tif_matrix2);

	static bool writeFourChannelTiff(std::string filePath, tif_data tif_matrix2);

};

#endif //SILLY_UTILS_GEOTIFF_UTILS_H
