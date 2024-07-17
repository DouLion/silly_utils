#include "example.h"
#include <tcp/silly_http.h>
#include "web/response_results.h"
#include "web/request_params.h"


void get_data(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    SU_CREAT_JSON_DEFAULT_RESPONSE

    publish_time_resp_result resp_param;
    publish_time_req_param req_param;
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
            list_grid_publish_time_param param;
            param.days = req_param.days;
            param.root = forecast_web_config::instance().grid_root();
            grid_helper::list_grid_publish_time(param);

            resp_param.source_type_pub_tms = param.source_type_pub_tms;
            respJson[SU_HTTP_JSON_RESPONSE_DATA] = resp_param.dump();
            respJson[SU_HTTP_JSON_RESPONSE_STATUS] = resp_param.status;
            respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = resp_param.err;
        }
    }
    else
    {
        SU_REQUEST_CALLBACK("")
    }

    SU_REQUEST_CALLBACK(silly_jsonpp::to_string(respJson))
}

void set_data_by_id(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{

}