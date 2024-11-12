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
#include "export_tool.h"
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
std::map<uint32_t, std::string> index_stcd;

SIZE_T max_len = 1024 * 1024 * 1024;
// SIZE_T max_len = 1024 * 1024;

bool has_stbpr;  // 是否导入stbprp

// 读取配置文件
bool init(const std::string& file);

// 从文件中读取stbprp
bool read_stbprp(std::vector<silly_stbprp>& dst_stbprps);
// 导入 stbprp 数据
bool import_stbprp(std::vector<silly_stbprp>& dst_stbprps);

// 从文件中读取pptn
bool read_pptn(std::vector<silly_pptn>& dst_pptns);
// 导入pptn
bool import_pptn(const std::vector<silly_pptn>& dst_pptns);

// 从文件中读取river
bool read_river(std::vector<silly_river>& dst_rivers);
// 导入river
bool import_river(const std::vector<silly_river>& dst_rivers);

// 从文件中读取rsvr
bool read_rsvr(std::vector<silly_rsvr>& dst_rsvrs);
// 导入rsvrv
bool import_rsvr(const std::vector<silly_rsvr>& dst_rsvrs);

int main(int argc, char** argv)
{
    // 初始化布尔变量，默认值为false
    bool has_pptn = false;
    bool has_river = false;
    bool has_rsvr = false;

    // 遍历命令行参数（从索引1开始，因为argv[0]是程序名）
    for (int i = 0; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "pptn")
        {
            has_pptn = true;
        }
        else if (arg == "river")
        {
            has_river = true;
        }
        else if (arg == "rsvr")
        {
            has_rsvr = true;
        }
    }
    if (!has_pptn && !has_river && !has_rsvr)
    {
        SLOG_ERROR("please input pptn or river or rsvr");
        return -1;
    }
#ifndef NDEBUG
    std::string configPath = std::filesystem::path(DEFAULT_SU_ROOT_DIR).append("docs").append("数据库导入导出").append("import.json").string();
#else
    std::string configPath = "./import.json";
#endif

    if (!init(configPath))
    {
        SLOG_ERROR("init failed:{}", configPath);
        return -1;
    }
    SLOG_INFO("读取配置文件完成开始导入数据");

#if IS_WIN32
    if (otl.type() == enum_database_type::dbORACLE)
    {
        _putenv_s("NLS_LANG", "SIMPLIFIED CHINESE_CHINA.UTF8");
    }
