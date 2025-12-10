/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-04
 * @file: silly_geo_cairo.cpp
 * @description: silly_geo_cairo 头文件
 * @version: v1.0.1 2025-12-04 dou li yang
 */
#ifndef SILLY_GEO_CAIRO_H
#define SILLY_GEO_CAIRO_H
#include <graphics/render/canvas/silly_cairo.h>
#include <geo/silly_geo.h>
#include <geo/proj/silly_proj.h>

enum eGeoCairoProj
{
    LonLat = 1,
    Gauss = 2,
    Mercator = 3
};
class suGeoCairo : public suCairo
{
public:
    suGeoCairo() =default;
    explicit suGeoCairo(const eGeoCairoProj& proj) : m_Proj(proj){} ;

protected:
    eGeoCairoProj m_Proj = LonLat;
    suRect m_Box;


};

#endif  // SILLY_GEO_CAIRO_H
