#include "example.h"

#include <network/tcp/silly_http.h>
#include "web/response_results.h"
#include "web/request_params.h"

using namespace project_name::api;

void example::get_data(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    SU_CREATE_JSON_DEFAULT_RESPONSE;

    get_lasest_data_param req_param;
    get_latest_data_resp_result resp_param;
    if (HttpMethod::Get == req->method())
    {
        respJson[SU_HTTP_JSON_RESPONSE_DATA][GET_REQUEST_PARAM_DEMO] = req_param.example();
        respJson[SU_HTTP_JSON_RESPONSE_DATA][GET_REQUEST_RESULT_DEMO] = resp_param.example();
        respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = GET_REQUEST_ERR;
    }
    else if (HttpMethod::Post == req->method())
    {
        if (!req_param.load(std::string(req->body())))  // 验证参数
        {
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = req_param.err;
        }
        else
        {
            // 调用处理函数,拼接返回结果
            respJson[SU_HTTP_JSON_RESPONSE_DATA] = resp_param.dump();
            respJson[SU_HTTP_JSON_RESPONSE_STATUS] = resp_param.status;
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = resp_param.err;
        }
    }
    else
    {
        SU_REQUEST_CALLBACK("")
    }

    SU_REQUEST_CALLBACK(silly_jsonpp::to_string(respJson));
}

void example::set_data_by_id(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    SU_CREATE_JSON_DEFAULT_RESPONSE;

    get_lasest_data_param req_param;
    get_latest_data_resp_result resp_param;
    if (HttpMethod::Get == req->method())
    {
        respJson[SU_HTTP_JSON_RESPONSE_DATA][GET_REQUEST_PARAM_DEMO] = req_param.example();
        respJson[SU_HTTP_JSON_RESPONSE_DATA][GET_REQUEST_RESULT_DEMO] = resp_param.example();
        respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = GET_REQUEST_ERR;
    }
    else if (HttpMethod::Post == req->method())
    {
        if (!req_param.load(std::string(req->body())))  // 验证参数
        {
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = req_param.err;
        }
        else
        {
            // 调用处理函数,拼接返回结果
            respJson[SU_HTTP_JSON_RESPONSE_DATA] = resp_param.dump();
            respJson[SU_HTTP_JSON_RESPONSE_STATUS] = resp_param.status;
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = resp_param.err;
        }
    }
    else
    {
        SU_REQUEST_CALLBACK("")
    }

    SU_REQUEST_CALLBACK(silly_jsonpp::to_string(respJson));
}