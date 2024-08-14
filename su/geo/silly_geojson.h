/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 11:08
 * @version: 1.0.1
 * @description:  geojson 操作,  TODO: 需要继续完善
 */
#ifndef SILLY_UTILS_SILLY_GEOJSON_H
#define SILLY_UTILS_SILLY_GEOJSON_H
#include <geo/silly_geo_coll.h>
#include <json/silly_jsonpp.h>
class silly_geojson
{
  public:
    static enum_geometry_type check_geojson_type(const std::string& content);
    static std::string dump_geojson(const std::vector<silly_point> points);
    static std::string dump_geojson(const std::vector<silly_line> lines);
    static std::string dump_geojson(const std::vector<silly_poly> polys);
    static std::vector<silly_poly> load_geojson(const std::string& geojson);
    /// <summary>
    /// 将silly_geo_coll回写为geojson中的表示方式
    /// </summary>
    /// <param name="sgc"></param>
    /// <returns></returns>
    static std::string dump_geo_coll(const silly_geo_coll& geo_coll);

    /// <summary>
    /// 从字符串加载silly_geo_coll
    /// </summary>
    /// <param name="content"></param>
    /// <returns></returns>
    static silly_geo_coll load_geo_coll(const std::string& content);
};

#endif  // SILLY_UTILS_SILLY_GEOJSON_H
