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

namespace silly
{

class jsonpp
{
  public:
    class opt
    {
      public:
        bool utf8{true};
        size_t precision{0};  // 小数精度, 为0则不处理
    };

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
    /// 将json内容解析到字符串中
    /// </summary>
    /// <param name="root"></param>
    /// <param name="opt">格式化方式</param>
    static std::string dumps(const Json::Value& root, const jsonpp::opt& _opt = {true, 0});

    static std::string to_string(const Json::Value root, const jsonpp::opt& _opt = {true, 0});

    static std::string stringify(const Json::Value root, const jsonpp::opt& _opt = {true, 0});

    static std::string string(const Json::Value root, const jsonpp::opt& _opt = {true, 0});

    /// <summary>
    /// 将json内容写入到文件
    /// </summary>
    /// <param name="root"></param>
    /// <param name="indentation">是否为紧凑型字符串,默认为紧凑型</param>
    static bool dumpf(const std::string& file, const Json::Value& root, const jsonpp::opt& _opt = {true, 0});
    static bool savef(const std::string& file, const Json::Value& root, const jsonpp::opt& _opt = {true, 0});

    static void find_by_key(const std::string& json, const std::string& key, const std::string& filter, std::vector<std::string>& arr);

    static void find_by_key(const Json::Value& root, const std::string& key, const std::string& filter, std::vector<std::string>& arr);

    /// <summary>
    /// 检查json中指定key的数据类型,如果正确,则赋值,并且返回true,否则返回false
    /// </summary>
    /// <param name="root"></param>
    /// <param name="key"></param>
    /// <param name="val"></param>
    /// <returns></returns>
    static bool check_str(const Json::Value& root, const std::string& key, std::string& val);
    static bool check_int32(const Json::Value& root, const std::string& key, int32_t& val);
    static bool check_uint32(const Json::Value& root, const std::string& key, uint32_t& val);
    static bool check_int64(const Json::Value& root, const std::string& key, int64_t& val);
    static bool check_uint64(const Json::Value& root, const std::string& key, uint64_t& val);
    static bool check_double(const Json::Value& root, const std::string& key, double& val);
    static bool check_bool(const Json::Value& root, const std::string& key, bool& val);
    static bool check_arr(const Json::Value& root, const std::string& key, Json::Value& jv_arr);
    static bool check_obj(const Json::Value& root, const std::string& key, Json::Value& jv_obj);

    /// <summary>
    /// 检查json中指定key的数据类型,如果正确,则赋值,否则抛出异常
    /// </summary>
    /// <param name="root"></param>
    /// <param name="key"></param>
    /// <param name="val"></param>
    /// <returns></returns>
    static void try_str(const Json::Value& root, const std::string& key, std::string& val);
    static void try_int32(const Json::Value& root, const std::string& key, int32_t& val);
    static void try_uint32(const Json::Value& root, const std::string& key, uint32_t& val);
    static void try_int64(const Json::Value& root, const std::string& key, int64_t& val);
    static void try_uint64(const Json::Value& root, const std::string& key, uint64_t& val);
    static void try_double(const Json::Value& root, const std::string& key, double& val);
    static void try_bool(const Json::Value& root, const std::string& key, bool& val);
    static void try_arr(const Json::Value& root, const std::string& key, Json::Value& jv_arr);
    static void try_obj(const Json::Value& root, const std::string& key, Json::Value& jv_obj);
};
}  // namespace silly

using silly_jsonpp = silly::jsonpp;

#endif  // SILLY_UTILS_SILLY_JSONPP_H
