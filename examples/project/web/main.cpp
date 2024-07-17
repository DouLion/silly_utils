

#include <drogon/drogon.h>
#include "project_marco.h"

int main(int argc, char** argv)
{
    // 日志默认只输出到 控制台
    SLOG_INFO("正在启动程序 {} ", argv[0]);
    // 将使用程序名作为输出日志文件名称
    silly_log::instance().init(argc, argv);

    std::string web_cfg_name = "web_config.json";
    std::string core_cfg_name = "core_config.json";
#ifndef NDEBUG
    std::filesystem::path web_config(DEFAULT_CONFIG_DIR);
    std::filesystem::path core_config(DEFAULT_CONFIG_DIR);
#else
    std::filesystem::path web_config("./config");
    std::filesystem::path core_config("./config");
#endif
    web_config.append(web_cfg_name);
    core_config.append(core_cfg_name);

    // 初始化 程序配置项

    // 初始化web配置项
    drogon::app().loadConfigFile(web_config.string());
    drogon::app().run();
    return 0;
}

