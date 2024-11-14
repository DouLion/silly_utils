/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-30
 * @file: TzxODBCImport.c
 * @description: 使用ODBC导入到自定文件到RWDB数据库
 * @version: v1.0.1 2024-09-30 dou li yang
 */

#include <database/otl/silly_otl.h>
#include "tools.h"
#include "silly_rwdb_stbprp.h"
#include "silly_rwdb_pptn.h"
#include "silly_rwdb_river.h"
#include "silly_rwdb_rsvr.h"

#include "datetime/silly_timer.h"  // 计时

silly_otl otl;
std::string insert_pptn_sql;
std::string insert_stbprp_sql;
std::string insert_river_sql;
std::string insert_rsvr_sql;
std::string stbprp_file_path;
std::string pptn_file_path;
std::string river_file_path;
std::string rsvr_file_path;
std::string src_encode;
std::string dst_encode;
std::unordered_map<uint32_t, std::string> g_index_stcd;  // 导入

//
std::string select_stbprp_sql;
std::string select_pptn_sql;
std::string select_river_sql;
std::string select_rsvr_sql;
std::string str_now_tm;
std::unordered_map<std::string, uint32_t> g_stcd_index;  // 导出

unsigned long long block_byte = 1024 * 1024 * 1024;
// SIZE_T block_byte = 1024 * 1024;

// 读取配置文件
bool init(const std::string& file);

int main(int argc, char** argv)
{
    // 参数解析
    paramAnalysis(argc, argv);

#ifndef NDEBUG
    std::string configPath = std::filesystem::path(DEFAULT_SU_ROOT_DIR).append("docs").append("数据库导入导出").append("import.json").string();
#else
    std::string configPath = "./import.json";
#endif

    if (!init(configPath))
    {
        return -1;
    }
    SLOG_INFO("读取配置文件完成开始导入数据");

    silly_timer timer;

    //////////// 解析 index 和 stcd对应关系 /////////////
    std::vector<silly_stbprp> stbprps;
    silly_rwdb_stbprp::setInsertStbprpSql(insert_stbprp_sql);
    silly_rwdb_stbprp::setStbprpFilePath(stbprp_file_path);
    if (!silly_rwdb_stbprp::import())
    {
        SLOG_ERROR("导入stbprp失败");
        return -1;
    }
    SLOG_INFO("stbprp 解析时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);

    //////////// 导入数据 /////////////

    if (_opt.stbprp)  // 默认不导入
    {
        if (!silly_rwdb_stbprp::insert())
        {
            SLOG_ERROR("导入stbprp失败");
            return false;
        }
        SLOG_INFO("stbprp 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    stbprps.clear();
    if (_opt.pptn)
    {
        timer.restart();
        silly_rwdb_pptn::setInsertPPTNsql(insert_pptn_sql);
        silly_rwdb_pptn::setPPTNFilePath(pptn_file_path);
        if (!silly_rwdb_pptn::import(block_byte))
        {
            SLOG_ERROR("导入PPTN失败");
        }
        SLOG_INFO("导入 PPTN 时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    if (_opt.river)
    {
        timer.restart();
        silly_rwdb_river::setInsertRiverSql(insert_river_sql);
        silly_rwdb_river::setRiverFilePath(river_file_path);
        if (!silly_rwdb_river::import(block_byte))
        {
            SLOG_ERROR("导入 River 失败");
        }
        SLOG_INFO("导入 River 时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    if (_opt.rsvr)
    {
        timer.restart();
        silly_rwdb_rsvr::setInsertRsvrSql(insert_rsvr_sql);
        silly_rwdb_rsvr::setRsvrFilePath(rsvr_file_path);
        if (!silly_rwdb_rsvr::import(block_byte))
        {
            SLOG_ERROR("导入 Rsvr 失败");
        }
        SLOG_INFO("导入 Rsvr 时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }

    return 0;
}

bool init(const std::string& file)
{
    bool status = false;
    Json::Value jv_root = silly_jsonpp::loadf(file);
    if (jv_root.isNull())
    {
        SLOG_ERROR("配置文件读取失败: {}", file);
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
        if (!silly_jsonpp::check_member_string(js_sql, "insert_pptn_sql", insert_pptn_sql))
        {
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "insert_stbprp_sql", insert_stbprp_sql))
        {
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "insert_river_sql", insert_river_sql))
        {
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "insert_rsvr_sql", insert_rsvr_sql))
        {
            return status;
        }
    }
    if (!silly_jsonpp::check_member_string(jv_root, "stbprp_file_path", stbprp_file_path))
    {
        return status;
    }
    if (!silly_jsonpp::check_member_string(jv_root, "pptn_file_path", pptn_file_path))
    {
        return status;
    }
    if (!silly_jsonpp::check_member_string(jv_root, "river_file_path", river_file_path))
    {
        return status;
    }
    if (!silly_jsonpp::check_member_string(jv_root, "rsvr_file_path", rsvr_file_path))
    {
        return status;
    }
    // block_byte
    if (!silly_jsonpp::check_member_uint64(jv_root, "block_byte", block_byte))
    {
        SLOG_WARN("配置文件中缺少 block_byte 字段, 默认按照 1G 字节数处理");
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
