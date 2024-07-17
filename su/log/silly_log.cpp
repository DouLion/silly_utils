#pragma once
#include <log/silly_log.h>
#include <log/loguru/loguru.hpp>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

typedef spdlog::sinks::rotating_file_sink_mt su_rotate_log;

const static char* SILLY_TZX_LOG_CHAR = " \n _________\n|  _   _  |               \n|_/ | | \\_|____   _   __  \n    | |   [_   ] [ \\ [  ] \n   _| |_   .' /_  > '  <  \n  |_____| [_____][__]`\\_]\n";

silly_log::silly_log()
{
    init();
}

bool silly_log::init(const option& opt)
{
#if WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    bool status = false;

    register_spdlog(opt);
    status = true;
    // const char* log_file = "./logs/tzx.log";
    // if (!opt.path.empty())
    // {
    //     log_file = (char*)(opt.path.c_str());
    // }
    //
    // switch (opt.type)
    // {
    //     case enum_log_type::eltLoguru:
    //         // loguru::init(argc, argv);
    //         // loguru::add_file(argv[0], loguru::Append, loguru::Verbosity_MAX);
    //         // status = true;
    //         break;
    //     case enum_log_type::eltSpdlog:
    //         register_spdlog(opt);
    //         status = true;
    //         break;
    //     default:
    //         status = false;
    //         break;
    // }
    return status;
}

void silly_log::register_loguru(const option& opt)
{
    m_spdlog_debug = nullptr;
    m_spdlog_info = nullptr;
    m_spdlog_warn = nullptr;
    m_spdlog_error = nullptr;
}
void silly_log::register_spdlog(const option& opt)
{
    try
    {
        const std::filesystem::path sfp_log_root(opt.path);
        std::filesystem::create_directories(sfp_log_root);
        size_t rotate_mb = opt.rotate_size * 1024 * 1024;
        size_t max_files = 10;
        const std::string log_pattern = "%^[%m-%d %H:%M:%S.%e]: %v%$";
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        console_sink->set_pattern(log_pattern);
        auto debug_file_sink = std::make_shared<su_rotate_log>(std::filesystem::path(sfp_log_root).append(opt.name + ".debug.log").string(), rotate_mb, max_files);
        auto warn_file_sink = std::make_shared<su_rotate_log>(std::filesystem::path(sfp_log_root).append(opt.name + ".warn.log").string(), rotate_mb, max_files);
        auto info_file_sink = std::make_shared<su_rotate_log>(std::filesystem::path(sfp_log_root).append(opt.name + ".info.log").string(), rotate_mb, max_files);
        auto error_file_sink = std::make_shared<su_rotate_log>(std::filesystem::path(sfp_log_root).append(opt.name + ".error.log").string(), rotate_mb, max_files);


        m_spdlog_debug = std::make_shared<spdlog::logger>("debug\t", spdlog::sinks_init_list{console_sink, debug_file_sink});
        m_spdlog_debug->set_pattern(log_pattern);
        m_spdlog_info = std::make_shared<spdlog::logger>("info\t", spdlog::sinks_init_list{console_sink, info_file_sink});
        m_spdlog_info->set_pattern(log_pattern);
        m_spdlog_warn = std::make_shared<spdlog::logger>("warn\t", spdlog::sinks_init_list{console_sink,  warn_file_sink});
        m_spdlog_warn->set_pattern(log_pattern);
        m_spdlog_error = std::make_shared<spdlog::logger>("error\t", spdlog::sinks_init_list{console_sink, error_file_sink});
        m_spdlog_error->set_pattern(log_pattern);

    }
    catch (const spdlog::spdlog_ex& ex)
    {
        throw std::runtime_error("SPDLOG初始化失败");
    }
    catch (std::exception& e)
    {
        throw std::runtime_error("日志初始化失败");
    }
}
void silly_log::register_glog(const option& opt)
{
    m_spdlog_debug = nullptr;
    m_spdlog_info = nullptr;
    m_spdlog_warn = nullptr;
    m_spdlog_error = nullptr;
}

