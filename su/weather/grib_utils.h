/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 15:51
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_GRIB_UTILS_H
#define SILLY_UTILS_GRIB_UTILS_H
#if GRIB_ENABLED
#include "geo/silly_geo.h"
#include "math/silly_matrix.h"
using namespace silly_math;
#include <map>
#include <iostream>
 // ����������ض���
#define DEFAULT_NOAA_DATE_TYPE											"APCP"		// Ĭ�Ͻ������� 
#define NOAA_RH															"RH"		// ʪ��
#define NOAA_TMAX														"TMAX"		// ����¶�
#define NOAA_TMIN														"TMIN"		// ��С�¶�
#define NOAA_PRES														"PRES"		// ��ѹ
#define NOAA_UGRD														"UGRD"		// ��������	������ʸ���ϲ�
#define NOAA_VGRD														"VGRD"		// �ϱ�����
#define NOAA_DSWRF														"DSWRF"		// �̲�����
#define NOAA_URLWRF														"ULWRF"		// ��������
#define NOAA_ALBDO														"ALBDO"		// ������
#define NOAA_APCP														"APCP"		// ����
#define NOAA_WIND														"FS"		// ������UGRD��VGRD�ϲ�

#define NOAAMISSVALUE														1.0e36

// Ĭ�����صľ�γ��Χ, �й�����ɳȺ���Ĵ��·�Χ
#define DEFAULT_LEFT														74
#define DEFAULT_RIGHT														135
#define DEFAULT_TOP															54
#define DEFAULT_BOTTOM														18
// ��ɳȺ�����϶˵�γ��
#define NANSHA_BOTTOM														3

#define PI (3.1415926535897932384626433832795028841971693993751f)

// Noaa�����м����ؼ���ʱ���
#define FUTURE_HOUR_120 											120		// 5��
#define FUTURE_HOUR_123 											123	
#define FUTURE_HOUR_240 											240		// 10��
#define FUTURE_HOUR_252 											252		
#define FUTURE_HOUR_384 											384		// 16��


#define HOURS_IN_DAY													24			//  һ��24Сʱ
#define DEFAULT_DOWNLOAD_HOURS											384		// Ĭ��Ԥ������ 16��
#define OFFSET_HOURS													8			// ʱ��,����ʱ�����������ʱ��
namespace grib_data {



	struct time_info {
	public:
		std::uint16_t		year;					// ��������
		std::uint16_t		month;				// ��������
		std::uint16_t		day;					// ��������
		std::uint16_t		hour;				// ������ʱ
		std::uint16_t		miniute;			// ������ʱ
		std::uint16_t		second;			// ������ʱ
	};

	struct geo_info {
	public:
		geo_info() = default;
		std::float_t			left;
		std::float_t			top;
		std::float_t			right;
		std::float_t			bottom;
		std::float_t			xstep;
		std::float_t			ystep;
		std::uint32_t			cols;
		std::uint32_t			rows;
		std::uint32_t			dnum;	// numberOfValues
	};

	struct normal_info {
	public:
		std::string			name;	// ����
		std::string			units;	// ��λ
		std::string			short_name;	// ���
		std::float_t		max;		// ���ֵ
		std::float_t		min;		// ��Сֵ
	};


	//typedef std::map<std::uint16_t, cv::Mat	> data_set;
	//typedef std::map<std::string, data_set> data_cluster;	// Noaa���ݴ�

	class grib_utils
	{
	public:
		bool read(const std::string& grib_file, DMatrix* matrix, int& type);

		time_info	m_pTimeInfo;
		geo_info	m_pGeoInfo;
		normal_info	m_pNormalInfo;
	};
}
#endif
#endif //SILLY_UTILS_GRIB_UTILS_H
