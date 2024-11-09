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
#include "datetime/silly_timer.h" // 计时
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
bool export_river();
bool export_rsvr();
// TODO: 导出水库信息  rsvr 分块导出 pptn分块导出导出 三个的分块导出导出

int main(int argc, char** argv)
{

        // 初始化布尔变量，默认值为false
    bool has_pptn = false;
    bool has_river = false;
    bool has_rsvr = false;

    // 遍历命令行参数（从索引1开始，因为argv[0]是程序名）
    for (int i = 1; i < argc; ++i)
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

    // 输出结果以验证是否正确设置了变量
    std::cout << "pptn: " << has_pptn << std::endl;
    std::cout << "river: " << has_river << std::endl;
    std::cout << "rsvr: " << has_rsvr << std::endl;

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
    
    timer.restart();
    // 导出stbprp
    if (!export_stbprp())
    {
        SLOG_ERROR("export_stbprp failed");
        return -1;
    }
    SLOG_INFO("stbprp 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);

    if (has_pptn)
    {
        // // 导出pptn
         timer.restart();
         export_pptn();
         SLOG_INFO("pptn 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }

    if (has_river)
    {
        timer.restart();
        if (!export_river())
        {
            SLOG_ERROR("export_river failed");
            return -1;
        }
        SLOG_INFO("river 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
    }

    if (has_rsvr)
    {
        timer.restart();
        if (!export_rsvr())
        {
            SLOG_ERROR("export_river failed");
            return -1;
        }
        SLOG_INFO("river 导出时间:{} 秒, {} 分钟", timer.elapsed_ms() / 1000, timer.elapsed_ms() / 1000 / 60);
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
        all_index+=pptns.size();
    }
    SLOG_INFO("pptn not found stcd: {}", nstdcindex);

    SLOG_INFO("{} 导出 pptn 完成, 导出 {} 条数据", ppth_file_path.string(), std::to_string(all_index));

    return true;
}


bool export_river()
{
    std::filesystem::path river_file_path(root);
    std::string fileName = str_now_tm + "_" + silly_river::FILE_NAME;
    river_file_path.append(fileName);
    std::filesystem::create_directories(river_file_path.parent_path());

    silly_posix_time startTime = silly_posix_time::time_from_string(btm);
    silly_posix_time endTime = silly_posix_time::time_from_string(etm);
    silly_time_duration minutes(5 * 24, 0, 0); // 10天
    SLOG_INFO("查询 river 截止时间: {}, etm: {}", btm, etm);
    size_t all_index = 0;
    for (silly_posix_time currentTm = startTime; currentTm < endTime; currentTm += minutes)
    {
        std::string btm_str = currentTm.to_string();
        silly_posix_time temp = currentTm;
        temp+= minutes;
        if (temp > endTime)
        {
            temp = endTime;
        }
        std::string etm_str = temp.to_string();
        std::vector<silly_river> rivers;
        //std::string sql = silly_format::format(select_pptn_sql, btm_str, etm_str);
        if (!otl.select(select_river_sql, [&rivers, btm_str, etm_str](otl_stream* stream) {
                otl_write_row(*stream, btm_str, etm_str);  // 传入参数
                while (!stream->eof())
                {
                    otl_value<std::string> STCD, WPTN;
                    otl_value<double> Z, Q;
                    otl_datetime tm;
                    otl_read_row(*stream, STCD, tm, Z, Q, WPTN);
                    silly_river tmp_river;

                    if (!STCD.is_null()){ tmp_river.stcd = STCD.v; }
                    if (!Z.is_null()){ tmp_river.zz = Z.v; }
                    if (!Q.is_null()){ tmp_river.qq = Q.v; }
                    if (!WPTN.is_null()){ tmp_river.wptn = WPTN.v; }

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

        std::ofstream ofs(river_file_path.string(), std::ios::binary | std::ios::app);
        for (auto& river : rivers)
        {
            ofs << river.serialize(SILLY_RIVER_FORMAT_V2);
        }
        ofs.close();
        std::cout << "btm: " << btm_str << ", etm: " << etm_str << ", size:" << rivers.size() << std::endl;
        all_index+= rivers.size();
    }

    SLOG_INFO("{} 导出完成, 导出 {} 条数据", river_file_path.string(), std::to_string(all_index));

    return true;
}

bool export_rsvr()
{
    bool status = true;

    //std::filesystem::path rsvr_file_path(root);
    //std::string fileName = str_now_tm + "_" + silly_rsvr::FILE_NAME;
    //rsvr_file_path.append(fileName);
    //std::filesystem::create_directories(rsvr_file_path.parent_path());

    //silly_posix_time startTime = silly_posix_time::time_from_string(btm);
    //silly_posix_time endTime = silly_posix_time::time_from_string(etm);
    //silly_time_duration minutes(5 * 24, 0, 0);  // 10天
    //SLOG_INFO("查询 rsvr 截止时间: {}, etm: {}", btm, etm);
    //size_t all_index = 0;
    //for (silly_posix_time currentTm = startTime; currentTm < endTime; currentTm += minutes)
    //{
    //    std::string btm_str = currentTm.to_string();
    //    silly_posix_time temp = currentTm;
    //    temp += minutes;
    //    if (temp > endTime)
    //    {
    //        temp = endTime;
    //    }
    //    std::string etm_str = temp.to_string();
    //    std::vector<silly_rsvr> rsvrs;
    //    // std::string sql = silly_format::format(select_pptn_sql, btm_str, etm_str);
    //    if (!otl.select(select_rsvr_sql, [&rsvrs, btm_str, etm_str](otl_stream* stream) {
    //            otl_write_row(*stream, btm_str, etm_str);  // 传入参数
    //            while (!stream->eof())
    //            {
    //                otl_value<std::string> STCD, WPTN;
    //                otl_value<double> Z, Q;
    //                otl_datetime tm;
    //                otl_read_row(*stream, STCD, tm, Z, Q, WPTN);
    //                silly_rsvr tmp_rsvr;

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

    //SLOG_INFO("{} 导出完成, 导出 {} 条数据", rsvr_file_path.string(), std::to_string(all_index));

    return status;
}

