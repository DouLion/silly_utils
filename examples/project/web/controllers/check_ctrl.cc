#include "check_ctrl.h"
#include <fstream>
#include <filesystem>
#include <chrono>
#include "tcp/silly_http.h"

#include "config_tool/config_tool.h"


#define BZ2_PATH    "bz2_path"
#define DIFF_TIME   "diff_time"
#define FILE_TEM    "file_name_template"
#define SUB_TIME    "subtract_time"


#define CHECK_STATUS 	"status"
#define CHECK_MESSAGE 	"message"

void check_ctrl::check(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string&& station, std::string&& work)
{

    SU_CREAT_JSON_DEFAULT_RESPONSE

    std::string dem_file = Config::instance().input_txt_dir;  
    std::cout << "input_txt_dir: " << station << std::endl;
    std::cout << "work: " << work << std::endl;
    std::cout << "dem_file: " << dem_file << std::endl;


    // 获取配置文件参数
    auto config = app().getCustomConfig();
    if (config.empty())
    {
        respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "fail";
        respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;
        SU_REQUEST_CALLBACK(Json::FastWriter().write(respJson))
    }
    // 读取配置文件参数
    std::string bz2_file = config[BZ2_PATH].asString();   // 文件地址
    std::string file_tem = config[FILE_TEM].asString();  // 文件名模板
    int diff_time = config[DIFF_TIME].asInt();      // 文件时间需要小时 数
    int subtract_time = config[SUB_TIME].asInt();   // 当前时间-文件时时间<?分钟


    respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "ok";
    respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 1;
    respJson[SU_HTTP_JSON_RESPONSE_DATA] = "hello word";
    SU_REQUEST_CALLBACK(Json::FastWriter().write(respJson))

    //respJson[SU_HTTP_JSON_RESPONSE_MESSAGE] = "fail";
    //respJson[SU_HTTP_JSON_RESPONSE_STATUS] = 0;
    //SU_REQUEST_CALLBACK(Json::FastWriter().write(respJson))

}
