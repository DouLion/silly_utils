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

#ifndef SILLY_UTILS_NETCDF_UTILS_H
#define SILLY_UTILS_NETCDF_UTILS_H
#include <vector>
#include <iostream>
#include <map>
#include <opencv2/opencv.hpp>

struct nc_geo_rect {
	double g_left{180.};
	double g_top{-90.};
	double g_right{-180.};
	double g_bottom{90.};
};

struct nc_dim_info
{
	std::string name;
	std::map<std::string, std::string> attributes;
	int size;
};

struct nc_geo_info
{
	nc_geo_rect rect;
	double x_delta;
	double y_delta;
	size_t x_size;
	size_t y_size;
};

struct nc_info{
	std::vector<nc_dim_info> dims;
	nc_geo_info geo;
};

class netcdf_utils
{
public:
	static bool read_netcdf(const std::string &path, const std::string &group, std::map<int, cv::Mat> &data, nc_info & info);

	static bool write_netcdf(const std::string& path, const nc_info& info, const std::string& name, std::map<int, cv::Mat>& data);

	static bool write_netcdf(const std::string& path, const nc_info& info, const std::string& name, cv::Mat data);
};

#endif //SILLY_UTILS_NETCDF_UTILS_H
