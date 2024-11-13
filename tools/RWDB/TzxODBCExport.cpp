/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: TzxODBCExport.c
 * @description: RWDB数据使用ODBC导出到自定文件
 * @version: v1.0.1 2024-09-30 dou li yang
 */
#include <database/otl/silly_otl.h>
#include <database/otl/otl_tools.h>
#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_stbprp.h>
#include <tzx/rwdb/silly_rsvr.h>
#include <tzx/rwdb/silly_river.h>
#include "export_tool.h"
#include "datetime/silly_posix_time.h"
#include "datetime/silly_timer.h"  // 计时
#include "string/silly_format.h"

// 检查查询数据,不为null 则赋值
#define CHECK_NULL_VALUE(var, dest) \
    if (!(var).is_null())           \
    {                               \
        (dest) = (var).v;           \
    }

// 全局变量
silly_otl otl;
std::string select_stbprp_sql;
std::string select_pptn_sql;
std::string select_river_sql;
std::string select_rsvr_sql;
std::string btm;
std::string etm;
std::string root = "./";  // 文件根路径
std::string src_encode;
std::string dst_encode;
std::unordered_map<std::string, uint32_t> stcd_index;
std::string str_now_tm = silly_posix_time::now().to_string("%Y%m%d%H%M%S");

struct export_obj
{
    bool has_pptn{false};
    bool has_river{false};
    bool has_rsvr{false};
};

// otl_datetime 转 时间戳
std::time_t to_timestamp(const otl_datetime& olt_tm)
{
    // std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
    std::tm t;
    t.tm_sec = olt_tm.second;
    t.tm_min = olt_tm.minute;
    t.tm_hour = olt_tm.hour;
    t.tm_mday = olt_tm.day;
    t.tm_mon = olt_tm.month - 1;
    t.tm_year = olt_tm.year - 1900;
    std::time_t stamp = std::mktime(&t);
    return stamp;
}

export_obj paramAnalysis(int argc, char** argv)
{
    export_obj obj;

    return obj;
}

bool init(const std::string& file);

// 时间分段函数,interval 单位为小时
std::vector<std::pair<std::string, std::string>> splitTime(std::string beginTM, std::string endTM, int interval)
{
    std::vector<std::pair<std::string, std::string>> b_e_tms;

    return b_e_tms;
}

// 将data中的字符串都写入到文件中,追加的写入
bool saveInfor(std::vector<std::string>& data, std::string& filename)
{
    return false;
}

// 根据stcd_index获取stcd对应的index,并写入datas中stcd字段
template <typename T>
void getindex(std::vector<T>& datas)
{
}

// 生成 stcd 对应的 index 结构
bool getStcdIndex(std::vector<silly_stbprp>& stbprp, std::unordered_map<std::string, uint32_t>& stcdindex)
{
    return false;
}

// 数据库加载 STBPRP 数据
bool loadSTBPRP(std::vector<silly_stbprp>& stbprps)
{
    return false;
}

// 序列化 STBPRP 数据为二进制流
bool serializeSTBPRP(const std::vector<silly_stbprp>& stbprp, std::vector<std::string>& datas)
{
    return false;
}

// 导出 STBPRP
bool exportSTBPRP()
{
    bool status = false;

    // 数据库查询 STBPRP
    std::vector<silly_stbprp> stbprps;
    if (!loadSTBPRP(stbprps))
    {
        SLOG_ERROR("STBPRP 数据库加载失败");
        return status;
    }

    // 生成 stcd对应的index 关系
    if (!getStcdIndex(stbprps, stcd_index))
    {
        SLOG_ERROR("STBPRP stcd index 对应关系生成失败");
        return status;
    }

    // 序列化 stbprps 为二进制文件
    std::vector<std::string> datas;
    if (!serializeSTBPRP(stbprps, datas))
    {
        SLOG_ERROR("STBPRP 序列化失败");
        return status;
    }
    // 写入文件
    std::string fileName = str_now_tm + "_" + silly_stbprp::FILE_NAME;
    if (!saveInfor(datas, fileName))
    {
        SLOG_ERROR("STBPRP 写入文件失败");
        return status;
    }

    status = true;
    return status;
}

