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

#define SU_HTTP_JSON_RESPONSE_DATA "data"
// 1 表示成功 0 表示失败
#define SU_HTTP_JSON_RESPONSE_STATUS "status"
#define SU_HTTP_JSON_RESPONSE_MESSAGE "message"

#define SU_CREATE_JSON_DEFAULT_RESPONSE                                      \
    auto resp = HttpResponse::newHttpResponse();                             \
    resp->setStatusCode(k200OK);                                             \
    resp->addHeader("access-control-allow-origin", "*");                     \
    resp->addHeader("access-control-allow-methods", "GET, POST, OPTIONS");   \
    resp->addHeader("access-control-allow-headers", "Origin, Content-Type"); \
    if(req->getMethod() == HttpMethod::Options) {                            \
        callback(resp); return;                                              \
    }                                                                        \
    resp->setContentTypeCode(CT_APPLICATION_JSON);                           \
    Json::Value respJson;                                                    \
    respJson[SU_HTTP_JSON_RESPONSE_DATA] = Json::objectValue;                \
    respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;                              \
    respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "未实现.";

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

namespace silly
{

namespace http
{
class post_param
{
  /// <summary>
  /// 处理前端post请求数据的基类
  /// </summary>
  public:
    /// <summary>
    /// 解析请求数据
    /// </summary>
    /// <param name="str"></param>
    /// <returns></returns>
    virtual bool parse(const std::string& str) = 0;

    /// <summary>
    /// 返回请求示例数据
    /// </summary>
    /// <returns></returns>
    virtual Json::Value example() = 0;

  public:
    int status = 0;
    std::string message = "未实现";
};

class post_result
{
  /// <summary>
  /// 处理返回结果的基类
  /// </summary>
  public:
    /// <summary>
    /// 将实际返回值转成json格式
    /// </summary>
    /// <returns></returns>
    virtual Json::Value jsonify() = 0;

    /// <summary>
    /// 返回示例结果
    /// </summary>
    /// <returns></returns>
    virtual Json::Value example() = 0;

  public:
    int status = 0;
    std::string message = "未实现";
};

}  // namespace http
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_HTTP_SERVER_H
