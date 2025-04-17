/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/3 11:28
 * @version: 1.0.1
 * @software: silly_utils
 * @description: GEO坐标点转换
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_GEO_CONVERT_H
#define SILLY_UTILS_SILLY_GEO_CONVERT_H

#include <geo/silly_geo.h>
#include <geo/proj/silly_proj.h>
#include <math/silly_matrix.h>

class silly_geo_convert
{
  public:
    /// <summary>
    /// shp文件转geojson
    /// </summary>
    /// <param name="shpFile">读取SHP文件地址</param>
    /// <param name="geojsonFile">生成geojson文件地址</param>
    /// <returns></returns>
    static bool shp_to_geojson(const char* shpFile, const char* geojsonFile);

    /// <summary>
    /// geojson文件转shp
    /// </summary>
    /// <param name="geojsonFile">读取geojson文件地址</param>
    /// <param name="shpFile">生成SHP文件地址</param>
    /// <returns></returns>
    static bool geojson_to_shp(const char* geojsonFile, const char* shpFile);

    /// <summary>
    /// 将一个GEO组织的二维矩阵转为墨卡托组织的二维矩阵
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="src"></param>
    /// <param name="rect"></param>
    /// <param name="dst"></param>
    template <typename T>
    static bool matrix_geo_to_mercator(silly_math::matrix_2d<T> src, const silly_geo_rect& rect, silly_math::matrix_2d<T>& dst);

};

/////////////////////////implement/////////////////////////

template <typename T>
bool silly_geo_convert::matrix_geo_to_mercator(silly_math::matrix_2d<T> src, const silly_geo_rect& rect, silly_math::matrix_2d<T>& dst)
{
    // 防止传参数进来是src与dst是同一个对象
    silly_math::matrix_2d<T> tmp = src.copy();
    if (!(tmp.row() && tmp.col() && tmp.seek_row(0)))
    {
        return false;
    }

    if (!dst.create(tmp.row(), tmp.col(), true))
    {
        return false;
    }
    double m_left{0}, m_top{0}, m_right{0}, m_bottom{0};
    silly::geo::proj::convert::Tlonlat_to_mercator(rect.max.x, rect.min.y, m_right, m_bottom);
    silly::geo::proj::convert::Tlonlat_to_mercator(rect.min.x, rect.max.y, m_left, m_top);
    double mc_xdelta = (m_right - m_left) / tmp.col();
    double mc_ydelta = (m_top - m_bottom) / tmp.row();

    double geo_xdelta = (rect.max.x - rect.min.x) / tmp.col();
    double geo_ydelta = (rect.max.y - rect.min.y) / tmp.row();

    /*   T g_width = rect.max.x - rect.min.x;
       T g_height = rect.max.y - rect.min.y;*/
    int max_r = tmp.row() - 1;
    int max_c = tmp.col() - 1;
    // 为dst即墨卡托上每个位置找到geo上对应的位置, 然后取值, 防止图片有撕裂的情况
    for (int r = 0; r < tmp.row(); ++r)
    {
        for (int c = 0; c < tmp.col(); ++c)
        {
            double m_x = c * mc_xdelta + m_left;  // 每个matrix网格点对应的mecator坐标
            double m_y = m_top - r * mc_ydelta;
            double lgtd, lttd;
            silly_projection::mercator_to_geo(m_x, m_y, lgtd, lttd);
            int dst_c = std::round((lgtd - rect.min.x) / geo_xdelta);
            int dst_r = std::round((rect.max.y - lttd) / geo_ydelta);
            // TODO: 这一步是不是有问题,是否是必须的,防止访问溢出
            dst_c = std::min(std::max(0, dst_c), max_c);
            dst_r = std::min(std::max(0, dst_r), max_r);
            dst[r][c] = tmp[dst_r][dst_c];
        }
    }
    tmp.release();
    return true;
}

#endif  // SILLY_UTILS_SILLY_GEO_CONVERT_H
