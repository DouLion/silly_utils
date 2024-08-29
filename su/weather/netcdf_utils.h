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
#include <math/silly_matrix.h>
using namespace silly_math;
typedef unsigned char su_ncbyte;

struct nc_geo_rect
{
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

struct nc_info
{
    std::vector<nc_dim_info> dims;
    nc_geo_info geo;
};

class nc_mat
{
    /// <summary>
    /// 左上角 为 (0, 0)原点
    /// </summary>
  public:
    nc_mat() = default;
    nc_mat(const size_t& rows, const size_t& cols, const int& type, const size_t& dszie, const su_ncbyte* data);

    template <typename T>
    T at(const size_t& r, const size_t& c);

    nc_mat operator=(const nc_mat& other);

    /// <summary>
    ///  与
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="other"></param>
    /// <param name="fill"></param>
    /// <returns></returns>
    template <typename T>
    nc_mat nc_and(const nc_mat& other, const T& fill);

    /// <summary>
    ///  与
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="other"></param>
    /// <param name="fill"></param>
    /// <returns></returns>
    template <typename T>
    nc_mat nc_or(const nc_mat& other, const T& fill);

    template <typename T>
    matrix_2d<T> convert_matrix();

  private:
    int m_dims{2};
    int m_type{1};
    size_t m_tsize{1};
    size_t m_rows{0};
    size_t m_cols{0};
    su_ncbyte* m_data{nullptr};
    size_t m_dsize{0};
};

class netcdf_utils
{
  public:
    static bool read_netcdf(const std::string& path, const std::string& group, std::map<int, DMatrix>& data, nc_info& info);

    static bool write_netcdf(const std::string& path, const nc_info& info, const std::string& name, std::map<int, DMatrix>& data);

    static bool write_netcdf(const std::string& path, const nc_info& info, const std::string& name, DMatrix data);
};

#endif  // SILLY_UTILS_NETCDF_UTILS_H
