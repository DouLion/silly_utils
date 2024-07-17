#pragma once
#include <drogon/drogon.h>
#include "project_marco.h"

int main(int argc, char** argv)
{
    // 日志默认只输出到 控制台
    SLOG_INFO("正在启动程序 {} ", argv[0]);
    // 将使用程序名作为输出日志文件名称
    silly_log::instance().init(argc, argv);

    const std::string web_cfg_name = "web_config.json";
    const std::string core_cfg_name = "core_config.json";
    const std::string core_config_file = std::filesystem::path(DEFAULT_ROOT_DIR).append("config").append(core_cfg_name).string();
    const std::string web_config_file = std::filesystem::path(DEFAULT_ROOT_DIR).append("config").append(web_cfg_name).string();

    // 初始化 程序配置项
    if(!true) {
            SLOG_ERROR("初始化 {} 配置项失败", core_config_file);
        return -1;
    }
    SLOG_INFO("初始化成功");
    // 初始化web配置项
    drogon::app().loadConfigFile(web_config_file);
    drogon::app().run();
    return 0;
}