#else
#endif

    /////////////////// stbprp ////////////////////////
    // 导入stbprp
    silly_timer timer;
    timer.restart();
    std::vector<silly_stbprp> dst_stbprps;
    if (!read_stbprp(dst_stbprps))
    {
        SLOG_ERROR("读取 stbprp 文件失败");
        return -1;
    }
    if (has_stbpr)  // 确定导入stbprp
    {
        if (!import_stbprp(dst_stbprps))
        {
            SLOG_ERROR("导入 stbprp 失败");
            return -1;
        }
    }
    SLOG_INFO("stbprp 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    dst_stbprps.clear();  // 释放内存

    /////////////////// pptn ////////////////////////
    if (has_pptn)
    {
        // 导入pptn
        timer.restart();
        std::vector<silly_pptn> dst_pptns;
        if (!read_pptn(dst_pptns))
        {
            SLOG_ERROR("读取 pptn 文件失败");
            return -1;
        }
        if (!import_pptn(dst_pptns))
        {
            SLOG_ERROR("导入 pptn 失败");
            return -1;
        }
        SLOG_INFO("pptn 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }

    /////////////////// river ////////////////////////
    if (has_river)
    {
        timer.restart();
        std::vector<silly_river> dst_rivers;
        if (!read_river(dst_rivers))
        {
            SLOG_ERROR("读取 river 文件失败");
            return -1;
        }
        if (!import_river(dst_rivers))
        {
            SLOG_ERROR("导入 rivers 失败");
            return -1;
        }
        SLOG_INFO("river 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }

    /////////////////// rsvr ////////////////////////
    if (has_rsvr)
    {
        timer.restart();
        std::vector<silly_rsvr> dst_rsvrs;
        if (!read_rsvr(dst_rsvrs))
        {
            SLOG_ERROR("读取 rsvr 文件失败");
            return -1;
        }
        if (!import_rsvr(dst_rsvrs))
        {
            SLOG_ERROR("导入 rsvr 失败");
            return -1;
        }
        SLOG_INFO("rsvr 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
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
    otl = otl_tools::conn_opt_from_json(js_db);
    if (otl.odbc().empty())
    {
        SLOG_ERROR("解析 odbc 链接串为空");
        return status;
    }
    SLOG_INFO("odbc 链接串: {}", otl.odbc());

    Json::Value js_sql;
    if (silly_jsonpp::check_member_object(jv_root, "sql", js_sql))
    {
        silly_jsonpp::check_member_string(js_sql, "insert_pptn_sql", insert_pptn_sql);
        silly_jsonpp::check_member_string(js_sql, "insert_stbprp_sql", insert_stbprp_sql);
        silly_jsonpp::check_member_string(js_sql, "insert_river_sql", insert_river_sql);
        silly_jsonpp::check_member_string(js_sql, "insert_rsvr_sql", insert_rsvr_sql);
    }

    silly_jsonpp::check_member_string(jv_root, "stbprp_file_path", stbprp_file_path);
    silly_jsonpp::check_member_string(jv_root, "pptn_file_path", pptn_file_path);
    silly_jsonpp::check_member_string(jv_root, "river_file_path", river_file_path);
    silly_jsonpp::check_member_string(jv_root, "rsvr_file_path", rsvr_file_path);

    Json::Value js_encode;
    if (silly_jsonpp::check_member_object(jv_root, "encode", js_encode))
    {
        silly_jsonpp::check_member_string(js_encode, "src", src_encode);
        silly_jsonpp::check_member_string(js_encode, "dst", dst_encode);
    }

    silly_jsonpp::check_member_bool(jv_root, "has_stbprp", has_stbpr);
    status = true;
    return status;
}

bool read_stbprp(std::vector<silly_stbprp>& dst_stbprps)
{
    bool status = false;
    if (!std::filesystem::exists(stbprp_file_path))
    {
        SLOG_ERROR("文件不存在: {}", stbprp_file_path);
        return status;
    }
    size_t fsize = silly_file::size(stbprp_file_path);
    if (fsize < 4)
    {
        SLOG_ERROR("文件大小小于 4 字节: {}", fsize);
        return status;
    }
    // 如果文件小于 1G,一次性读取,否则分块读取
    if (fsize <= max_len)
    {
        std::string content;
        size_t r = silly_file::read(stbprp_file_path, content, 0, fsize);
        if (r != fsize)
        {
            SLOG_ERROR("文件大小:{}, 读取到的字节数:{},两者不符", fsize, r);
            return status;
        }
        // 逐块解析
        size_t pos = 0;
        while (pos < content.size())
        {
            uint32_t blockLength = 0;  // 读取块长度
            memcpy(&blockLength, content.data() + pos, 4);
            if (pos + blockLength > content.size())  // 检查是否越界
            {
                break;
            }
            std::string block = content.substr(pos, blockLength);
            silly_stbprp stbprp;
            if (stbprp.deserialize(block))  // 反序列化解析文件中的数据
            {
                dst_stbprps.push_back(stbprp);
            }
            pos += blockLength;
        }
    }
    else
    {
        // 如果文件大小大于 1GB，进行分块读取处理
        size_t pos = 0;
        std::string leftoverData;  // 缓存跨越分块边界的数据

        while (pos < fsize)
        {
            std::string content;
            size_t r = silly_file::read(stbprp_file_path, content, pos, max_len);  // 读取当前块大小
            if (r == 0)
            {
                break;  // 如果读取不到数据，退出循环
            }
            size_t currentPos = 0;
            // 如果有上一块的剩余数据添加到当前块开头
            if (!leftoverData.empty())
            {
                content = leftoverData + content;
                leftoverData.clear();
            }

            while (currentPos < content.size())
            {
                uint32_t blockLength = 0;
                memcpy(&blockLength, content.data() + currentPos, 4);

                if (currentPos + blockLength > content.size())
                {
                    leftoverData = content.substr(currentPos);  // 该块不完整的数据,保存到下一块开头
                    break;
                }

                std::string block = content.substr(currentPos, blockLength);
                silly_stbprp stbprp;
                if (stbprp.deserialize(block))
                {
                    dst_stbprps.push_back(stbprp);
                }
                currentPos += blockLength;
            }

            pos += r;
        }
    }

    // --------------index 找 stcd--------------
    for (const auto& entry : dst_stbprps)
    {
        index_stcd[entry.index] = entry.STCD;
    }

    SLOG_INFO("stbprp insert size:{}", dst_stbprps.size());
    status = true;
    return status;
}

bool import_stbprp(std::vector<silly_stbprp>& dst_stbprps)
{
    // --------------转编码-----------------
    if (!src_encode.empty() && !dst_encode.empty())
    {
        for (auto& entry : dst_stbprps)
        {
            encode(entry, src_encode, dst_encode);
        }
    }
    // return true; // 临时添加
    //  --------------插入数据库--------------
    if (!otl.insert(insert_stbprp_sql, [&dst_stbprps](otl_stream* stream) {
            for (const auto& entry : dst_stbprps)
            {
                otl_value<std::string> STCD(entry.STCD);
                otl_value<std::string> STNM(entry.STNM);
                // otl_value<std::string> RVNM(entry.RVNM);
                // otl_value<std::string> HNNM(entry.HNNM);
                // otl_value<std::string> BSNM(entry.BSNM);
                otl_value<double> LGTD(entry.LGTD);
                otl_value<double> LTTD(entry.LTTD);
                // otl_value<std::string> STLC(entry.STLC);
                // otl_value<std::string> ADDVCD(entry.ADDVCD);
                // otl_value<std::string> DTMNM(entry.DTMNM);
                // otl_value<double> DTMEL(entry.DTMEL);
                // otl_value<double> DTPR(entry.DTPR);
                // otl_value<std::string> STTP(entry.STTP);
                // otl_value<std::string> FRGRD(entry.FRGRD);
                // otl_value<std::string> ESSTYM(entry.ESSTYM);
                // otl_value<std::string> BGFRYM(entry.BGFRYM);
                // otl_value<std::string> ATCUNIT(entry.ATCUNIT);
                // otl_value<std::string> ADMAUTH(entry.ADMAUTH);
                // otl_value<std::string> LOCALITY(entry.LOCALITY);
                // otl_value<std::string> STBK(entry.STBK);
                // otl_value<int> STAzt(entry.STAzt);
                // otl_value<double> DSTRVM(entry.DSTRVM);
                // otl_value<int> DRNA(entry.DRNA);
                // otl_value<std::string> PHCD(entry.PHCD);
                // otl_value<std::string> USFL(entry.USFL);
                // otl_value<std::string> COMMENTS(entry.COMMENTS);
                // otl_datetime MODITIME = otl_tools::otl_time_from_string(entry.MODITIME);
                // otl_value<std::string> HNNM0(entry.HNNM0);
                // otl_value<std::string> ADCD(entry.ADCD);
                // otl_value<std::string> ADDVCD1(entry.ADDVCD1);

                otl_write_row(
                    *stream, STCD, STNM /*, RVNM, HNNM, BSNM*/, LGTD, LTTD /*, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1*/);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    SLOG_INFO("{} 导入完成, 导入数量: {}", stbprp_file_path, dst_stbprps.size());

    return true;
}

bool read_pptn(std::vector<silly_pptn>& dst_pptns)
{
    std::vector<silly_pptn> temp_pptns;
    if (index_stcd.empty())
    {
        SLOG_ERROR("index 下标对应的 stcd 为空 ");
        return false;
    }
    if (!std::filesystem::exists(pptn_file_path))
    {
        SLOG_ERROR("文件不存在: {}", pptn_file_path);
    }
    size_t fsize = silly_file::size(pptn_file_path);
    if (fsize <= max_len)
    {
        std::string content;  // 全部文件数据
        size_t r = silly_file::read(pptn_file_path, content, 0, fsize);
        if (r != fsize)
        {
            SLOG_ERROR("文件大小:{}, 读取到的字节数:{},两者不符", fsize, r);
            return false;
        }
        size_t pos = 0;
        while (pos < content.size())
        {
            std::string block = content.substr(pos, silly_pptn::SIZE_V1);
            silly_pptn pptn;
            if (pptn.deserialize(block))
            {
                temp_pptns.push_back(pptn);
            }
            pos += silly_pptn::SIZE_V1;
        }
    }
    else
    {
        // 如果文件大小大于 1GB，进行分块读取处理
        size_t pos = 0;
        std::string leftoverData;  // 缓存跨越分块边界的数据
        while (pos < fsize)
        {
            std::string content;
            size_t r = silly_file::read(pptn_file_path, content, pos, max_len);  // 读取当前块大小
            if (r == 0)
            {
                break;  // 如果读取不到数据，退出循环
            }

            if (!leftoverData.empty())
            {
                content = leftoverData + content;  // 将上一块不完整的数据与当前块拼接
                leftoverData.clear();
            }

            size_t currentPos = 0;
            while (currentPos < content.size())
            {
                if (currentPos + silly_pptn::SIZE_V1 > content.size())
                {
                    leftoverData = content.substr(currentPos);  // 该块不完整的数据,保存到下一块开头
                    break;
                }
                std::string block = content.substr(currentPos, silly_pptn::SIZE_V1);
                silly_pptn pptn;
                if (pptn.deserialize(block))
                {
                    temp_pptns.push_back(pptn);
                }
                currentPos += silly_pptn::SIZE_V1;
            }
            pos += r;
        }
    }

    // -----------index 找 stcd-------------
    for (auto& pptn : temp_pptns)
    {
        uint32_t t_index = pptn.index;
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            if (!t_stcd.empty())
            {
                pptn.stcd = t_stcd;
                dst_pptns.push_back(pptn);
            }
        }
    }
    SLOG_INFO("pptn insert size: {}", dst_pptns.size());
    return true;
}

bool import_pptn(const std::vector<silly_pptn>& dst_pptns)
{
    //  -----------数据插入-------------
    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, dst_pptns.size());
    while (bi < dst_pptns.size())
    {
        if (!otl.insert(insert_pptn_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = dst_pptns[i];

                    otl_value<std::string> stcd(entry.stcd.c_str());
                    struct tm* timeinfo;
                    timeinfo = localtime(&entry.stamp);
                    otl_datetime tm;
                    tm.year = (timeinfo->tm_year + 1900);
                    tm.month = timeinfo->tm_mon + 1;
                    tm.day = timeinfo->tm_mday;
                    tm.hour = timeinfo->tm_hour;
                    tm.minute = timeinfo->tm_min;
                    tm.second = timeinfo->tm_sec;

                    otl_value<double> intv(entry.intv);
                    otl_value<double> drp(entry.drp);

                    otl_write_row(*stream, stcd, tm, drp, intv);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return false;
        }
        SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, dst_pptns.size());
    }

    SLOG_INFO("{} 导入完成, 导入数量: {}", pptn_file_path, dst_pptns.size());

    return true;
}

bool read_river(std::vector<silly_river>& dst_rivers)
{
    std::vector<silly_river> temp_rivers;
    if (index_stcd.empty())
    {
        SLOG_ERROR("index 下标对应的 stcd 为空 ");
        return false;
    }
    if (!std::filesystem::exists(river_file_path))
    {
        SLOG_ERROR("文件不存在: {}", river_file_path);
    }
    size_t fsize = silly_file::size(river_file_path);
    if (fsize <= max_len)
    {
        std::string content;  // 全部文件数据
        size_t r = silly_file::read(river_file_path, content, 0, fsize);
        if (r != fsize)
        {
            SLOG_ERROR("文件大小:{}, 读取到的字节数:{},两者不符", fsize, r);
            return false;
        }
        size_t pos = 0;
        while (pos < content.size())
        {
            std::string block = content.substr(pos, silly_river::SIZE_V2);
            silly_river river;
            if (river.deserialize(block))
            {
                temp_rivers.push_back(river);
            }
            pos += silly_river::SIZE_V2;
        }
    }
    else
    {
        // 如果文件大小大于 1GB，进行分块读取处理
        size_t pos = 0;
        std::string leftoverData;  // 缓存跨越分块边界的数据
        while (pos < fsize)
        {
            std::string content;
            size_t r = silly_file::read(river_file_path, content, pos, max_len);  // 读取当前块大小
            if (r == 0)
            {
                break;  // 如果读取不到数据，退出循环
            }

            if (!leftoverData.empty())
            {
                content = leftoverData + content;  // 将上一块不完整的数据与当前块拼接
                leftoverData.clear();
            }

            size_t currentPos = 0;
            while (currentPos < content.size())
            {
                if (currentPos + silly_river::SIZE_V2 > content.size())
                {
                    leftoverData = content.substr(currentPos);  // 该块不完整的数据,保存到下一块开头
                    break;
                }
                std::string block = content.substr(currentPos, silly_river::SIZE_V2);
                silly_river river;
                if (river.deserialize(block))
                {
                    temp_rivers.push_back(river);
                }
                currentPos += silly_river::SIZE_V2;
            }
            pos += r;
        }
    }

    // -----------index 找 stcd-------------
    for (auto& river : temp_rivers)
    {
        uint32_t t_index = river.index;
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            if (!t_stcd.empty())
            {
                river.stcd = t_stcd;
                dst_rivers.push_back(river);
            }
        }
    }

    SLOG_INFO("rivers insert size: {}", dst_rivers.size());
    return true;
}

bool import_river(const std::vector<silly_river>& dst_rivers)
{
    // -----------数据插入-------------
    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, dst_rivers.size());
    while (bi < dst_rivers.size())
    {
        if (!otl.insert(insert_river_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = dst_rivers[i];
                    otl_value<std::string> stcd(entry.stcd);
                    struct tm* timeinfo;
                    timeinfo = localtime(&entry.stamp);
                    otl_datetime tm;
                    tm.year = (timeinfo->tm_year + 1900);
                    tm.month = timeinfo->tm_mon + 1;
                    tm.day = timeinfo->tm_mday;
                    tm.hour = timeinfo->tm_hour;
                    tm.minute = timeinfo->tm_min;
                    tm.second = timeinfo->tm_sec;

                    otl_value<double> zz(entry.zz);
                    otl_value<double> qq(entry.qq);
                    otl_value<std::string> wptn(entry.wptn);

                    otl_write_row(*stream, stcd, tm, zz, qq, wptn);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return false;
        }
        SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, dst_rivers.size());
    }

    SLOG_INFO("{} 导入完成, 导入数量: {}", river_file_path, dst_rivers.size());

    return true;
}

bool read_rsvr(std::vector<silly_rsvr>& dst_rsvrs)
{
    std::vector<silly_rsvr> temp_rsvrs;
    if (index_stcd.empty())
    {
        SLOG_ERROR("index 下标对应的 stcd 为空 ");
        return false;
    }
    if (!std::filesystem::exists(rsvr_file_path))
    {
        SLOG_ERROR("文件不存在: {}", rsvr_file_path);
    }
    size_t fsize = silly_file::size(rsvr_file_path);
    if (fsize <= max_len)
    {
        std::string content;  // 全部文件数据
        size_t r = silly_file::read(rsvr_file_path, content, 0, fsize);
        if (r != fsize)
        {
            SLOG_ERROR("文件大小:{}, 读取到的字节数:{},两者不符", fsize, r);
            return false;
        }
        size_t pos = 0;
        while (pos < content.size())
        {
            std::string block = content.substr(pos, silly_rsvr::SIZE_V1);
            silly_rsvr river;
            if (river.deserialize(block))
            {
                temp_rsvrs.push_back(river);
            }
            pos += silly_rsvr::SIZE_V1;
        }
    }
    else
    {
        // 如果文件大小大于 1GB，进行分块读取处理
        size_t pos = 0;
        std::string leftoverData;  // 缓存跨越分块边界的数据
        while (pos < fsize)
        {
            std::string content;
            size_t r = silly_file::read(rsvr_file_path, content, pos, max_len);  // 读取当前块大小
            if (r == 0)
            {
                break;  // 如果读取不到数据，退出循环
            }

            if (!leftoverData.empty())
            {
                content = leftoverData + content;  // 将上一块不完整的数据与当前块拼接
                leftoverData.clear();
            }

            size_t currentPos = 0;
            while (currentPos < content.size())
            {
                if (currentPos + silly_rsvr::SIZE_V1 > content.size())
                {
                    leftoverData = content.substr(currentPos);  // 该块不完整的数据,保存到下一块开头
                    break;
                }
                std::string block = content.substr(currentPos, silly_rsvr::SIZE_V1);
                silly_rsvr river;
                if (river.deserialize(block))
                {
                    temp_rsvrs.push_back(river);
                }
                currentPos += silly_rsvr::SIZE_V1;
            }
            pos += r;
        }
    }

    // -----------index 找 stcd-------------
    for (auto& river : temp_rsvrs)
    {
        uint32_t t_index = river.index;
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            if (!t_stcd.empty())
            {
                river.stcd = t_stcd;
                dst_rsvrs.push_back(river);
            }
        }
    }

    SLOG_INFO("rivers insert size: {}", dst_rsvrs.size());
    return true;
}

bool import_rsvr(const std::vector<silly_rsvr>& dst_rsvrs)
{
    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, dst_rsvrs.size());
    while (bi < dst_rsvrs.size())
    {
        if (!otl.insert(insert_rsvr_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = dst_rsvrs[i];

                    otl_value<std::string> STCD(entry.stcd);
                    otl_datetime OTM;
                    struct tm* timeinfo;
                    timeinfo = localtime(&entry.stamp);
                    OTM.year = (timeinfo->tm_year + 1900);
                    OTM.month = timeinfo->tm_mon + 1;
                    OTM.day = timeinfo->tm_mday;
                    OTM.hour = timeinfo->tm_hour;
                    OTM.minute = timeinfo->tm_min;
                    OTM.second = timeinfo->tm_sec;
                    otl_value<double> RZ(entry.rz);
                    otl_value<double> INQ(entry.inq);
                    otl_value<double> W(entry.w);
                    otl_value<double> OTQ(entry.otq);
                    otl_value<std::string> RWCHRCD(entry.rwchrcd);
                    otl_value<std::string> RWPTN(entry.rwptn);
                    otl_value<double> INQDR(entry.inqdr);
                    otl_value<std::string> MSQMT(entry.msqmt);
                    otl_value<double> BLRZ(entry.blrz);

                    otl_write_row(*stream, STCD, OTM, RZ, INQ, W, OTQ, RWCHRCD, RWPTN, INQDR, MSQMT, BLRZ);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return false;
        }
        SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, dst_rsvrs.size());
    }

    SLOG_INFO("{} 导入完成, 导入数量: {}", rsvr_file_path, dst_rsvrs.size());

    return true;
}
