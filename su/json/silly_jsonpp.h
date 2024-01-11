/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/26 14:06
 * @version: 1.0.1
 * @description: jsoncpp操作
 */
#ifndef SILLY_UTILS_SILLY_JSONPP_H
#define SILLY_UTILS_SILLY_JSONPP_H
#include <su_marco.h>
#include <json/json.h>

class silly_jsonpp
{
  public:
    /// <summary>
    /// 从文件加载json内容
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static Json::Value loadf(const std::string& file);

    /// <summary>
    /// 从字符串加载json内容
    /// </summary>
    /// <param name="content"></param>
    /// <returns></returns>
    static Json::Value loads(const std::string& content);

    /// <summary>
    /// jsonpp转字符串,处理了中文变为unicode编码的问题
    /// </summary>
    /// <param name="root"></param>
    /// <returns></returns>
    static std::string to_string(const Json::Value root);

    static void find_by_key(const std::string& json, const std::string& key, const std::string& filter, std::vector<std::string>& arr);

    static void find_by_key(const Json::Value& root, const std::string& key, const std::string& filter, std::vector<std::string>& arr);
};

#endif  // SILLY_UTILS_SILLY_JSONPP_H
