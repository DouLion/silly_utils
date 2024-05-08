#pragma once

#include <su_marco.h>
#include <spdlog/spdlog.h>
struct silly_log_opt
{
    std::string path;
};



class silly_log
{
  public:
    enum class enum_log_type
    {
        eltLoguru = 1,
        eltSpdlog = 2,
        eltGlog = 3
    };
  public:
    static bool init(silly_log_opt opt = {}, enum_log_type type = enum_log_type::eltLoguru);

  public:
    static std::shared_ptr<spdlog::logger> m_spdlog_handler;
		
};

#define SFLOG_INFO(s, ...) silly_log::m_spdlog_handler->info(s , ##__VA_ARGS__);