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

namespace silly
{
class geojson{
  public:
    
    static enum_geometry_type check_geojson_type(const std::string& content);
    static std::string stringify(const std::vector<silly_point> points);
    static std::string stringify(const std::vector<silly_line> lines);
    static std::string stringify(const std::vector<silly_poly> polys);
    // static std::vector<silly_poly> loads(const std::string& geojson);
    static std::vector<silly_geo_coll> loads(const std::string& geojson);
    static std::vector<silly_geo_coll> loadf(const std::string& file);
};
}

#endif  // SILLY_UTILS_SILLY_GEOJSON_H
