/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/26 14:06
 * @version: 1.0.1
 * @description: jsoncpp操作
 */
#ifndef SILLY_JSONPP_H
#define SILLY_JSONPP_H
#include <files/silly_file.h>
#include <json/json.h>

class suJsonPP
{
  public:
    class style
    {
      public:
        style() = default;
        style(const bool& u8, const uint8_t& nn) : utf8(u8), precision(nn)
        {
        }
        style(const uint8_t& nn) :  precision(nn)
        {
        }
        ~style() = default;

      public:
        bool utf8 = true;
        uint8_t precision = 8;                  // 小数精度, 为0则不处理
        std::string indentation = "  ";        // 无缩进（紧凑模式）
        std::string commentStyle = "None";     // 不保留注释
        bool enableYAMLCompatibility = false;  // 禁用YAML兼容
    };

  public:
    /// <summary>
    /// 从文件加载json内容
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static Json::Value read(const suPath& file);

    /// <summary>
    /// 从字符串加载json内容
    /// </summary>
    /// <param name="content"></param>
    /// <returns></returns>
    static Json::Value loads(const std::string& content);
    static Json::Value parse(const std::string& content);

    /// <summary>
    /// 将json内容解析到字符串中
    /// </summary>
    /// <param name="root"></param>
    /// <param name="opt">格式化方式</param>
    static std::string dumps(const Json::Value& root, const suJsonPP::style& opt ={8});
    static std::string to_string(const Json::Value& root, const suJsonPP::style& opt ={8});
    static std::string stringify(const Json::Value& root, const suJsonPP::style& opt ={8});

    /// <summary>
    /// 将json内容写入到文件
    /// </summary>
    /// <param name="file">文件路径</param>
    /// <param name="root"></param>
    /// <param name="opt">样式</param>
    static bool write(const suPath& file, const Json::Value& root, const suJsonPP::style& opt ={8});

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

using silly_jsonpp = suJsonPP;
using sujson = suJsonPP;

#endif  // SILLY_JSONPP_H
