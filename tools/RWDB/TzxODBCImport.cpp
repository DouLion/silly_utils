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
#include <database/otl/otl_tools.h>
#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_stbprp.h>
#include <tzx/rwdb/silly_rsvr.h>
#include <tzx/rwdb/silly_river.h>
#include "tools.h"
#include "datetime/silly_timer.h"  // 计时
#include "files/silly_file.h"

silly_otl otl;
std::string insert_pptn_sql;
std::string insert_stbprp_sql;
std::string insert_river_sql;
std::string insert_rsvr_sql;
std::string stbprp_file_path;
std::string pptn_file_path;
std::string river_file_path;
std::string rsvr_file_path;
std::string btm;
std::string etm;
std::string src_encode;
std::string dst_encode;
std::unordered_map<uint32_t, std::string> index_stcd;

unsigned long long block_byte = 1024 * 1024 * 1024;
// SIZE_T block_byte = 1024 * 1024;



// 模板函数根据index查找stcd
template <typename T>
std::vector<T> getStcd(std::vector<T>& dst_objects)
{
    // 根据 index_stcd 查找并更新 stcd
    std::vector<T> res_objects;
    for (auto& obj : dst_objects)
    {
        uint32_t t_index = obj.index;
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            if (!t_stcd.empty())
            {
                obj.stcd = t_stcd;
                res_objects.push_back(obj);
            }
        }
    }
    return res_objects;
}


// 读取配置文件
bool init(const std::string& file);

// 从文件中一次性读取全部的stbprp数据
bool loadSTBPRP(const std::string& file_path, std::vector<silly_stbprp>& stbprps)
{
    return true;
}

// 插入stbprp到数据库
bool insertSTBPRP(std::vector<silly_stbprp>& stbprps)
{
    return true;
}



// 导入stbprp ,默认不插入数据库指导出stcd和index的映射
bool importSTBPRP(const std::string& file_path, bool import = false)
{
    //  一次性全部读取出stbprp文件中的数据,并反序列化文件字符串中的所有数据
    std::vector<silly_stbprp> stbprps;
    if (!loadSTBPRP(file_path, stbprps))
    {
        SLOG_ERROR("读取stbprp文件失败");
        return false;
    }

    // 根据 stbprps 生成说有 stcd 和 index 的映射
    for (const auto& stbprp : stbprps)
    {
        index_stcd[stbprp.index] = stbprp.STCD;
    }

    // 导入 stbprp 到数据库
    if (import)
    {
        if (!insertSTBPRP(stbprps))
        {
            SLOG_ERROR("导入stbprp失败");
            return false;
        }
    }
}

// 从文件中读取全部的pptn数据,residue_size为剩余数据大小
bool deserializePPTN(const std::string& block_data, std::vector<silly_pptn>& pptns, int& residue_size)
{
    return true;
}

// 导入pptn到数据库
bool insertPPTN(std::vector<silly_pptn>& pptns)
{
    return true;
}

// 导入pptn数据到数据库
bool importPPTN(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024)
{
    bool status = false;

    size_t fsize = silly_file::size(file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(file_path, block_data, pos, block_size);
        std::vector<silly_pptn> pptns;
        int residue_size = 0;
        if (deserializePPTN(block_data, pptns, residue_size))
        {
            SLOG_ERROR("反序列化pptn失败");
            return status;
        }
        getStcd(pptns);
        if (!insertPPTN(pptns))
        {
            SLOG_ERROR("导入pptn失败");
            return status;
        }
        pos = pos + residue_size - residue_size;
    }

    status = true;
    return status;
}

bool deserializeRiver(const std::string& block_data, std::vector<silly_river>& rivers, int& residue_size)
{
    bool status = false;

    status = true;
    return status;
}

bool insertRiver(std::vector<silly_river>& rivers)
{
    bool status = false;

    status = true;
    return status;
}

bool deserializeRsvr(const std::string& block_data, std::vector<silly_rsvr>& rsvrs, int& residue_size)
{
    bool status = false;

    status = true;
    return status;
}

bool insertRsvr(std::vector<silly_rsvr>& rsvrs)
{
    bool status = false;

    status = true;
    return status;
}

// 导入river数据到数据
bool importRiver(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024)
{
    bool status = false;
    size_t fsize = silly_file::size(file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(file_path, block_data, pos, block_size);
        std::vector<silly_river> rivers;
        int residue_size = 0;
        if (deserializeRiver(block_data, rivers, residue_size))
        {
            SLOG_ERROR("反序列化river失败");
            return status;
        }
        getStcd(rivers);
        if (!insertRiver(rivers))
        {
            SLOG_ERROR("导入river失败");
            return status;
        }
        pos = pos + residue_size - residue_size;
    }
}

bool importRsvr(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024)
{
    bool status = false;
    size_t fsize = silly_file::size(file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(file_path, block_data, pos, block_size);
        std::vector<silly_rsvr> rsvrs;
        int residue_size = 0;
        if (deserializeRsvr(block_data, rsvrs, residue_size))
        {
            SLOG_ERROR("反序列化rsvr失败");
            return status;
        }
        getStcd(rsvrs);
        if (!insertRsvr(rsvrs))
        {
            SLOG_ERROR("导入rsvr失败");
            return status;
        }
        pos = pos + residue_size - residue_size;
    }
}

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
    std::vector<silly_stbprp> stbprps;
    if (!loadSTBPRP(stbprp_file_path, stbprps))
    {
        SLOG_ERROR("导入stbprp失败");
        return -1;
    }
    SLOG_INFO("stbprp 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);

    if (_opt.pptn)
    {
        timer.restart();
        if (!importPPTN(pptn_file_path, block_byte))
        {
            SLOG_ERROR("导入PPTN失败");
        }
        SLOG_INFO("导入 PPTN 时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    if (_opt.river)
    {
        timer.restart();
        if (!importRiver(river_file_path, block_byte))
        {
            SLOG_ERROR("导入 River 失败");
        }
        SLOG_INFO("导入 River 时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    if (_opt.rsvr)
    {
        timer.restart();
        if (!importRsvr(rsvr_file_path, block_byte))
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
        SLOG_ERROR("解析 odbc 错误");
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
            SLOG_ERROR("配置文件中缺少 insert_pptn_sql 字段");
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "insert_stbprp_sql", insert_stbprp_sql))
        {
            SLOG_ERROR("配置文件中缺少 insert_stbprp_sql 字段");
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "insert_river_sql", insert_river_sql))
        {
            SLOG_ERROR("配置文件中缺少 insert_river_sql 字段");
            return status;
        }
        if (!silly_jsonpp::check_member_string(js_sql, "insert_rsvr_sql", insert_rsvr_sql))
        {
            SLOG_ERROR("配置文件中缺少 insert_rsvr_sql 字段");
            return status;
        }
    }
    if (!silly_jsonpp::check_member_string(jv_root, "stbprp_file_path", stbprp_file_path))
    {
        SLOG_ERROR("配置文件中缺少 stbprp_file_path 字段");
        return status;
    }
    if (!silly_jsonpp::check_member_string(jv_root, "pptn_file_path", pptn_file_path))
    {
        SLOG_ERROR("配置文件中缺少 pptn_file_path 字段");
        return status;
    }
    if (!silly_jsonpp::check_member_string(jv_root, "river_file_path", river_file_path))
    {
        SLOG_ERROR("配置文件中缺少 river_file_path 字段");
        return status;
    }
    if (!silly_jsonpp::check_member_string(jv_root, "rsvr_file_path", rsvr_file_path))
    {
        SLOG_ERROR("配置文件中缺少 rsvr_file_path 字段");
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
