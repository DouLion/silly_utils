#pragma once
#pragma execution_character_set("utf-8")
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
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
        std::string path{"./logs"}; // 日志文件路径
        std::string name{"silly"}; // 日志文件名称
        enum_log_type type{2}; // 日志类型
        enum_log_level level{0}; // 日志级别
        size_t rotate_size{20}; // 日志文件大小(MB)
        bool console{true};  // 是否在控制台输出,服务部署时建议设置为false
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
    bool init(const option& opt = option());


    template <typename... Args>
    void debug(Args&&... s);

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

  private:
    std::shared_ptr<spdlog::logger> m_spdlog_debug;
    std::shared_ptr<spdlog::logger> m_spdlog_info;
    std::shared_ptr<spdlog::logger> m_spdlog_warn;
    std::shared_ptr<spdlog::logger> m_spdlog_error;
};

template <typename... Args>
void silly_log::debug(Args&&... s)
{
    if (m_spdlog_debug)
    {
        m_spdlog_debug->debug(std::forward<Args>(s)...);
    }
}
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

#define SLOG_DEBUG(...) silly_log::instance().debug(##__VA_ARGS__);
#define SLOG_INFO(...) silly_log::instance().info(##__VA_ARGS__);
#define SLOG_WARN(...) silly_log::instance().warn(##__VA_ARGS__);
#define SLOG_ERROR(...) silly_log::instance().error(##__VA_ARGS__);