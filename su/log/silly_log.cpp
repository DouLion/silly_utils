#pragma once
#include <log/silly_log.h>
#include <log/loguru/loguru.hpp>
#include <filesystem>


#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
std::shared_ptr<spdlog::logger> silly_log::m_spdlog_handler = nullptr;
const static char* SILLY_TZX_LOG_CHAR = " \n _________\n|  _   _  |               \n|_/ | | \\_|____   _   __  \n    | |   [_   ] [ \\ [  ] \n   _| |_   .' /_  > '  <  \n  |_____| [_____][__]`\\_]\n";
bool silly_log::init(const silly_log_opt& opt)
{
    bool status = false;
    char* log_file = "./logs/tzx.log";
    if (!opt.path.empty())
    {
        log_file = (char*)(opt.path.c_str());
    }
    std::filesystem::create_directories(std::filesystem::path(log_file).parent_path());
    char* argv[2];
    argv[0] = log_file;
    argv[1] = nullptr;
    int argc = 1;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;
    std::string sink_name = "multi_sink";
    std::initializer_list<spdlog::sink_ptr> inils;
    switch (type)
    {
        case enum_log_type::eltLoguru:
            loguru::init(argc, argv);
            loguru::add_file(argv[0], loguru::Append, loguru::Verbosity_MAX);
            status = true;
            break;
        case enum_log_type::eltSpdlog:
            console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::trace);
            console_sink->set_pattern("%^[%Y%m%d %H:%M:%S.%e] [%s:%#] %v");
            // 创建每日文件sink
            file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_file, 0, 0);
            file_sink->set_level(spdlog::level::trace);
            file_sink->set_pattern("[%Y%m%d %H:%M:%S.%e] [%s:%#] %v");
            inils = {console_sink, file_sink};
            // 创建日志器
            // spdlog::logger logger();
            m_spdlog_handler = std::make_shared<spdlog::logger>(sink_name, inils);
            // 设置日志级别
            m_spdlog_handler->set_level(spdlog::level::info);
            m_spdlog_handler->flush_on(spdlog::level::info);
            status = true;
            break;
        default:
            status = false;
           break;
    }
    return status;
}
