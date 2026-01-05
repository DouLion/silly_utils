/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_http_server.h
 * @description: silly_http_server 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_HTTP_SERVER_H
#define SILLY_UTILS_SILLY_HTTP_SERVER_H
#include <json/silly_jsonpp.h>
#include <system/silly_system.h>

#define SU_HTTP_JSON_RESPONSE_DATA "data"
// 1 表示成功 0 表示失败
#define SU_HTTP_JSON_RESPONSE_STATUS "status"
#define SU_HTTP_JSON_RESPONSE_MESSAGE "message"

using suHttpKV = std::unordered_map<std::string, std::string>;

#define SU_CREATE_JSON_DEFAULT_RESPONSE                                      \
    auto resp = HttpResponse::newHttpResponse();                             \
    resp->setStatusCode(k200OK);                                             \
    resp->addHeader("access-control-allow-origin", "*");                     \
    resp->addHeader("access-control-allow-methods", "GET, POST, OPTIONS");   \
    resp->addHeader("access-control-allow-headers", "Origin, Content-Type"); \
    if (req->getMethod() == HttpMethod::Options)                             \
    {                                                                        \
        callback(resp);                                                      \
        return;                                                              \
    }                                                                        \
    resp->setContentTypeCode(CT_APPLICATION_JSON);                           \
    Json::Value respJson;                                                    \
    respJson[SU_HTTP_JSON_RESPONSE_DATA] = Json::objectValue;                \
    respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;                              \
    respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "未实现.";

#define SU_HTTP_CONVERT_KV(_dragon_req_)                         \
    [_dragon_req_]() -> suHttpKV {                               \
        suHttpKV ret;                                            \
        for (const auto& [k, v] : _dragon_req_->getParameters()) \
        {                                                        \
            std::string nk = k;                                  \
            ret[TO_LOWER(nk)] = v;                               \
        }                                                        \
        return ret;                                              \
    }()

#define SU_REQUEST_CALLBACK(data) \
    resp->setBody(data);          \
    callback(resp);               \
    return;

/// 以下宏定义只会在旧的服务中使用
#define SU_OLD_HTTP_JSON_RESPONSE_HEADER "header"
#define SU_OLD_HTTP_JSON_RESPONSE_BODY "body"
#define SU_OLD_HTTP_JSON_RESPONSE_RET_CODE "return_code"
#define SU_OLD_HTTP_JSON_RESPONSE_RET_MESSAGE "return_msg"

#define SU_OLD_CREATE_JSON_DEFAULT_RESPONSE(type)                                       \
    auto resp = HttpResponse::newHttpResponse();                                        \
    resp->setStatusCode(k200OK);                                                        \
    resp->setContentTypeCode(CT_APPLICATION_JSON);                                      \
    resp->addHeader("access-control-allow-origin", "*");                                \
    resp->addHeader("access-control-allow-methods", "GET, POST, OPTIONS");              \
    resp->addHeader("access-control-allow-headers", "Origin, Content-Type");            \
    Json::Value respJson;                                                               \
    respJson[SU_OLD_HTTP_JSON_RESPONSE_BODY] = Json::objectValue;                       \
    respJson[SU_OLD_HTTP_JSON_RESPONSE_HEADER] = Json::objectValue;                     \
    respJson[SU_OLD_HTTP_JSON_RESPONSE_HEADER][SU_OLD_HTTP_JSON_RESPONSE_RET_CODE] = 1; \
    respJson[SU_OLD_HTTP_JSON_RESPONSE_HEADER][SU_OLD_HTTP_JSON_RESPONSE_RET_MESSAGE] = "not implement.";

#define SU_HTTP_CHECK_STR suHttpUtils::CheckStr
#define SU_HTTP_TRY_STR suHttpUtils::TryStr
#define SU_HTTP_CHECK_NUM suHttpUtils::CheckNum
#define SU_HTTP_TRY_NUM suHttpUtils::TruNum

class suHttpUtils
{
  public:
    /**
     * 查找指定key,获取其对应的值,如果找不到以按照默认值返回
     *
     */
    static std::string CheckStr(const std::unordered_map<std::string, std::string>& k2v, const std::string& key, const std::string& dv = "");

    /**
     * 查找指定key,获取其对应的值,如果找不到,抛出异常
     *
     */
    static std::string TryStr(const std::unordered_map<std::string, std::string>& k2v, const std::string& key);

    static double CheckNum(const std::unordered_map<std::string, std::string>& k2v, const std::string& key, const double& dv = 0.0);

    static double TryNum(const std::unordered_map<std::string, std::string>& k2v, const std::string& key);
};

#endif  // SILLY_UTILS_SILLY_HTTP_SERVER_H


#ifdef DROGON_EXPORT_H
#ifndef RUN_DROGON_HTTP_SERVER
#define RUN_DROGON_HTTP_SERVER
#include <drogon/drogon.h>

inline void RunDrogonHttpServer(const std::string& cfg, const int& port, const int& thread_num = std::thread::hardware_concurrency())
{
    supath p(cfg);
    if (p.is_file())
    {
        // 使用 配置文件
        drogon::app().loadConfigFile(p.string());
    }
    else
    {
        drogon::app().addListener("0.0.0.0", port);
        drogon::app().setThreadNum(thread_num);
        std::string drLogCfg = R"({
      "log_path": "./logs",
      "log_format": "",
      "log_file": "access.log",
      "log_size_limit": 0,
      "use_local_time": true,
      "log_index": 0
    })";
        Json::Value jvDrLogCfg;
        if (Json::Reader().parse(drLogCfg, jvDrLogCfg))
        {
            drogon::app().addPlugin("drogon::plugin::AccessLogger", {}, jvDrLogCfg);
        }
    }
#ifndef _WIN32
    supath dlock("/tmp/drogon.lock");
    if (dlock.exists())
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        supath::rmfile(dlock);
    }
#endif
    drogon::app().run();
}
#endif
#endif