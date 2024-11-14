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
#include "tools.h"
#include "silly_rwdb_stbprp.h"
#include "silly_rwdb_pptn.h"
#include "silly_rwdb_river.h"
#include "silly_rwdb_rsvr.h"

#include "datetime/silly_timer.h"  // 计时

// 全局变量
silly_otl otl;
std::string select_stbprp_sql;
std::string select_pptn_sql;
std::string select_river_sql;
std::string select_rsvr_sql;
std::string btm;
std::string etm;
std::string src_encode;
std::string dst_encode;
std::string str_now_tm;


std::string insert_pptn_sql;
std::string insert_stbprp_sql;
std::string insert_river_sql;
std::string insert_rsvr_sql;
std::string stbprp_file_path;
std::string pptn_file_path;
std::string river_file_path;
std::string rsvr_file_path;


// 读取配置文件
bool init(const std::string& file);

int main(int argc, char** argv)
{
    // 参数解析
    paramAnalysis(argc, argv);
    str_now_tm = silly_posix_time::now().to_string("%Y%m%d%H%M%S");

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


    silly_timer timer;
    silly_rwdb_stbprp::setSelectStbprpSql(select_stbprp_sql);
    silly_rwdb_stbprp::setNowTime(str_now_tm);
    // STBPRP 一定要导出
    if (!silly_rwdb_stbprp::output())
    {
        SLOG_ERROR("STBPRP 导出失败");
        return -1;
    }
    SLOG_INFO("stbprp 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);

    // 时间分段
    std::vector<std::pair<std::string, std::string>> btm_etm = splitTime(btm, etm, 12);

    if (_opt.pptn)
    {
        timer.restart();
        silly_rwdb_pptn::setSelectPPTNsql(select_pptn_sql);
        silly_rwdb_pptn::setNowTime(str_now_tm);
        if (!silly_rwdb_pptn::output(btm_etm))
        {
            SLOG_ERROR("PPTN 导出失败");
        }
        SLOG_INFO("PPTN 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    if (_opt.river)
    {
        timer.restart();
        silly_rwdb_river::setSelectRiverSql(select_river_sql);
        silly_rwdb_river::setNowTime(str_now_tm);
        if (!silly_rwdb_river::output(btm_etm))
        {
            SLOG_ERROR("RIVER 导出失败");
        }
        SLOG_INFO("RIVER 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    if (_opt.rsvr)
    {
        timer.restart();
        silly_rwdb_rsvr::setSelectRsvrSql(select_rsvr_sql);
        silly_rwdb_rsvr::setNowTime(str_now_tm);
        if (!silly_rwdb_rsvr::output(btm_etm))
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
        SLOG_ERROR("解析 odbc 错误: {}", otl.odbc());
        return status;
    }
    SLOG_INFO("odbc 链接串: {}", otl.odbc());

#if IS_WIN32
    if (otl.type() == enum_database_type::dbORACLE)
    {
        _putenv_s("NLS_LANG", "SIMPLIFIED CHINESE_CHINA.UTF8");
    }
#else
#endif

    Json::Value js_sql;
    if (silly_jsonpp::check_member_object(jv_root, "sql", js_sql))
    {
        if (!silly_jsonpp::check_member_string(js_sql, "select_stbprp_sql", select_stbprp_sql))
        {
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "select_pptn_sql", select_pptn_sql))
        {
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "select_river_sql", select_river_sql))
        {
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "select_rsvr_sql", select_rsvr_sql))
        {
            return status;
        }
    }

    if (!silly_jsonpp::check_member_string(jv_root, "btm", btm))
    {
        return status;
    }
    if (!silly_jsonpp::check_member_string(jv_root, "etm", etm))
    {
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
