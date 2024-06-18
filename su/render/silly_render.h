/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/6/17 20:33
 * @version: 1.0.1
 * @description: silly_render 类声明
 */
#ifndef SILLY_UTILS_SILLY_RENDER_H
#define SILLY_UTILS_SILLY_RENDER_H

#include <math/silly_matrix.h>
#include <image/png_utils.h>
#include <image/silly_color.h>


using namespace silly_math;
using namespace silly_image;


template <typename T>
class silly_val2color
{
    T val;
    silly_color color;
};

template <typename T>
class silly_render_param
{
    matrix_2d<T> mtx;
    std::vector<silly_val2color<T>> v2cs;
    png_data pd;

};


template <typename T>
class silly_render
{
    friend class matrix_2d<T>;
    friend class png_data;
  public:
    static void render_greater(silly_render_param<T>& rpp )
    {

    }

};

#endif  // SILLY_UTILS_SILLY_RENDER_H
