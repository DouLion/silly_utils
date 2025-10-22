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

    silly_geo_coll(const suPoint& p);
    silly_geo_coll(const silly_multi_point& mp);
    silly_geo_coll(const suLine& l);
    silly_geo_coll(const suMultiLine& ml);
    silly_geo_coll(const suPoly& p);
    silly_geo_coll(const suMultiPoly& mp);

    void add(const suPoint& p);
    void add(const silly_multi_point& mp);
    void add(const suLine& l);
    void add(const suMultiLine& ml);
    void add(const suPoly& p);
    void add(const suMultiPoly& mp);

  public:
    // 类型
    eGeometryType m_type{eGeometryType::Invalid};
    // 内容
    suPoint m_point;           // 单点
    silly_multi_point m_m_points;  // 多点
    suLine m_line;             // 单线
    suMultiLine m_m_lines;    // 多线
    suPoly m_poly;             // 单面(内环+外环)
    suMultiPoly m_m_polys;    // 多面(多个 单面)
    // 属性列表
    std::unordered_map<std::string, silly_geo_prop> m_props;
    std::map<uint16_t, std::string> m_prop_index;
};

#endif  // SILLY_UTILS_SILLY_GEO_COLL_H