// 数据库查询 PPTN 数据
bool loadPPTN(const std::string& btm, const std::string& etm, std::vector<silly_pptn>& pptns)
{
    return false;
}

bool serializePPTN(std::vector<silly_pptn>& pptn, std::vector<std::string>& datas)
{
    return false;
}

bool exportPPTN(const std::string& beginTM, const std::string& endTM, const int& hour)
{
    bool status = false;

    std::string fileName = str_now_tm + "_" + silly_pptn::FILE_NAME;
    // 时间分段
    std::vector<std::pair<std::string, std::string>> btm_etm = splitTime(beginTM, endTM, hour);
    for (const auto& [btm, etm] : btm_etm)
    {
        // 数据库查询 PPTN
        std::vector<silly_pptn> pptns;
        if (!loadPPTN(beginTM, endTM, pptns))
        {
            SLOG_ERROR("PPTN 数据库加载失败 ({} ~ {})", beginTM, endTM);
            continue;
        }
        // 根据stcd 获取index
        getindex(pptns);
        // 序列化 pptns 为二进制文件
        std::vector<std::string> datas;
        if (!serializePPTN(pptns, datas))
        {
            SLOG_ERROR("PPTN 序列化失败  ({} ~ {})", beginTM, endTM);
            continue;
        }
        // 写入文件
        if (!saveInfor(datas, fileName))
        {
            SLOG_ERROR("PPTN 写入文件失败  ({} ~ {})", beginTM, endTM);
            continue;
        }
    }

    status = true;
    return status;
}

// 数据库查询 RIVER 数据
bool loadRiver(const std::string& btm, const std::string& etm, std::vector<silly_river>& rivers)
{
    return false;
}

// 序列化 RIVER 数据为二进制流
bool serializeRiver(std::vector<silly_river>& rivers, std::vector<std::string>& datas)
{
    return false;
}

bool exportRiver(const std::string& beginTM, const std::string& endTM, const int& hour)
{
    bool status = false;

    std::string fileName = str_now_tm + "_" + silly_river::FILE_NAME;
    // 时间分段
    std::vector<std::pair<std::string, std::string>> btm_etm = splitTime(beginTM, endTM, hour);
    for (const auto& [btm, etm] : btm_etm)
    {
        // 数据库查询 RIVER
        std::vector<silly_river> rivers;
        if (!loadRiver(beginTM, endTM, rivers))
        {
            SLOG_ERROR("RIVER 数据库加载失败 ({} ~ {})", beginTM, endTM);
            continue;
        }
        // 根据stcd 获取index
        getindex(rivers);
        // 序列化 rivers 为二进制文件
        std::vector<std::string> datas;
        if (!serializeRiver(rivers, datas))
        {
            SLOG_ERROR("RIVER 序列化失败  ({} ~ {})", beginTM, endTM);
            continue;
        }
        // 写入文件
        if (!saveInfor(datas, fileName))
        {
            SLOG_ERROR("RIVER 写入文件失败  ({} ~ {})", beginTM, endTM);
            continue;
        }
    }
    status = true;
    return status;
}

// 数据库查询 RSVR 数据
bool loadRsvr(const std::string& btm, const std::string& etm, std::vector<silly_rsvr>& rsvrs)
{
    return false;
}

bool serializeRsvr(std::vector<silly_rsvr>& rsvrs, std::vector<std::string>& datas)
{
    return false;
}

// 数据库查询 RSVR 数据
bool exportRsvr(const std::string& beginTM, const std::string& endTM, const int& hour)
{
    bool status = false;

    std::string fileName = str_now_tm + "_" + silly_rsvr::FILE_NAME;
    std::vector<std::pair<std::string, std::string>> btm_etm = splitTime(beginTM, endTM, hour);
    for (const auto& [btm, etm] : btm_etm)
    {
        // 数据库查询 RSVR
        std::vector<silly_rsvr> rsvrs;
        if (!loadRsvr(beginTM, endTM, rsvrs))
        {
            SLOG_ERROR("RSVR 数据库加载失败 ({} ~ {})", beginTM, endTM);
            continue;
        }
        // 根据stcd 获取index
        getindex(rsvrs);

        // 序列化 rivers 为二进制文件
        std::vector<std::string> datas;
        if (!serializeRsvr(rsvrs, datas))
        {
            SLOG_ERROR("RSVR 序列化失败  ({} ~ {})", beginTM, endTM);
            continue;
        }
        // 写入文件
        if (!saveInfor(datas, fileName))
        {
            SLOG_ERROR("RSVR 写入文件失败  ({} ~ {})", beginTM, endTM);
            continue;
        }
    }
    // 数据库查询 RSVR
    std::vector<silly_rsvr> rsvrs;

    return status;
}

