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

// 全局变量
silly_otl otl;
std::string stbprp_select_sql;
std::string pptn_select_sql;
std::string btm;
std::string etm;
std::string fpath = "./";  // 文件根路径
std::string src_encode;
std::string dst_encode;
std::map<std::string, uint32_t> stcd_index;


bool init(const std::string& file);


// 导出测站基本信息
bool export_stbprp(const std::string& str_now_tm);

// 导出降雨记录
bool export_pptn(const std::string& btm, const std::string& etm, const std::string& str_now_tm);


int main(int argc, char** argv)
{

#ifndef NDEBUG
    std::string configPath = "../../../../tools/RWDB/export.json";
#else
    std::string configPath = "./config/export.json";
#endif

    if (!init(configPath))
    {
        return -1;
    }

#if IS_WIN32
    if (otl.type() == enum_database_type::dbORACLE)
    {
        _putenv_s("NLS_LANG", "SIMPLIFIED CHINESE_CHINA.UTF8");
    }
#else
#endif
    otl.help();
    std::string str_now_tm = silly_posix_time::now().to_string("%Y%m%d%H%M%S");
 
    // 导入导入stbprp
    export_stbprp(str_now_tm);

    // 导入导入pptn
    export_pptn(btm, etm, str_now_tm);

    //export_rsvr(btm, etm);
    //export_river(btm, etm);

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
    otl = otl_tools::conn_opt_from_json(jv_root["src_db"]);
    otl.dump_odbc();
    pptn_select_sql = jv_root["sql"]["pptn_select_sql"].asString();
    stbprp_select_sql = jv_root["sql"]["stbprp_select_sql"].asString();

    btm = jv_root["btm"].asString();
    etm = jv_root["etm"].asString();

    src_encode = jv_root["encode"]["src"].asString();
    dst_encode = jv_root["encode"]["dst"].asString();

    status = true;
    return status;
}



bool export_stbprp(const std::string& str_now_tm)
{
    std::vector<silly_stbprp> stbprps;
    // ---------查询数据库-----------
    if (!otl.select(stbprp_select_sql, [&stbprps](otl_stream* stream) {
            while (!stream->eof())
            {
                otl_value<std::string> STCD, STNM, RVNM, HNNM, BSNM, STLC, ADDVCD, DTMNM, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, PHCD, USFL, COMMENTS, HNNM0, ADCD, ADDVCD1;
                otl_value<double> LGTD, LTTD, DTMEL, DTPR, DSTRVM;
                otl_value<int> STAzt, DRNA;
                otl_datetime MODITIME;

                otl_read_row(*stream, STCD, STNM, RVNM, HNNM, BSNM, LGTD, LTTD, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1);

                silly_stbprp temp;
                temp.STCD = STCD.v;
                temp.STNM = STNM.v;
                temp.RVNM = RVNM.v;
                temp.HNNM = HNNM.v;
                temp.BSNM = BSNM.v;
                temp.LGTD = LGTD.v;
                temp.LTTD = LTTD.v;
                temp.STLC = STLC.v;
                temp.ADDVCD = ADDVCD.v;
                temp.DTMNM = DTMNM.v;
                temp.DTMEL = DTMEL.v;
                temp.DTPR = DTPR.v;
                temp.STTP = STTP.v;
                temp.FRGRD = FRGRD.v;
                temp.ESSTYM = ESSTYM.v;
                temp.BGFRYM = BGFRYM.v;
                temp.ATCUNIT = ATCUNIT.v;
                temp.ADMAUTH = ADMAUTH.v;
                temp.LOCALITY = LOCALITY.v;
                temp.STBK = STBK.v;
                temp.STAzt = STAzt.v;
                temp.DSTRVM = DSTRVM.v;
                temp.DRNA = DRNA.v;
                temp.PHCD = PHCD.v;
                temp.USFL = USFL.v;
                temp.COMMENTS = COMMENTS.v;
                temp.MODITIME = otl_tools::otl_time_to_string(MODITIME);
                temp.HNNM0 = HNNM0.v;
                temp.ADCD = ADCD.v;
                temp.ADDVCD1 = ADDVCD1.v;

                stbprps.push_back(temp);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    // ------------添加index------------
    uint32_t index = 0;
    for (auto& stbprp : stbprps)
    {
        stbprp.index = index;
        stcd_index[stbprp.STCD] = index;
        index++;
    }

    // -----------转编码--------------
    if (!src_encode.empty() && !dst_encode.empty())
    {
        for (auto& stbprp : stbprps)
        {
            encode(stbprp, src_encode, dst_encode);
        }
    }

    // -----------导出stbprp文件------------
    std::filesystem::path stbprp_file_path(fpath);
    if (!std::filesystem::exists(stbprp_file_path.parent_path()))
    {
        std::filesystem::create_directories(stbprp_file_path.parent_path());
    }
    std::string fileName = str_now_tm + "_" + silly_stbprp::FILE_NAME;
    stbprp_file_path.append(fileName);

    std::ofstream ofs(stbprp_file_path.string(), std::ios::binary);
    for (auto& stbprp : stbprps)
    {
        ofs << stbprp.serialize();
    }
    ofs.close();

    SLOG_INFO("{} 导出完成", stbprp_file_path.string());

    return true;
}


bool export_pptn(const std::string& btm, const std::string& etm, const std::string& str_now_tm)
{
    std::vector<silly_pptn> pptns;

    if (!otl.select(pptn_select_sql, [&pptns, &btm, &etm](otl_stream* stream) {
            otl_write_row(*stream, btm.c_str(), etm.c_str());  // 传入参数
            uint32_t index = 0;
            while (!stream->eof())
            {
                std::string stcd;
                double drp, intv;
                otl_datetime tm;
                otl_read_row(*stream, stcd, tm, intv, drp);
                std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
                std::time_t stamp = std::mktime(&t);
                silly_pptn pptn(stcd, stamp, intv, drp);
                pptn.index = index;
                stcd_index[stcd] = pptn.index;
                pptns.push_back(pptn);

                index++;
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    std::filesystem::path ppth_file_path(fpath);
    if (!std::filesystem::exists(ppth_file_path.parent_path()))
    {
        std::filesystem::create_directories(ppth_file_path.parent_path());
    }
    std::string fileName = str_now_tm + "_" + silly_pptn::FILE_NAME;
    ppth_file_path.append(fileName);

    std::ofstream ofs(ppth_file_path.string(), std::ios::binary);
    for (auto& pptn : pptns)
    {
        ofs << pptn.serialize();
    }
    ofs.close();
    SLOG_INFO("{} 导出完成", ppth_file_path.string());

    return true;
}



