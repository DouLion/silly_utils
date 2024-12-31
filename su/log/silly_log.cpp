#pragma once
#include <log/silly_log.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

typedef spdlog::sinks::rotating_file_sink_mt su_rotate_log;

const static char* SILLY_TZX_LOG_CHAR =
    " \n _________\n|  _   _  |               \n|_/ | | \\_|____   _   __  \n    | |   [_   ] [ \\ [  ] \n   _| |_   .' /_  > '  <  \n  |_____| [_____][__]`\\_]\nBeijing TianZhiXiang Information Technology Co., Ltd.\n北京天智祥信息科技有限公司\n";

const static std::string SU_SINK_NAME_DEBUG = "debug";
const static std::string SU_SINK_NAME_INFO = "info";
const static std::string SU_SINK_NAME_WARN = "warn";
const static std::string SU_SINK_NAME_ERROR = "error";
const static std::string log_pattern = "%^[%Y-%m-%d %H:%M:%S.%e]: %v%$";

silly_log::silly_log()
{
    WINDOWS_UTF8_PAGE
    // init();
    try
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#ifndef NDEBUG
        m_spdlog_debug = std::make_shared<spdlog::logger>(SU_SINK_NAME_DEBUG, spdlog::sinks_init_list{console_sink});
        m_spdlog_debug->set_pattern(log_pattern);
#endif
        m_spdlog_info = std::make_shared<spdlog::logger>(SU_SINK_NAME_INFO, spdlog::sinks_init_list{console_sink});
        m_spdlog_info->set_pattern(log_pattern);
        m_spdlog_warn = std::make_shared<spdlog::logger>(SU_SINK_NAME_WARN, spdlog::sinks_init_list{console_sink});
        m_spdlog_warn->set_pattern(log_pattern);
        m_spdlog_error = std::make_shared<spdlog::logger>(SU_SINK_NAME_ERROR, spdlog::sinks_init_list{console_sink});
        m_spdlog_error->set_pattern(log_pattern);
#ifndef NDEBUG
        m_spdlog_debug->set_level(spdlog::level::debug);
        m_spdlog_info->set_level(spdlog::level::debug);
        m_spdlog_warn->set_level(spdlog::level::debug);
        m_spdlog_error->set_level(spdlog::level::debug);
#endif
        m_spdlog_info->info(SILLY_TZX_LOG_CHAR);
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

bool silly_log::init(const option& opt)
{
    bool status = true;

    register_spdlog(opt);
#ifndef NDEBUG
    status &= (m_spdlog_debug != nullptr);
#endif
    status &= (m_spdlog_info != nullptr);
    status &= (m_spdlog_warn != nullptr);
    status &= (m_spdlog_error != nullptr);

    return status;
}

bool silly_log::init(int argc, char** argv)
{
    if (!argc || !argv)
    {
        return false;
    }
    option opt;
    opt.name = std::filesystem::path(argv[0]).stem().string();
    return init(opt);
}
bool silly_log::init(const std::string& path)
{
    option opt;

    // TODO: 从配置文件加载
    return false;
}

void silly_log::register_loguru(const option& opt)
{
#ifndef NDEBUG
    m_spdlog_debug = nullptr;
#endif
    m_spdlog_info = nullptr;
    m_spdlog_warn = nullptr;
    m_spdlog_error = nullptr;
}
void silly_log::register_spdlog(const option& opt)
{
    try
    {
        {  // 清理
            std::shared_ptr<spdlog::logger> logger = spdlog::get(SU_SINK_NAME_DEBUG);
            if (logger)
            {
                logger->sinks().clear();
            }
            logger = spdlog::get(SU_SINK_NAME_INFO);
            if (logger)
            {
                logger->sinks().clear();
            }
            logger = spdlog::get(SU_SINK_NAME_WARN);

            if (logger)
            {
                logger->sinks().clear();
            }
            logger = spdlog::get(SU_SINK_NAME_ERROR);
            if (logger)
            {
                logger->sinks().clear();
            }
        }

        const std::filesystem::path sfp_log_root(opt.path);
        std::filesystem::create_directories(sfp_log_root);
        size_t rotate_mb = opt.rotate_size * 1024 * 1024;
        size_t max_files = 10;

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        console_sink->set_pattern(log_pattern);
        // auto debug_file_sink = std::make_shared<su_rotate_log>(std::filesystem::path(sfp_log_root).append(opt.name + ".debug.log").string(), rotate_mb, max_files);
        auto warn_file_sink = std::make_shared<su_rotate_log>(std::filesystem::path(sfp_log_root).append(opt.name + ".warn.log").string(), rotate_mb, max_files);
        auto info_file_sink = std::make_shared<su_rotate_log>(std::filesystem::path(sfp_log_root).append(opt.name + ".info.log").string(), rotate_mb, max_files);
        auto error_file_sink = std::make_shared<su_rotate_log>(std::filesystem::path(sfp_log_root).append(opt.name + ".error.log").string(), rotate_mb, max_files);

        // m_spdlog_debug = std::make_shared<spdlog::logger>(SU_SINK_NAME_DEBUG, spdlog::sinks_init_list{console_sink, debug_file_sink});
        // debug 信息暂时不考虑输出到文件
#ifndef NDEBUG
        m_spdlog_debug = std::make_shared<spdlog::logger>(SU_SINK_NAME_DEBUG, spdlog::sinks_init_list{console_sink});
        m_spdlog_debug->set_pattern(log_pattern);
#endif
        m_spdlog_info = std::make_shared<spdlog::logger>(SU_SINK_NAME_INFO, spdlog::sinks_init_list{console_sink, info_file_sink});
        m_spdlog_info->set_pattern(log_pattern);
        m_spdlog_warn = std::make_shared<spdlog::logger>(SU_SINK_NAME_WARN, spdlog::sinks_init_list{console_sink, warn_file_sink});
        m_spdlog_warn->set_pattern(log_pattern);
        m_spdlog_error = std::make_shared<spdlog::logger>(SU_SINK_NAME_ERROR, spdlog::sinks_init_list{console_sink, error_file_sink});
        m_spdlog_error->set_pattern(log_pattern);
#ifndef NDEBUG
        m_spdlog_debug->set_level(spdlog::level::debug);
        m_spdlog_info->set_level(spdlog::level::debug);
        m_spdlog_warn->set_level(spdlog::level::debug);
        m_spdlog_error->set_level(spdlog::level::debug);
#endif
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
#ifndef NDEBUG
    m_spdlog_debug = nullptr;
#endif
    m_spdlog_info = nullptr;
    m_spdlog_warn = nullptr;
    m_spdlog_error = nullptr;
}
