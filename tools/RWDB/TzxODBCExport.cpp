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

// 全局变量
struct encode
{
    std::string src;
    std::string dst;
};
silly_otl otl;
std::string select_stbprp_sql;
std::string select_pptn_sql;
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
bool export_river();
// TODO: 导出水库信息  rsvr 分块导入 pptn分块导入导出 三个的分块导入导出

int main(int argc, char** argv)
{
#ifndef NDEBUG

    std::string configPath = std::filesystem::path(DEFAULT_SU_ROOT_DIR).append("tools").append("RWDB").append("export.json").string();
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

    // 导出stbprp
    if (!export_stbprp())
    {
        SLOG_ERROR("export_stbprp failed");
        return -1;
    }

    // 导出pptn
    // export_pptn();

    if (!export_river())
    {
        SLOG_ERROR("export_river failed");
        return -1;
    }

    // export_river(btm, etm);
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

                otl_read_row(*stream, STCD, STNM/*, RVNM, HNNM, BSNM*/, LGTD, LTTD/*, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1*/);
                silly_stbprp temp;

                // 检查并赋值
                if (!STCD.is_null()) { temp.STCD = STCD.v; }
                if (!STNM.is_null()) { temp.STNM = STNM.v; }
               /* if (!RVNM.is_null()) { temp.RVNM = RVNM.v; }
                if (!HNNM.is_null()) { temp.HNNM = HNNM.v; }
                if (!BSNM.is_null()) { temp.BSNM = BSNM.v; }*/
                if (!LGTD.is_null()) { temp.LGTD = LGTD.v; }
                if (!LTTD.is_null()) { temp.LTTD = LTTD.v; }
                /* if (!STLC.is_null())
                {
                    temp.STLC = STLC.v;
                }
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

    SLOG_INFO("{} 导出完成", stbprp_file_path.string());

    return true;
}

bool export_pptn()
{
    std::vector<silly_pptn> pptns;

    if (!otl.select(select_pptn_sql, [&pptns](otl_stream* stream) {
            otl_write_row(*stream, btm, etm);  // 传入参数
            while (!stream->eof())
            {
                otl_value<std::string> STCD;
                otl_value<double> DRP, INTV;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, DRP, INTV);
                silly_pptn tmp_pptn;

                if (!STCD.is_null()){tmp_pptn.stcd = STCD.v;}
                if (!DRP.is_null()){tmp_pptn.drp = DRP.v;}
                if (!INTV.is_null()){tmp_pptn.intv = INTV.v;}

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

    for(auto& pptn : pptns)
    {
        if(stcd_index.find(pptn.stcd) == std::end(stcd_index))
        {
            pptn.index = 0;
            continue;
        }
        pptn.index = stcd_index[pptn.stcd];
    }

    std::filesystem::path ppth_file_path(root);
    std::string fileName = str_now_tm + "_" + silly_pptn::FILE_NAME;
    ppth_file_path.append(fileName);
    std::filesystem::create_directories(ppth_file_path.parent_path());

    std::ofstream ofs(ppth_file_path.string(), std::ios::binary);
    for (auto& pptn : pptns)
    {
        ofs << pptn.serialize();
    }
    ofs.close();
    SLOG_INFO("{} 导出完成", ppth_file_path.string());

    return true;
}


bool export_river()
{
    std::filesystem::path ppth_file_path(root);
    std::string fileName = str_now_tm + "_" + silly_river::FILE_NAME;
    ppth_file_path.append(fileName);
    std::filesystem::create_directories(ppth_file_path.parent_path());

    silly_posix_time startTime = silly_posix_time::time_from_string(btm);
    silly_posix_time endTime = silly_posix_time::time_from_string(etm);
    silly_time_duration minutes(1 * 24, 0, 0); // 10天

    for (silly_posix_time currentTm = startTime; currentTm < endTime; currentTm += minutes)
    {
        std::string btm_str = currentTm.to_string();
        silly_posix_time temp = currentTm;
        temp+= minutes;
        std::string etm_str = temp.to_string();
        std::vector<silly_river> rivers;
        if (!otl.select(select_pptn_sql, [&rivers, btm_str, etm_str](otl_stream* stream) {
                otl_write_row(*stream, btm_str, etm_str);  // 传入参数
                while (!stream->eof())
                {
                    otl_value<std::string> STCD;
                    otl_value<double> DRP, INTV;
                    otl_datetime tm;
                    otl_read_row(*stream, STCD, tm, DRP, INTV);
                    silly_river tmp_river;

                    if (!STCD.is_null())
                    {
                        tmp_river.stcd = STCD.v;
                    }
                    if (!DRP.is_null())
                    {
                        tmp_river.zz = DRP.v;
                    }
                    if (!INTV.is_null())
                    {
                        tmp_river.qq = INTV.v;
                    }

                    std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
                    std::time_t stamp = std::mktime(&t);
                    tmp_river.stamp = stamp;

                    rivers.push_back(tmp_river);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return false;
        }

        for (auto& river : rivers)
        {
            if (stcd_index.find(river.stcd) == std::end(stcd_index))
            {
                river.index = 0;
                continue;
            }
            river.index = stcd_index[river.stcd];
        }

        std::ofstream ofs(ppth_file_path.string(), std::ios::binary | std::ios::app);
        for (auto& river : rivers)
        {
            ofs << river.serialize();
        }
        ofs.close();
        SLOG_INFO("btm: {}, etm: {}, size:{}", btm_str, etm_str, rivers.size());

    }

    SLOG_INFO("{} 导出完成", ppth_file_path.string());

    return true;
}
