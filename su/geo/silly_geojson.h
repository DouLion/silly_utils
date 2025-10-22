/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 11:08
 * @version: 1.0.1
 * @description:  geojson 读写, 参照https://geojson.cn/docs/ref/geojson
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
    ////////////////读///////////////////

    /// <summary>
    /// 从文件加载所有矢量
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static std::vector<silly_geo_coll> read(const std::filesystem::path& file);

    /// <summary>
    /// 从字符串加载所有矢量
    /// </summary>
    /// <param name="geojson"></param>
    /// <returns></returns>
    static std::vector<silly_geo_coll> load(const std::string& geojson);

    /// <summary>
    /// 从Json::Value对象加载所有矢量
    /// </summary>
    /// <param name="jv"></param>
    /// <returns></returns>
    static std::vector<silly_geo_coll> load(const Json::Value& jv);

    static bool check(const Json::Value& jv, suPoint& point);
    static bool check(const Json::Value& jv, silly_multi_point& mpoint);
    static bool check(const Json::Value& jv, suLine& line);
    static bool check(const Json::Value& jv, suMultiLine& mline);
    static bool check(const Json::Value& jv, suPoly& poly);
    static bool check(const Json::Value& jv, suMultiPoly& mpoly);

    /// <summary>
    /// 从 Json::Value对象加载点
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="point"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, suPoint& point);

    /// <summary>
    /// 从 Json::Value对象加载多点
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="mpoint"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, silly_multi_point& mpoint);

    /// <summary>
    /// 从 Json::Value对象加载线
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="line"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, suLine& line);

    /// <summary>
    /// 从 Json::Value对象加载多线
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="mline"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, suMultiLine& mline);

    /// <summary>
    /// 从 Json::Value对象加载环
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="ring"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, suRing& ring);

    /// <summary>
    /// 从 Json::Value对象加载面
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="poly"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, suPoly& poly);

    /// <summary>
    /// 从 Json::Value对象加载多面
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="mpoly"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, suMultiPoly& mpoly);

    /// <summary>
    /// 从 Json::Value对象加载矢量对象
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="gc"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, silly_geo_coll& gc);

    /// <summary>
    /// 从 Json::Value对象加载属性
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="props"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, std::unordered_map<std::string, silly_geo_prop>& props);

    ///////////////////////////////////写

    /// <summary>
    ///
    /// </summary>
    /// <param name="geo_colls"></param>
    /// <param name="precision"></param>
    /// <returns></returns>
    static std::string stringify(const std::vector<silly_geo_coll>& geo_colls, const int& precision = 8);
    static std::string stringify(const std::vector<suPoint>& points, const int& precision = 8);
    static std::string stringify(const std::vector<suLine>& lines, const int& precision = 8);
    static std::string stringify(const std::vector<suPoly>& polys, const int& precision = 8);
    static std::string stringify(const std::vector<silly_multi_point>& mpoints, const int& precision = 8);
    static std::string stringify(const std::vector<suMultiLine>& mlines, const int& precision = 8);
    static std::string stringify(const std::vector<suMultiPoly>& mpolys, const int& precision = 8);

    /// <summary>
    ///
    /// </summary>
    /// <param name="geo_coll"></param>
    /// <returns></returns>

    static Json::Value jsonify(const silly_geo_coll& geo_coll);
    static Json::Value jsonify(const suPoint& point);
    static Json::Value jsonify(const suLine& line);
    static Json::Value jsonify(const suPoly& poly);
    static Json::Value jsonify(const silly_multi_point& mpoint);
    static Json::Value jsonify(const suMultiLine& mline);
    static Json::Value jsonify(const suMultiPoly& mpoly);

    /// <summary>
    ///
    /// </summary>
    /// <param name="geo_coll"></param>
    /// <param name="precision"></param>
    /// <returns></returns>
    static Json::Value stringify(const silly_geo_coll& geo_coll, const int& precision = 8);
    static std::string stringify(const suPoint& point, const int& precision = 8);
    static std::string stringify(const suLine& line, const int& precision = 8);
    static std::string stringify(const suPoly& poly, const int& precision = 8);
    static std::string stringify(const silly_multi_point& mpoint, const int& precision = 8);
    static std::string stringify(const suMultiLine& mline, const int& precision = 8);
    static std::string stringify(const suMultiPoly& mpoly, const int& precision = 8);
};
}  // namespace silly

using sugeojson = silly::geojson;

#endif  // SILLY_UTILS_SILLY_GEOJSON_H
