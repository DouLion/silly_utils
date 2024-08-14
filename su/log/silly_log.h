#pragma once
#ifndef SILLY_UTILS_SILLY_LOG_H
#define SILLY_UTILS_SILLY_LOG_H
#include "singleton/silly_singleton.h"
#include <su_marco.h>
#include <spdlog/spdlog.h>

class silly_log : public silly_singleton<silly_log>
{
    friend class silly_singleton<silly_log>;

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
    class option
    {
      public:
        std::string path{"./logs"};  // 日志文件路径
        std::string name{"silly"};   // 日志文件名称
        enum_log_type type{2};       // 日志类型
        enum_log_level level{0};     // 日志级别
        size_t rotate_size{20};      // 日志文件大小(MB)
        bool console{true};          // 是否在控制台输出,服务部署时建议设置为false
    };

  public:
    /// <summary>
    /// 由主函数的入参初始化日志模块,存储路径在当前目录的 logs文件夹下
    /// 日志文件名称为程序名称
    /// </summary>
    /// <param name="argc"></param>
    /// <param name="argv"></param>
    /// <returns></returns>
    bool init(int argc, char** argv);

    /// <summary>
    /// 由配置文件构建option对象,再初始化日志模块
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    bool init(const std::string& path);

    /// <summary>
    /// 由option对象初始化日志模块
    /// </summary>
    /// <param name="opt"></param>
    /// <returns></returns>
    bool init(const option& opt);

#ifndef NDEBUG
    template <typename... Args>
    void debug(Args&&... s);
#endif

    template <typename... Args>
    void info(Args&&... s);

    template <typename... Args>
    void warn(Args&&... s);

    template <typename... Args>
    void error(Args&&... s);

  private:
    silly_log();

  private:
    void register_loguru(const option& opt);
    void register_spdlog(const option& opt);
    void register_glog(const option& opt);

  public:
#ifndef NDEBUG
    std::shared_ptr<spdlog::logger> m_spdlog_debug;
#endif
    std::shared_ptr<spdlog::logger> m_spdlog_info;
    std::shared_ptr<spdlog::logger> m_spdlog_warn;
    std::shared_ptr<spdlog::logger> m_spdlog_error;
};

#ifndef NDEBUG
template <typename... Args>
void silly_log::debug(Args&&... s)
{
    if (m_spdlog_debug)
    {
        m_spdlog_debug->debug(std::forward<Args>(s)...);
    }
}
#endif
template <typename... Args>
void silly_log::info(Args&&... s)
{
    if (m_spdlog_info)
    {
        m_spdlog_info->info(std::forward<Args>(s)...);
    }
}
template <typename... Args>
void silly_log::warn(Args&&... s)
{
    if (m_spdlog_warn)
    {
        m_spdlog_warn->warn(std::forward<Args>(s)...);
    }
}
template <typename... Args>
void silly_log::error(Args&&... s)
{
    if (m_spdlog_error)
    {
        m_spdlog_error->error(std::forward<Args>(s)...);
    }
}
/*#ifndef NDEBUG
#define SLOG_DEBUG(s, ...) \
    if (silly_log::instance().m_spdlog_debug)                          \
    {                                                                  \
        silly_log::instance().m_spdlog_debug->debug(s, ##__VA_ARGS__); \
    }
#else
#define SLOG_DEBUG(s, ...)
#endif

#define SLOG_INFO(s, ...)                                            \
    if (silly_log::instance().m_spdlog_info)                         \
    {                                                                \
        silly_log::instance().m_spdlog_info->info(s, ##__VA_ARGS__); \
    }
#define SLOG_WARN(s, ...)                                            \
    if (silly_log::instance().m_spdlog_warn)                         \
    {                                                                \
        silly_log::instance().m_spdlog_warn->warn(s, ##__VA_ARGS__); \
    }
#define SLOG_ERROR(s, ...)                                             \
    if (silly_log::instance().m_spdlog_error)                          \
    {                                                                  \
        silly_log::instance().m_spdlog_error->error(s, ##__VA_ARGS__); \
    }*/
#ifndef NDEBUG
#define SLOG_DEBUG(s, ...)  silly_log::instance().debug(silly_format::format("<{}:{}> ", SU_FILE_NAME, __LINE__).append(silly_format::format(s, ##__VA_ARGS__)));
#else
#define SLOG_DEBUG(s, ...)
#endif
#define SLOG_INFO(s, ...) silly_log::instance().info(silly_format::format("<{}:{}> ", SU_FILE_NAME, __LINE__).append(silly_format::format(s, ##__VA_ARGS__)));

#define SLOG_WARN(s, ...) silly_log::instance().warn(silly_format::format("<{}:{}> ", SU_FILE_NAME, __LINE__).append(silly_format::format(s, ##__VA_ARGS__)));

#define SLOG_ERROR(s, ...) silly_log::instance().error(silly_format::format("<{}:{}> ", SU_FILE_NAME, __LINE__).append(silly_format::format(s, ##__VA_ARGS__)));

#endif  // SILLY_UTILS_SILLY_LOG_H