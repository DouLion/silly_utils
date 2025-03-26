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
class geojson
{
  public:
    /// <summary>
    /// 从字符串获取矢量类型
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    static enum_geometry_type type(const std::string& str);
    ///////////////////////////////////读

    /// <summary>
    /// 
    /// </summary>
    /// <param name="content"></param>
    /// <returns></returns>
    static enum_geometry_type check_geojson_type(const std::string& content);

    /// <summary>
    /// 从文件加载所有矢量
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static std::vector<silly_geo_coll> loadf(const std::string& file);

    /// <summary>
    /// 从字符串加载所有矢量
    /// </summary>
    /// <param name="geojson"></param>
    /// <returns></returns>
    static std::vector<silly_geo_coll> loads(const std::string& geojson);

    /// <summary>
    /// 从Json::Value对象加载所有矢量
    /// </summary>
    /// <param name="jv"></param>
    /// <returns></returns>
    static std::vector<silly_geo_coll> loadj(const Json::Value& jv);

    /// <summary>
    /// 从 Json::Value对象加载点
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="point"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, silly_point& point);

    /// <summary>
    /// 从 Json::Value对象加载多点
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="mpoint"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, silly_multi_point& mpoint);

    /// <summary>
    /// 从 Json::Value对象加载线
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="line"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, silly_line& line);

    /// <summary>
    /// 从 Json::Value对象加载多线
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="mline"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, silly_multi_silly_line& mline);

    /// <summary>
    /// 从 Json::Value对象加载环
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="ring"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, silly_ring& ring);

    /// <summary>
    /// 从 Json::Value对象加载面
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="poly"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, silly_poly& poly);

    /// <summary>
    /// 从 Json::Value对象加载多面
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="mpoly"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, silly_multi_poly& mpoly);

    /// <summary>
    /// 从 Json::Value对象加载矢量对象
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="gc"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, silly_geo_coll& gc);

    /// <summary>
    /// 从 Json::Value对象加载属性
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="props"></param>
    /// <returns></returns>
    static bool read(const Json::Value& jv, std::map<std::string, silly_geo_prop>& props);

    ///////////////////////////////////写

    static std::string stringify(const std::vector<silly_point> points);
    static std::string stringify(const std::vector<silly_line> lines);
    static std::string stringify(const std::vector<silly_poly> polys);
};
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_GEOJSON_H
