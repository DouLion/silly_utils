#include "example.h"

#include <network/tcp/silly_http.h>
#include "web/response_results.h"
#include "web/request_params.h"

using namespace project_name::api;

void example::get_data(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    SU_CREATE_JSON_DEFAULT_RESPONSE;

    fupload::web::param::upload_single param;
    fupload::web::result::upload_single result;
    if (req->method() == HttpMethod::Post)
    {
        if (param.parse(std::string(req->body())))
        {
            //result = 调用实际请求;
            respJson[SU_HTTP_JSON_RESPONSE_DATA] = result.jsonify();
            respJson[SU_HTTP_JSON_RESPONSE_STATUS] = result.status;
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = result.message;
        }
        else
        {
            respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = param.message;
        }
    }
    else if (req->method() == HttpMethod::Get)
    {
        respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;
        Json::Value data;
        data["请求示例"] = param.example();
        data["返回示例"] = result.example();
        respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "使用Post请求";
        respJson[SU_HTTP_JSON_RESPONSE_DATA] = data;
    }

    SU_REQUEST_CALLBACK(silly_jsonpp::to_string(respJson));
}

void example::set_data_by_id(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    SU_CREATE_JSON_DEFAULT_RESPONSE;

    fupload::web::param::upload_single param;
    fupload::web::result::upload_single result;
    if (req->method() == HttpMethod::Post)
    {
        if (param.parse(std::string(req->body())))
        {
            // result = 调用实际请求;
            respJson[SU_HTTP_JSON_RESPONSE_DATA] = result.jsonify();
            respJson[SU_HTTP_JSON_RESPONSE_STATUS] = result.status;
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = result.message;
        }
        else
        {
            respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = param.message;
        }
    }
    else if (req->method() == HttpMethod::Get)
    {
        respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;
        Json::Value data;
        data["请求示例"] = param.example();
        data["返回示例"] = result.example();
        respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "使用Post请求";
        respJson[SU_HTTP_JSON_RESPONSE_DATA] = data;
    }

    SU_REQUEST_CALLBACK(silly_jsonpp::to_string(respJson));
}