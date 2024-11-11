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

// 全局变量
struct encode
{
    std::string src;
    std::string dst;
};
silly_otl otl;
std::string select_stbprp_sql;
std::string select_pptn_sql;
std::string select_river_sql;
std::string select_rsvr_sql;
std::string btm;
std::string etm;
std::string root = "./";  // 文件根路径
struct encode cvt;
std::unordered_map<std::string, uint32_t> stcd_index;
std::string str_now_tm = silly_posix_time::now().to_string("%Y%m%d%H%M%S");

bool init(const std::string& file);

// 导出测站基本信息
bool export_stbprp();
// 导出降雨记录
bool export_pptn();

// 查询 pptn 表中从 begtm 到 endtm 的数据
std::vector<silly_pptn> query_pptn(const std::string& begtm, const std::string& endtm);
// 将 pptn 表中的数据导出到二进制流中
std::vector<std::string> pptn_strs(std::vector<silly_pptn>& pptns);

// 查询 river 表中从 begtm 到 endtm 的数据
std::vector<silly_river> query_river(const std::string& begtm, const std::string& endtm);
// 将river 表中的数据导出到二进制流中
std::vector<std::string> river_strs(std::vector<silly_river>& rivers);

// 查询 rsvr 表中从 begtm 到 endtm 的数据
std::vector<silly_rsvr> query_rsvr(const std::string& begtm, const std::string& endtm);
// 将 rsvr 表中的数据导出到二进制流中
std::vector<std::string> rsvr_strs(std::vector<silly_rsvr>& rsvrs);

bool export_type(const int& type);

