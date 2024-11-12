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

unsigned long long block_byte = 1024 * 1024 * 1024;
// SIZE_T block_byte = 1024 * 1024;

// 转换时间戳为otl_datetime
otl_datetime from_timestamp(std::time_t timestamp)
{
    std::tm* t = std::localtime(&timestamp);  // 获取本地时间的 std::tm 结构
    // 将 std::tm 转换为 otl_datetime
    otl_datetime olt_tm;
    olt_tm.year = t->tm_year + 1900;  // tm_year 是从 1900 年开始的，所以加 1900
    olt_tm.month = t->tm_mon + 1;     // tm_mon 是从 0 开始的，所以加 1
    olt_tm.day = t->tm_mday;
    olt_tm.hour = t->tm_hour;
    olt_tm.minute = t->tm_min;
    olt_tm.second = t->tm_sec;

    return olt_tm;
}

// 模板函数根据index查找stcd
template <typename T>
std::vector<T> indexToStcd(std::vector<T>& dst_objects)
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

// 从 once_obj_str 中解码 pptn
bool decode_pptn(const std::string& once_obj_str, std::vector<silly_pptn>& dst_pptn);
// 从 once_obj_str 中解码 stbprp
bool decode_river(const std::string& once_obj_str, std::vector<silly_river>& dst_rivers);
// 从 once_obj_str 中解码 rsvr
bool decode_rsvr(const std::string& once_obj_str, std::vector<silly_rsvr>& dst_rsvrs);

// 从文件中读取stbprp
bool read_stbprp(std::vector<silly_stbprp>& dst_stbprps);
// 导入 stbprp 数据
bool import_stbprp(std::vector<silly_stbprp>& dst_stbprps);

//  导入pptn
bool import_pptn(const std::vector<silly_pptn>& dst_pptns);

//  导入river
bool import_river(const std::vector<silly_river>& dst_rivers);

//  导入rsvrv
bool import_rsvr(const std::vector<silly_rsvr>& dst_rsvrs);

// 根据type 选择对应的文件 分块读取,解析一块,插入一块的数据到数据库
bool blcok_read_decode_import(int type);

