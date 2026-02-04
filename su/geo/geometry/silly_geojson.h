/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/25 11:08
 * @version: 1.0.1
 * @description:  geojson 读写, 参照https://geojson.cn/docs/ref/geojson
 */
#ifndef SILLY_GEO_JSON_H
#define SILLY_GEO_JSON_H
#include <geo/geometry/silly_geo_coll.h>
#include <json/silly_jsonpp.h>

class suGeoJson
{
  public:
    ////////////////读///////////////////

    /// <summary>
    /// 从文件加载所有矢量
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static std::vector<suGeoColl> Read(const suPath& file);

    /// <summary>
    /// 从字符串加载所有矢量
    /// </summary>
    /// <param name="geojson"></param>
    /// <returns></returns>
    static std::vector<suGeoColl> Parse(const std::string& geojson);

    static bool Parse(const std::string& geojson, suGeoColl& gc);

    static bool Parse(const Json::Value& geojson, suGeoColl& gc);


    static bool Parse(const Json::Value& geojson, suPoint& meta);
    static bool Parse(const Json::Value& geojson, suMultiPoint& meta);
    static bool Parse(const Json::Value& geojson, suLine& meta);
    static bool Parse(const Json::Value& geojson, suMultiLine& meta);
    static bool Parse(const Json::Value& geojson, suPoly& meta);
    static bool Parse(const Json::Value& geojson, suMultiPoly& meta);

    static bool ParseProperties(const Json::Value& prop, std::unordered_map<std::string, suGeoProp>& kv,std::map<uint16_t, std::string>& sort );

    static void CheckParse();

    /// <summary>
    /// 从Json::Value对象加载所有矢量
    /// </summary>
    /// <param name="jv"></param>
    /// <returns></returns>
    static std::vector<suGeoColl> Parse(const Json::Value& jv);

    static bool check(const Json::Value& jv, suPoint& point);
    static bool check(const Json::Value& jv, suMultiPoint& mpoint);
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
    static bool load(const Json::Value& jv, suMultiPoint& mpoint);

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
    static bool load(const Json::Value& jv, suGeoColl& gc);

    /// <summary>
    /// 从 Json::Value对象加载属性
    /// </summary>
    /// <param name="jv"></param>
    /// <param name="props"></param>
    /// <returns></returns>
    static bool load(const Json::Value& jv, std::unordered_map<std::string, suGeoProp>& props);

    ///////////////////////////////////写

    /// <summary>
    ///
    /// </summary>
    /// <param name="geo_colls"></param>
    /// <param name="precision"></param>
    /// <returns></returns>
    static std::string stringify(const std::vector<suGeoColl>& geo_colls, const int& precision = 8);
    static std::string stringify(const std::vector<suPoint>& points, const int& precision = 8);
    static std::string stringify(const std::vector<suLine>& lines, const int& precision = 8);
    static std::string stringify(const std::vector<suPoly>& polys, const int& precision = 8);
    static std::string stringify(const std::vector<suMultiPoint>& mpoints, const int& precision = 8);
    static std::string stringify(const std::vector<suMultiLine>& mlines, const int& precision = 8);
    static std::string stringify(const std::vector<suMultiPoly>& mpolys, const int& precision = 8);

    /// <summary>
    ///
    /// </summary>
    /// <param name="geo_coll"></param>
    /// <returns></returns>

    static Json::Value jsonify(const suGeoColl& geo_coll);
    static Json::Value jsonify(const suPoint& point);
    static Json::Value jsonify(const suLine& line);
    static Json::Value jsonify(const suPoly& poly);
    static Json::Value jsonify(const suMultiPoint& mpoint);
    static Json::Value jsonify(const suMultiLine& mline);
    static Json::Value jsonify(const suMultiPoly& mpoly);

    /// <summary>
    ///
    /// </summary>
    /// <param name="geo_coll"></param>
    /// <param name="precision"></param>
    /// <returns></returns>
    static Json::Value stringify(const suGeoColl& geo_coll, const int& precision = 8);
    static std::string stringify(const suPoint& point, const int& precision = 8);
    static std::string stringify(const suLine& line, const int& precision = 8);
    static std::string stringify(const suPoly& poly, const int& precision = 8);
    static std::string stringify(const suMultiPoint& mpoint, const int& precision = 8);
    static std::string stringify(const suMultiLine& mline, const int& precision = 8);
    static std::string stringify(const suMultiPoly& mpoly, const int& precision = 8);
};

#endif  // SILLY_GEO_JSON_H