int main(int argc, char** argv)
{
    int type = 0;
    // 遍历命令行参数（从索引1开始，因为argv[0]是程序名）
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "pptn")
        {
            type = 1;
        }
        else if (arg == "river")
        {
            type = 2;
        }
        else if (arg == "rsvr")
        {
            type = 3;
        }
        else
        {
            SLOG_ERROR("please input pptn or river or rsvr");
            // return -1;
        }
    }

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

    // 导出stbprp
    if (!export_stbprp())
    {
        SLOG_ERROR("export_stbprp failed");
        return -1;
    }
    SLOG_INFO("stbprp 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);

    if (type == 1)
    {
        timer.restart();
        // 导出pptn
        if (!export_type(1))
        {
            SLOG_ERROR("export_pptn failed");
        }
        SLOG_INFO("pptn 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    else if (type == 2)
    {
        timer.restart();
        // 导出river
        if (!export_type(2))
        {
            SLOG_ERROR("export_river failed");
        }
        SLOG_INFO("river 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }
    else if (type == 3)
    {
        timer.restart();
        // 导出rsvr
        if (!export_type(3))
        {
            SLOG_ERROR("export_rsvr failed");
        }
        SLOG_INFO("rsvr 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
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
    otl = otl_tools::conn_opt_from_json(jv_root["db"]);
    otl.odbc();
    select_stbprp_sql = jv_root["sql"]["select_stbprp_sql"].asString();
    select_pptn_sql = jv_root["sql"]["select_pptn_sql"].asString();
    select_river_sql = jv_root["sql"]["select_river_sql"].asString();
    select_rsvr_sql = jv_root["sql"]["select_rsvr_sql"].asString();

    btm = jv_root["btm"].asString();
    etm = jv_root["etm"].asString();

    cvt.src = jv_root["encode"]["src"].asString();
    cvt.dst = jv_root["encode"]["dst"].asString();

    status = true;
    return status;
}

bool export_stbprp()
{
    std::vector<silly_stbprp> stbprps;
    // ---------查询数据库-----------
    if (!otl.select(select_stbprp_sql, [&stbprps](otl_stream* stream) {
            while (!stream->eof())
            {
                otl_value<std::string> STCD, STNM, RVNM, HNNM, BSNM, STLC, ADDVCD, DTMNM, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, PHCD, USFL, COMMENTS, HNNM0, ADCD, ADDVCD1;
                otl_value<double> LGTD, LTTD, DTMEL, DTPR, DSTRVM;
                otl_value<int> STAzt, DRNA;
                otl_datetime MODITIME;

                otl_read_row(
                    *stream, STCD, STNM /*, RVNM, HNNM, BSNM*/, LGTD, LTTD /*, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1*/);
                silly_stbprp temp;

                // 检查并赋值
                if (!STCD.is_null())
                {
                    temp.STCD = STCD.v;
                }
                if (!STNM.is_null())
                {
                    temp.STNM = STNM.v;
                }
                /* if (!RVNM.is_null()) { temp.RVNM = RVNM.v; }
                 if (!HNNM.is_null()) { temp.HNNM = HNNM.v; }
                 if (!BSNM.is_null()) { temp.BSNM = BSNM.v; }*/
                if (!LGTD.is_null())
                {
                    temp.LGTD = LGTD.v;
                }
                if (!LTTD.is_null())
                {
                    temp.LTTD = LTTD.v;
                }
                /* if (!STLC.is_null()) { temp.STLC = STLC.v; }
                if (!ADDVCD.is_null()) { temp.ADDVCD = ADDVCD.v; }
                if (!DTMNM.is_null()) { temp.DTMNM = DTMNM.v; }
                if (!DTMEL.is_null()) { temp.DTMEL = DTMEL.v; }
                if (!DTPR.is_null()) { temp.DTPR = DTPR.v; }
                if (!STTP.is_null()) { temp.STTP = STTP.v; }
                if (!FRGRD.is_null()) { temp.FRGRD = FRGRD.v; }
                if (!ESSTYM.is_null()) { temp.ESSTYM = ESSTYM.v; }
                if (!BGFRYM.is_null()) { temp.BGFRYM = BGFRYM.v; }
                if (!ATCUNIT.is_null()) { temp.ATCUNIT = ATCUNIT.v; }
                if (!ADMAUTH.is_null()) { temp.ADMAUTH = ADMAUTH.v; }
                if (!LOCALITY.is_null()) { temp.LOCALITY = LOCALITY.v; }
                if (!STBK.is_null()) { temp.STBK = STBK.v; }
                if (!STAzt.is_null()) { temp.STAzt = STAzt.v; }
                if (!DSTRVM.is_null()) { temp.DSTRVM = DSTRVM.v; }
                if (!DRNA.is_null()) { temp.DRNA = DRNA.v; }
                if (!PHCD.is_null()) { temp.PHCD = PHCD.v; }
                if (!USFL.is_null()) { temp.USFL = USFL.v; }
                if (!COMMENTS.is_null()) { temp.COMMENTS = COMMENTS.v; }
                temp.MODITIME = otl_tools::otl_time_to_string(MODITIME);
                if (!HNNM0.is_null()) { temp.HNNM0 = HNNM0.v; }
                if (!ADCD.is_null()) { temp.ADCD = ADCD.v; }
                if (!ADDVCD1.is_null()) { temp.ADDVCD1 = ADDVCD1.v; }*/

                stbprps.push_back(temp);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        SLOG_ERROR(silly_encode::gbk_utf8(otl.err()));
        return false;
    }

    // ------------添加index------------
    uint32_t index = 1;
    for (auto& stbprp : stbprps)
    {
        stbprp.index = index;
        stcd_index[stbprp.STCD] = index;
        index++;
    }
    SLOG_INFO("查询到 stbprp 数据:{} 条数据开始导出到文件", stbprps.size());

    // -----------转编码--------------
    if (!cvt.src.empty() && !cvt.dst.empty())
    {
        for (auto& stbprp : stbprps)
        {
            encode(stbprp, cvt.src, cvt.dst);
        }
    }

    // -----------导出stbprp文件------------
    std::filesystem::path stbprp_file_path(root);
    std::string fileName = str_now_tm + "_" + silly_stbprp::FILE_NAME;
    stbprp_file_path.append(fileName);
    std::filesystem::create_directories(stbprp_file_path.parent_path());
    std::ofstream ofs(stbprp_file_path.string(), std::ios::binary);
    for (auto& stbprp : stbprps)
    {
        ofs << stbprp.serialize();
    }
    ofs.close();

    SLOG_INFO("导出 stbprp 文件 {}  完成, 导出 {} 条数据", stbprp_file_path.string(), stbprps.size());

    return true;
}

bool export_pptn()
{
    std::filesystem::path ppth_file_path(root);
    std::string fileName = str_now_tm + "_" + silly_pptn::FILE_NAME;
    ppth_file_path.append(fileName);
    std::filesystem::create_directories(ppth_file_path.parent_path());

    silly_posix_time startTime = silly_posix_time::time_from_string(btm);
    silly_posix_time endTime = silly_posix_time::time_from_string(etm);
    silly_time_duration minutes(5 * 24, 0, 0);  // 5天
    SLOG_INFO("查询 pptn 截止时间: {}, etm: {}", btm, etm);
    size_t all_index = 0;
    size_t nstdcindex = 0;

    for (silly_posix_time currentTm = startTime; currentTm < endTime; currentTm += minutes)
    {
        std::string btm_str = currentTm.to_string();
        silly_posix_time temp = currentTm;
        temp += minutes;
        if (temp > endTime)
        {
            temp = endTime;
        }
        std::string etm_str = temp.to_string();

        std::vector<silly_pptn> pptns;
        if (!otl.select(select_pptn_sql, [&pptns, btm_str, etm_str](otl_stream* stream) {
                otl_write_row(*stream, btm_str, etm_str);  // 传入参数
                while (!stream->eof())
                {
                    otl_value<std::string> STCD;
                    otl_value<double> DRP, INTV;
                    otl_datetime tm;
                    otl_read_row(*stream, STCD, tm, DRP, INTV);
                    silly_pptn tmp_pptn;

                    if (!STCD.is_null())
                    {
                        tmp_pptn.stcd = STCD.v;
                    }
                    if (!DRP.is_null())
                    {
                        tmp_pptn.drp = DRP.v;
                    }
                    if (!INTV.is_null())
                    {
                        tmp_pptn.intv = INTV.v;
                    }

                    std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
                    std::time_t stamp = std::mktime(&t);
                    tmp_pptn.stamp = stamp;

                    pptns.push_back(tmp_pptn);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return false;
        }
        for (auto& pptn : pptns)
        {
            if (stcd_index.find(pptn.stcd) == std::end(stcd_index))
            {
                pptn.index = 0;
                continue;
                nstdcindex++;
            }
            pptn.index = stcd_index[pptn.stcd];
        }
        SLOG_INFO("查询到:{} 条数据开始导出到文件", pptns.size());

        std::ofstream ofs(ppth_file_path.string(), std::ios::binary | std::ios::app);
        for (auto& pptn : pptns)
        {
            ofs << pptn.serialize();
        }
        ofs.close();
        std::cout << "btm: " << btm_str << ", etm: " << etm_str << ", size:" << pptns.size() << std::endl;
        all_index += pptns.size();
    }
    SLOG_INFO("pptn not found stcd: {}", nstdcindex);

    SLOG_INFO("{} 导出 pptn 完成, 导出 {} 条数据", ppth_file_path.string(), std::to_string(all_index));

    return true;
}

bool export_type(const int& type)
{
    std::filesystem::path out_file_path(root);
    std::string fileName;
    if (type == 1)
    {
        fileName = str_now_tm + "_" + silly_pptn::FILE_NAME;
    }
    else if (type == 2)
    {
        fileName = str_now_tm + "_" + silly_river::FILE_NAME;
    }
    else if (type == 3)
    {
        fileName = str_now_tm + "_" + silly_rsvr::FILE_NAME;
    }
    out_file_path.append(fileName);
    std::filesystem::create_directories(out_file_path.parent_path());

    silly_posix_time startTime = silly_posix_time::time_from_string(btm);
    silly_posix_time endTime = silly_posix_time::time_from_string(etm);
    silly_time_duration minutes(5 * 24, 0, 0);  // 10天
    SLOG_INFO("查询 river 截止时间: {}, etm: {}", btm, etm);
    size_t all_index = 0;
    for (silly_posix_time currentTm = startTime; currentTm < endTime; currentTm += minutes)
    {
        std::string btm_str = currentTm.to_string();
        silly_posix_time temp = currentTm;
        temp += minutes;
        if (temp > endTime)
        {
            temp = endTime;
        }
        std::string etm_str = temp.to_string();

        std::vector<std::string> rows;
        if (type == 1)  // pptn
        {
            std::vector<silly_pptn> pptns = query_pptn(btm_str, etm_str);
            rows = pptn_strs(pptns);
        }
        else if (type == 2)  // river
        {
            std::vector<silly_river> rivers = query_river(btm_str, etm_str);
            rows = river_strs(rivers);
        }
        else if (type == 3)  // rsvr
        {
            std::vector<silly_rsvr> rsvrs = query_rsvr(btm_str, etm_str);
            rows = rsvr_strs(rsvrs);
        }
        if (!rows.empty())
        {
            std::ofstream ofs(out_file_path.string(), std::ios::binary | std::ios::app);
            for (auto r : rows)
            {
                ofs << r;
            }
            ofs.close();
        }
        std::cout << "btm: " << btm_str << ", etm: " << etm_str << ", size:" << rows.size() << std::endl;
        all_index += rows.size();
    }

    SLOG_INFO("{} 导出完成, 导出 {} 条数据", out_file_path.string(), std::to_string(all_index));

    return true;
}

std::vector<silly_pptn> query_pptn(const std::string& begtm, const std::string& endtm)
{
    std::vector<silly_pptn> pptns;
    std::string sql = silly_format::format(select_pptn_sql, begtm, endtm);

    if (!otl.select(sql, [&pptns](otl_stream* stream) {
            // otl_write_row(*stream, btm_str, etm_str);  // 传入参数
            while (!stream->eof())
            {
                otl_value<std::string> STCD;
                otl_value<double> DRP, INTV;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, DRP, INTV);
                silly_pptn tmp_pptn;

                if (!STCD.is_null())
                {
                    tmp_pptn.stcd = STCD.v;
                }
                if (!DRP.is_null())
                {
                    tmp_pptn.drp = DRP.v;
                }
                if (!INTV.is_null())
                {
                    tmp_pptn.intv = INTV.v;
                }

                std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
                std::time_t stamp = std::mktime(&t);
                tmp_pptn.stamp = stamp;

                pptns.push_back(tmp_pptn);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        pptns.clear();
    }
    SLOG_INFO("查询到:{} 条 PPTN 数据", pptns.size());
    return pptns;
}
std::vector<std::string> pptn_strs(std::vector<silly_pptn>& pptns, const std::unordered_map<std::string, uint32_t>& stcd_index)
{
    std::vector<std::string> rows;
    for (auto& pptn : pptns)
    {
        auto it = stcd_index.find(pptn.stcd);

        if (it == std::end(stcd_index))
        {
            pptn.index = 0;
            continue;
        }
        pptn.index = it->second;
    }
    for (auto& pptn : pptns)
    {
        rows.push_back(pptn.serialize());
    }
    return rows;
}

std::vector<silly_river> query_river(const std::string& begtm, const std::string& endtm)
{
    std::vector<silly_river> rivers;
    std::string sql = silly_format::format(select_river_sql, begtm, endtm);
    if (!otl.select(sql, [&rivers](otl_stream* stream) {
            while (!stream->eof())
            {
                otl_value<std::string> STCD, WPTN;
                otl_value<double> Z, Q;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, Z, Q, WPTN);
                silly_river tmp_river;

                if (!STCD.is_null())
                {
                    tmp_river.stcd = STCD.v;
                }
                if (!Z.is_null())
                {
                    tmp_river.zz = Z.v;
                }
                if (!Q.is_null())
                {
                    tmp_river.qq = Q.v;
                }
                if (!WPTN.is_null())
                {
                    tmp_river.wptn = WPTN.v;
                }

                std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
                std::time_t stamp = std::mktime(&t);
                tmp_river.stamp = stamp;

                rivers.push_back(tmp_river);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        rivers.clear();  // 查询失败,返回空
    }

    SLOG_INFO("查询到:{} 条 RIVER 数据", rivers.size());
    return rivers;
}
std::vector<std::string> river_strs(std::vector<silly_river>& rivers)
{
    std::vector<std::string> rows;
    for (auto& river : rivers)
    {
        if (stcd_index.find(river.stcd) == std::end(stcd_index))
        {
            river.index = 0;
            continue;
        }
        river.index = stcd_index[river.stcd];
    }
    for (auto& river : rivers)
    {
        rows.push_back(river.serialize(SILLY_RIVER_FORMAT_V2));
    }
    return rows;
}

// 查询 rsvr 表中从 begtm 到 endtm 的数据
std::vector<silly_rsvr> query_rsvr(const std::string& begtm, const std::string& endtm)
{
    std::vector<silly_rsvr> rsvrs;

    std::string sql = silly_format::format(select_rsvr_sql, begtm, endtm);
    if (!otl.select(sql, [&rsvrs](otl_stream* stream) {
            while (!stream->eof())
            {
                silly_rsvr tmp_rsvr;

                otl_value<std::string> STCD, RWCHRCD, RWPTN, MSQMT;
                otl_value<double> RZ, INQ, W, OTQ, INQDR, BLRZ;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, RZ, INQ, W, OTQ, RWCHRCD, RWPTN, INQDR, MSQMT, BLRZ);
                if (!STCD.is_null())
                {
                    tmp_rsvr.stcd = STCD.v;
                }
                if (!RZ.is_null())
                {
                    tmp_rsvr.rz = RZ.v;
                }
                if (!INQ.is_null())
                {
                    tmp_rsvr.inq = INQ.v;
                }
                if (!W.is_null())
                {
                    tmp_rsvr.w = W.v;
                }
                if (!OTQ.is_null())
                {
                    tmp_rsvr.otq = OTQ.v;
                }
                if (!RWCHRCD.is_null())
                {
                    tmp_rsvr.rwchrcd = RWCHRCD.v;
                }
                if (!RWPTN.is_null())
                {
                    tmp_rsvr.rwptn = RWPTN.v;
                }
                if (!INQDR.is_null())
                {
                    tmp_rsvr.inqdr = INQDR.v;
                }
                if (!MSQMT.is_null())
                {
                    tmp_rsvr.msqmt = MSQMT.v;
                }
                if (!BLRZ.is_null())
                {
                    tmp_rsvr.blrz = BLRZ.v;
                }

                std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
                std::time_t stamp = std::mktime(&t);
                tmp_rsvr.stamp = stamp;

                rsvrs.push_back(tmp_rsvr);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        rsvrs.clear();  // 查询失败,返回空
    }

    SLOG_INFO("查询到:{} 条 RIVER 数据", rsvrs.size());
    return rsvrs;
}
// 将 rsvr 表中的数据导出到二进制流中
std::vector<std::string> rsvr_strs(std::vector<silly_rsvr>& rsvrs)
{
    std::vector<std::string> rows;
    for (auto& river : rsvrs)
    {
        if (stcd_index.find(river.stcd) == std::end(stcd_index))
        {
            river.index = 0;
            continue;
        }
        river.index = stcd_index[river.stcd];
    }
    for (auto& river : rsvrs)
    {
        rows.push_back(river.serialize(SILLY_RSVR_FORMAT_V1));
    }

    return rows;
}

bool export_rsvr()
{
    bool status = true;

    // std::filesystem::path rsvr_file_path(root);
    // std::string fileName = str_now_tm + "_" + silly_rsvr::FILE_NAME;
    // rsvr_file_path.append(fileName);
    // std::filesystem::create_directories(rsvr_file_path.parent_path());

    // silly_posix_time startTime = silly_posix_time::time_from_string(btm);
    // silly_posix_time endTime = silly_posix_time::time_from_string(etm);
    // silly_time_duration minutes(5 * 24, 0, 0);  // 10天
    // SLOG_INFO("查询 rsvr 截止时间: {}, etm: {}", btm, etm);
    // size_t all_index = 0;
    // for (silly_posix_time currentTm = startTime; currentTm < endTime; currentTm += minutes)
    //{
    //     std::string btm_str = currentTm.to_string();
    //     silly_posix_time temp = currentTm;
    //     temp += minutes;
    //     if (temp > endTime)
    //     {
    //         temp = endTime;
    //     }
    //     std::string etm_str = temp.to_string();
    //     std::vector<silly_rsvr> rsvrs;
    //     // std::string sql = silly_format::format(select_pptn_sql, btm_str, etm_str);
    //     if (!otl.select(select_rsvr_sql, [&rsvrs, btm_str, etm_str](otl_stream* stream) {
    //             otl_write_row(*stream, btm_str, etm_str);  // 传入参数
    //             while (!stream->eof())
    //             {
    //                 otl_value<std::string> STCD, WPTN;
    //                 otl_value<double> Z, Q;
    //                 otl_datetime tm;
    //                 otl_read_row(*stream, STCD, tm, Z, Q, WPTN);
    //                 silly_rsvr tmp_rsvr;

    //                if (!STCD.is_null())
    //                {
    //                    tmp_rsvr.stcd = STCD.v;
    //                }
    //                if (!Z.is_null())
    //                {
    //                    tmp_rsvr.zz = Z.v;
    //                }
    //                if (!Q.is_null())
    //                {
    //                    tmp_rsvr.qq = Q.v;
    //                }
    //                if (!WPTN.is_null())
    //                {
    //                    tmp_rsvr.wptn = WPTN.v;
    //                }

    //                std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
    //                std::time_t stamp = std::mktime(&t);
    //                tmp_rsvr.stamp = stamp;

    //                rsvrs.push_back(tmp_rsvr);
    //            }
    //        }))
    //    {
    //        SLOG_ERROR(otl.err());
    //        return false;
    //    }
    //    for (auto& rsvr : rsvrs)
    //    {
    //        if (stcd_index.find(rsvr.stcd) == std::end(stcd_index))
    //        {
    //            rsvr.index = 0;
    //            continue;
    //        }
    //        rsvr.index = stcd_index[rsvr.stcd];
    //    }

    //    std::ofstream ofs(rsvr_file_path.string(), std::ios::binary | std::ios::app);
    //    for (auto& rsvr : rsvrs)
    //    {
    //        ofs << rsvr.serialize(SILLY_RIVER_FORMAT_V2);
    //    }
    //    ofs.close();
    //    std::cout << "btm: " << btm_str << ", etm: " << etm_str << ", size:" << rsvrs.size() << std::endl;
    //    all_index += rsvrs.size();
    //}

    // SLOG_INFO("{} 导出完成, 导出 {} 条数据", rsvr_file_path.string(), std::to_string(all_index));

    return status;
}