int main(int argc, char** argv)
{
    // 初始化布尔变量，默认值为false
    int type = 0;
    bool has_pptn = false;
    bool has_river = false;
    bool has_rsvr = true;

    bool has_stbprp = false;

    // 遍历命令行参数（从索引1开始，因为argv[0]是程序名）
    for (int i = 0; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "pptn")
        {
            has_pptn = true;
            type = 1;
        }
        else if (arg == "river")
        {
            has_river = true;
            type = 2;
        }
        else if (arg == "rsvr")
        {
            has_rsvr = true;
            type = 3;
        }
        else if (arg == "stbprp")
        {
            has_stbprp = true;
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
    if (has_stbprp)  // 确定导入stbprp
    {
        if (!import_stbprp(dst_stbprps))
        {
            SLOG_ERROR("导入 stbprp 失败");
            return -1;
        }
    }
    SLOG_INFO("stbprp 导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    dst_stbprps.clear();  // 释放内存

    timer.restart();
    blcok_read_decode_import(type);
    SLOG_INFO("导入时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);

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
    // 一次性读取
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
                    otl_datetime tm = from_timestamp(entry.stamp);
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
                    otl_datetime tm = from_timestamp(entry.stamp);
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
                    otl_datetime OTM = from_timestamp(entry.stamp);
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

bool blcok_read_decode_import(int type)
{
    std::string file_path;
    unsigned int object_size = 0;
    if (type == 1)
    {
        file_path = pptn_file_path;
        object_size = silly_pptn::SIZE_V1;
        SLOG_INFO("导入 PPTN 数据");
    }
    else if (type == 2)
    {
        file_path = river_file_path;
        object_size = silly_river::SIZE_V2;
        SLOG_INFO("导入 RIVER 数据");
    }
    else if (type == 3)
    {
        file_path = rsvr_file_path;
        object_size = silly_rsvr::SIZE_V1;
        SLOG_INFO("导入 RSVR 数据");
    }
    else
    {
        SLOG_ERROR("type 参数错误:{}", type);
        return false;
    }

    if (index_stcd.empty())
    {
        SLOG_ERROR("index 下标对应的 stcd 为空 ");
        return false;
    }
    if (!std::filesystem::exists(file_path))
    {
        SLOG_ERROR("文件不存在: {}", file_path);
    }
    size_t all_insert_size = 0;
    size_t fsize = silly_file::size(file_path);
    size_t pos = 0;
    std::string leftoverData;  // 缓存跨越分块边界的数据
    while (pos < fsize)
    {
        std::string content;
        size_t onceBlock = silly_file::read(file_path, content, pos, block_byte);  // 读取当前块大小
        if (onceBlock <= 0)
        {
            break;
        }

        if (!leftoverData.empty())
        {
            content = leftoverData + content;  // 将上一块不完整的数据与当前块拼接
            leftoverData.clear();
        }

        std::vector<silly_pptn> dst_pptns;
        std::vector<silly_river> dst_rivers;
        std::vector<silly_rsvr> dst_rsvrs;

        size_t currentPos = 0;
        while (currentPos < content.size())
        {
            if (currentPos + object_size > content.size())
            {
                leftoverData = content.substr(currentPos);  // 该块不完整的数据,保存到下一块开头
                break;
            }
            std::string once_obj_str = content.substr(currentPos, object_size);
            if (type == 1)
            {
                decode_pptn(once_obj_str, dst_pptns);
            }
            else if (type == 2)
            {
                decode_river(once_obj_str, dst_rivers);
            }
            else if (type == 3)
            {
                decode_rsvr(once_obj_str, dst_rsvrs);
            }
            currentPos += object_size;
        }

        // 解析完一块数据后,插入数据库
        if (type == 1)
        {
            std::vector<silly_pptn> stcd_ppnts = indexToStcd(dst_pptns);
            // 插入数据库
            if (import_pptn(stcd_ppnts))
            {
                all_insert_size += stcd_ppnts.size();
            }
        }
        else if (type == 2)
        {
            std::vector<silly_river> stcd_rivers = indexToStcd(dst_rivers);
            // 插入数据库
            if (import_river(stcd_rivers))
            {
                all_insert_size += stcd_rivers.size();
            }
        }
        else if (type == 3)
        {
            std::vector<silly_rsvr> stcd_rivers = indexToStcd(dst_rsvrs);
            if (import_rsvr(stcd_rivers))
            {
                all_insert_size += dst_rsvrs.size();
            }
        }
        pos += onceBlock;
    }

    SLOG_INFO("插入数据量: {}", all_insert_size);
    return true;
}

// 从字符串中解码出 river 对象 并插入数据库
bool decode_import_river(const std::string& content, std::string& leftoverData, size_t& num)
{
    std::vector<silly_river> temp_rivers;
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
    }
    // 根据 index_stcd 查找并更新 stcd
    std::vector<silly_river> stcd_rivers = indexToStcd(temp_rivers);

    // 插入数据库
    if (!import_river(stcd_rivers))
    {
        return false;
    }
    num += stcd_rivers.size();
    return true;
}

bool decode_pptn(const std::string& once_obj_str, std::vector<silly_pptn>& dst_pptn)
{
    silly_pptn pptn;
    if (!pptn.deserialize(once_obj_str))
    {
        return false;
    }
    dst_pptn.push_back(pptn);
    return true;
}

bool decode_river(const std::string& once_obj_str, std::vector<silly_river>& dst_rivers)
{
    silly_river river;
    if (!river.deserialize(once_obj_str))
    {
        return false;
    }
    dst_rivers.push_back(river);
    return true;
}

bool decode_rsvr(const std::string& once_obj_str, std::vector<silly_rsvr>& dst_rsvrs)
{
    silly_rsvr river;
    if (!river.deserialize(once_obj_str))
    {
        return false;
    }
    dst_rsvrs.push_back(river);
    return true;
}
