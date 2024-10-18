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
#include <su_marco.h>

#define SU_HTTP_JSON_RESPONSE_DATA "data"
#define SU_HTTP_JSON_RESPONSE_STATUS "status"
#define SU_HTTP_JSON_RESPONSE_MESSAGE "message"

#define SU_OLD_HTTP_JSON_RESPONSE_HEADER "header"
#define SU_OLD_HTTP_JSON_RESPONSE_BODY "body"
#define SU_OLD_HTTP_JSON_RESPONSE_RET_CODE "return_code"
#define SU_OLD_HTTP_JSON_RESPONSE_RET_MESSAGE "return_msg"

#define SU_CREATE_JSON_DEFAULT_RESPONSE                                       \
    auto resp = HttpResponse::newHttpResponse();                             \
    resp->setStatusCode(k200OK);                                             \
    resp->setContentTypeCode(CT_APPLICATION_JSON);                           \
    resp->addHeader("access-control-allow-origin", "*");                     \
    resp->addHeader("access-control-allow-methods", "GET, POST, OPTIONS");   \
    resp->addHeader("access-control-allow-headers", "Origin, Content-Type"); \
    Json::Value respJson;                                                    \
    respJson[SU_HTTP_JSON_RESPONSE_DATA] = Json::objectValue;                \
    respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;                              \
    respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "not implement.";

#define SU_OLD_CREATE_JSON_DEFAULT_RESPONSE(type)                                        \
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

#define SU_REQUEST_CALLBACK(data) \
    resp->setBody(data);          \
    callback(resp);               \
    return;

class silly_http_server
{
  public:
    silly_http_server();
    ~silly_http_server();

  private:
};

#endif  // SILLY_UTILS_SILLY_HTTP_SERVER_H
