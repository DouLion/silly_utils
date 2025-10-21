/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-14
 * @file: silly_dem.cpp
 * @description: silly_dem 头文件
 * @version: v1.0.1 2025-08-14 dou li yang
 */
#ifndef SILLY_DEM_H
#define SILLY_DEM_H
#include "math/silly_matrix.h"
#include <geo/silly_geo.h>
#include <geo/proj/silly_proj.h>
class suDem
{
  public:
    /// getter and setter
    void dx(const double& d);
    double dx() const;
    void dy(const double& d);
    double dy() const;
    void fill(const double& f);
    double fill() const;
    void l0(const double& l);
    double l0() const;
    void height(const size_t& h);
    size_t height() const;
    void width(const size_t& w);
    size_t width() const;
    void rect(const silly_rect& r);
    silly_rect rect() const;
    void data(const suDMatrix& d);
    suDMatrix& data();

    void gauss2lonlat(const suDem& rh, const double& cellsize, const double& l0);
    void lonlat2gauss(const suDem& rh, const double& cellsize, const double& l0);

    void cover(const suDem& rh);

    void release();

  protected:
    double m_dx = 0;
    double m_dy = 0;
    double m_fill = -9999.0;
    size_t m_width = 0;
    size_t m_height = 0;
    double m_l0 = 0;
    /*double m_scale = 1.0;
    double m_offset = 0.0;*/
    suDMatrix m_data;
    silly_rect m_rect;
};

inline void suDem::dx(const double& d)
{
    m_dx = d;
}
inline double suDem::dx() const
{
    return m_dx;
}
inline void suDem::dy(const double& d)
{
    m_dy = d;
}
inline double suDem::dy() const
{
    return m_dy;
}
inline void suDem::fill(const double& f)
{
    m_fill = f;
}
inline double suDem::fill() const
{
    return m_fill;
}
inline void suDem::l0(const double& l)
{
    m_l0 = l;
}
inline double suDem::l0() const
{
    return m_l0;
}
inline void suDem::height(const size_t& h)
{
    m_height = h;
}
inline size_t suDem::height() const
{
    return m_height;
}
inline void suDem::width(const size_t& w)
{
    m_width = w;
}
inline size_t suDem::width() const
{
    return m_width;
}
inline void suDem::rect(const silly_rect& r)
{
    m_rect = r;
}
inline silly_rect suDem::rect() const
{
    return m_rect;
}
inline void suDem::data(const suDMatrix& d)
{
    m_data = d;
    m_width = d.col();
    m_height = d.row();
}
inline suDMatrix& suDem::data()
{
    return m_data;
}

inline void suDem::release()
{
    m_data.release();
}


#endif //SILLY_DEM_H
