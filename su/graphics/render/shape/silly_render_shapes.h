/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-25
 * @file: silly_render_shapes.h
 * @description: silly_render_shapes 类声明
 * @version: v1.0.1 2024-10-25 dou li yang
 */
#ifndef SILLY_RENDER_SHAPES_H
#define SILLY_RENDER_SHAPES_H
#include <geo/silly_geo.h>
namespace silly_render
{

class ellipse
{
  public:
    suPoint m_center;
    double m_radius_x;
    double m_radius_y;
};

class line : suLine
{
  public:
};
class rectangle : suRect
{
  public:
};

class polygon : suPoly
{
    std::vector<suPoint> m_points;
};
class multi_polygon : suMultiPoly
{
    std::vector<polygon> m_polygons;
};
}  // namespace silly_render

#endif  // SILLY_RENDER_SHAPES_H