int main(int argc, char** argv)
{
    // 参数解析
    export_obj obj = paramAnalysis(argc, argv);

// 初始化
#ifndef NDEBUG
    std::string configPath = std::filesystem::path(DEFAULT_SU_ROOT_DIR).append("docs").append("数据库导入导出").append("export.json").string();
#else
    std::string configPath = "./export.json";
#endif

    if (!init(configPath))
    {
        SLOG_ERROR("init failed:{}", configPath);
        return -1;
    }

#if IS_WIN32
    if (otl.type() == enum_database_type::dbORACLE)
    {
        _putenv_s("NLS_LANG", "SIMPLIFIED CHINESE_CHINA.UTF8");
    }
#else
#endif
    silly_timer timer;

    // STBPRP 一定要导出
    if (!exportSTBPRP())
    {
        SLOG_ERROR("STBPRP 导出失败");
        return -1;
    }
    SLOG_INFO("stbprp 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);

    if (obj.has_pptn)
    {
        timer.restart();
        if (!exportPPTN(btm, etm, 24))
        {
            SLOG_ERROR("PPTN 导出失败");
        }
        SLOG_INFO("PPTN 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    if (obj.has_river)
    {
        timer.restart();
        if (!exportRiver(btm, etm, 24))
        {
            SLOG_ERROR("RIVER 导出失败");
        }
        SLOG_INFO("RIVER 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    if (obj.has_rsvr)
    {
        timer.restart();
        if (!exportRsvr(btm, etm, 24))
        {
            SLOG_ERROR("RIVER 导出失败");
        }
        SLOG_INFO("RIVER 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }

    return 0;
}

bool init(const std::string& file)
{
    bool status = false;
    Json::Value jv_root = silly_jsonpp::loadf(file);
    if (jv_root.isNull())
    {
        SFP_ERROR("配置文件读取失败: {}", file);
        return status;
    }
    Json::Value js_db;
    if (!silly_jsonpp::check_member_object(jv_root, "db", js_db))
    {
        SLOG_ERROR("配置文件中缺少 db 字段记录 odbc 链接信息");
        return status;
    }
    std::string js_db_str = silly_jsonpp::dumps(js_db);
    if (!otl.load(js_db_str))
    {
        SLOG_ERROR("解析 odbc 错误");
        return status;
    }
    SLOG_INFO("odbc 链接串: {}", otl.odbc());

    Json::Value js_sql;
    if (silly_jsonpp::check_member_object(jv_root, "sql", js_sql))
    {
        if (!silly_jsonpp::check_member_string(js_sql, "select_stbprp_sql", select_stbprp_sql))
        {
            SLOG_ERROR("配置文件中缺少 select_stbprp_sql 字段");
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "select_pptn_sql", select_pptn_sql))
        {
            SLOG_ERROR("配置文件中缺少 select_pptn_sql 字段");
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "select_river_sql", select_river_sql))
        {
            SLOG_ERROR("配置文件中缺少 select_river_sql 字段");
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "select_rsvr_sql", select_rsvr_sql))
        {
            SLOG_ERROR("配置文件中缺少 select_rsvr_sql 字段");
            return status;
        }
    }

    if (!silly_jsonpp::check_member_string(jv_root, "btm", btm))
    {
        SLOG_ERROR("配置文件中缺少 btm 字段");
        return status;
    }
    if (!silly_jsonpp::check_member_string(jv_root, "etm", etm))
    {
        SLOG_ERROR("配置文件中缺少 etm 字段");
        return status;
    }

    Json::Value js_encode;
    if (silly_jsonpp::check_member_object(jv_root, "encode", js_encode))
    {
        silly_jsonpp::check_member_string(js_encode, "src", src_encode);
        silly_jsonpp::check_member_string(js_encode, "dst", dst_encode);
    }

    status = true;
    return status;
}
