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
#include "geo/silly_geo.h"
#include "math/silly_matrix.h"
using namespace silly_math;
#include <map>
#include <iostream>
 // 气象数据相关定义
#define DEFAULT_NOAA_DATE_TYPE											"APCP"		// 默认降雨类型 
#define NOAA_RH															"RH"		// 湿度
#define NOAA_TMAX														"TMAX"		// 最大温度
#define NOAA_TMIN														"TMIN"		// 最小温度
#define NOAA_PRES														"PRES"		// 气压
#define NOAA_UGRD														"UGRD"		// 东西风向	这两个矢量合并
#define NOAA_VGRD														"VGRD"		// 南北风向
#define NOAA_DSWRF														"DSWRF"		// 短波辐射
#define NOAA_URLWRF														"ULWRF"		// 长波辐射
#define NOAA_ALBDO														"ALBDO"		// 反照率
#define NOAA_APCP														"APCP"		// 降雨
#define NOAA_WIND														"FS"		// 风向，由UGRD，VGRD合并

#define NOAAMISSVALUE														1.0e36

// 默认下载的经纬范围, 中国除南沙群岛的大致范围
#define DEFAULT_LEFT														74
#define DEFAULT_RIGHT														135
#define DEFAULT_TOP															54
#define DEFAULT_BOTTOM														18
// 南沙群岛最南端的纬度
#define NANSHA_BOTTOM														3

#define PI (3.1415926535897932384626433832795028841971693993751f)

// Noaa数据中几个关键的时间点
#define FUTURE_HOUR_120 											120		// 5天
#define FUTURE_HOUR_123 											123	
#define FUTURE_HOUR_240 											240		// 10天
#define FUTURE_HOUR_252 											252		
#define FUTURE_HOUR_384 											384		// 16天


#define HOURS_IN_DAY													24			//  一天24小时
#define DEFAULT_DOWNLOAD_HOURS											384		// 默认预报天数 16天
#define OFFSET_HOURS													8			// 时差,北京时间与格林尼治时间
namespace grib_data {



	struct time_info {
	public:
		std::uint16_t		year;					// 发布的年
		std::uint16_t		month;				// 发布的月
		std::uint16_t		day;					// 发布的日
		std::uint16_t		hour;				// 发布的时
		std::uint16_t		miniute;			// 发布的时
		std::uint16_t		second;			// 发布的时
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
		std::string			name;	// 名称
		std::string			units;	// 单位
		std::string			short_name;	// 简称
		std::float_t		max;		// 最大值
		std::float_t		min;		// 最小值
	};


	//typedef std::map<std::uint16_t, cv::Mat	> data_set;
	//typedef std::map<std::string, data_set> data_cluster;	// Noaa数据簇

	class grib_utils
	{
	public:
		bool read(const std::string& grib_file, std::vector<DMatrix>& matrixs, int& type);

		time_info	m_pTimeInfo;
		geo_info	m_pGeoInfo;
		normal_info	m_pNormalInfo;
	};
}
#endif //SILLY_UTILS_GRIB_UTILS_H
