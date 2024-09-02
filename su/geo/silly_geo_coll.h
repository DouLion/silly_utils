/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 10:23
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_GEO_COLL_H
#define SILLY_UTILS_SILLY_GEO_COLL_H
#include <geo/silly_geo.h>
#include <geo/silly_geo_prop.h>

class silly_geo_coll
{
  public:
    silly_geo_coll() = default;

    silly_geo_coll(silly_point p);
    silly_geo_coll(silly_multi_point mp);
    silly_geo_coll(silly_line l);
    silly_geo_coll(silly_multi_silly_line ml);
    silly_geo_coll(silly_multi_poly mp);
    silly_geo_coll(silly_poly p);

    void add(silly_point p);
    void add(silly_multi_point mp);
    void add(silly_line l);
    void add(silly_multi_silly_line ml);
    void add(silly_multi_poly mp);
    void add(silly_poly p);

  public:
    // 类型
    enum_geometry_type m_type{enum_geometry_type::egtInvalid};
    // 内容
    silly_point m_point;                        // 单点
    silly_multi_point m_m_points;               // 多点
    silly_line m_line;                          // 单线
    silly_multi_silly_line m_m_lines;           // 多线
    silly_poly m_poly;                          // 单面(内环+外环)
    silly_multi_poly m_m_polys;                 // 多面(多个 单面)
    std::vector<enum_geometry_type> comp_type;  // 用于存储复合数据类型变量中的类型变量
    // 属性列表
    std::map<std::string, silly_geo_prop> m_props;
};

#endif  // SILLY_UTILS_SILLY_GEO_COLL_H
