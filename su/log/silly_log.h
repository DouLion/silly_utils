#pragma once

#include <su_marco.h>
#include <spdlog/spdlog.h>
struct silly_log_opt
{
    std::string path

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

    enum class enum_log_level
    {
        ellALL = 0,
        ellDEBUG = 1,
        ellWARNING = 2,
        ellINFO = 3,
        ellERROR = 4,
        ellFATAL = 5
    };
    class opt
    {
    public:
        std::string path;
        enum_log_type type{2};
        enum_log_level level{0};

    };
  public:
    static bool init(const silly_log_opt& opt = {});

  public:
    static std::shared_ptr<spdlog::logger> m_spdlog_handler;
		
};

#define SFLOG_INFO(s, ...) silly_log::m_spdlog_handler->info(s , ##__VA_ARGS__);