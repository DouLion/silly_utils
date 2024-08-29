/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 15:53
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_HTTP_H
#define SILLY_UTILS_SILLY_HTTP_H
#include <iostream>
#include <map>
#include <vector>

#define SU_HTTP_JSON_RESPONSE_DATA "data"
#define SU_HTTP_JSON_RESPONSE_STATUS "status"
#define SU_HTTP_JSON_RESPONSE_MESSAGE "message"

#define SU_OLD_HTTP_JSON_RESPONSE_HEADER "header"
#define SU_OLD_HTTP_JSON_RESPONSE_BODY "body"
#define SU_OLD_HTTP_JSON_RESPONSE_RET_CODE "return_code"
#define SU_OLD_HTTP_JSON_RESPONSE_RET_MESSAGE "return_msg"

#define SU_CREAT_JSON_DEFAULT_RESPONSE                                       \
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

#define SU_OLD_CREAT_JSON_DEFAULT_RESPONSE(type)                                        \
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

class silly_http
{
  public:
    /// <summary>
    /// 简单的get请求
    /// </summary>
    /// <param name="url"></param>
    /// <param name="headers"></param>
    /// <returns></returns>
    static std::string request_get(const std::string& url, const std::map<std::string, std::string>& headers);

    /// <summary>
    /// 简单的post请求
    /// </summary>
    /// <param name="url"></param>
    /// <param name="body"></param>
    /// <param name="headers"></param>
    /// <returns></returns>
    static std::string request_post(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers);

    /// <summary>
    /// 下载文件
    /// </summary>
    /// <param name="url"></param>
    /// <param name="save">本地保存的路径</param>
    /// <param name="headers"></param>
    /// <returns></returns>
    static bool request_download(const std::string& url, const std::string& save, const std::map<std::string, std::string>& headers);

    /// <summary>
    ///
    /// </summary>
    /// <param name="url"></param>
    /// <param name="body">附加的请求信息</param>
    /// <param name="files">本地需要上传的文件</param>
    /// <param name="headers"></param>
    /// <param name="resp"></param>
    /// <returns></returns>
    static bool request_upload(const std::string& url, const std::string& body, const std::vector<std::string> files, const std::map<std::string, std::string>& headers, std::string& resp);
};

#endif  // SILLY_UTILS_SILLY_HTTP_H
