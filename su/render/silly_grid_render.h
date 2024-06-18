/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/18 10:34
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_GRID_RENDER_H
#define SILLY_UTILS_SILLY_GRID_RENDER_H

// silly_geo_convert::matrix_geo_to_mercato

#include "image/png_utils.h"
#include <geo/silly_geo_convert.h>
using namespace silly_math;
using namespace silly_image;

template <typename T>
class silly_val2color
{
  public:
    silly_val2color() = default;
    silly_val2color(T v, silly_color c)
    {
        val = v;
        color = c;
    }

  public:
    T val;
    silly_color color;


};

template <typename T>
class silly_render_param
{
    matrix_2d<T> mtx;
    std::vector<silly_val2color<T>> v2cs; // 需要排好序
    png_data pd;
    silly_geo_rect rect;

    void sort()
    {

    }

};

template <typename T>
class silly_grid_render
{
    friend class matrix_2d<T>;
    friend class png_data;
  public:
     void normal_render_greater(silly_render_param<T>& srp)
    {
        int color_num = srp.v2cs.size();
        srp.pd = png_utils::create_empty(srp.mtx.row(), srp.mtx.col());
        // size_t max = mtx.row() * mtx.col();
        T*  ptr  = srp.mtx.data;
        for(size_t r = 0; r < srp.mtx.row(); ++r)
        {
            for(size_t c =0; c < srp.mtx.col(); ++c)
            {
                T v = ptr[0];ptr++;
                if(v< srp.v2cs[0].val)
                {
                    continue ;
                }
                int i  = 1;
                for(;i < color_num; ++i)
                {
                    if(v< srp.v2cs[i].val)
                    {
                        break;
                    }
                }
                i--;
                srp.pd.set_pixel(r, c, srp.v2cs[i].color);
            }

        }
    }

     void geo_mc_render_greater(silly_render_param<T>& srp)
    {

        matrix_2d<T> mtx;
        if(!silly_geo_convert::matrix_geo_to_mercator(srp.mtx, srp.rect, mtx))
        {
            // TODO: 错误提示
            return;
        }

        silly_render_param<T> p2 = srp;
        p2.mtx = mtx;

    }

};

#endif  // SILLY_UTILS_SILLY_GRID_RENDER_H
