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

// 全局变量
std::map<std::string, uint32_t> stcd_index;

class silly_export_pptn
{
  public:
    // 初始化
    bool init(const std::string& file);

    // 导出降雨记录
    bool export_pptn(const std::string& btm, const std::string& etm, const std::string& str_now_tm);

    //// 导出水库水情记录
    //void export_rsvr(const std::string& btm, const std::string& etm);
    //// 导出河道水情记录
    //void export_river(const std::string& btm, const std::string& etm);

    std::string src_odbc;
    std::string ppnt_select_sql;
    std::string insert_pptn_sql;
    std::string fpath ="./";  // 文件根路径
    std::string btm;
    std::string etm;

    silly_otl otl;
};



class silly_export_stbprp
{
  public:
    // 初始化
    bool init(const std::string& file);

    // 导出测站基本信息
    bool export_stbprp(const std::string& str_now_tm);
    std::string src_odbc;
    std::string stbprp_select_sql;
    std::string insert_stbprp_sql;
    std::string fpath = "./";  // 文件根路径

    silly_otl otl;
};




// 记录stcd对应的索引
void serialize_stcd_index(const std::string& filepath, const std::map<std::string, uint32_t>& stcd_index)
{
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file for writing.");
    }

    for (const auto& [stcd, index] : stcd_index)
    {
        file << stcd << " " << index << std::endl;
    }

    file.close();
}

// 读取stcd对应的索引
std::map<std::string, uint32_t> deserialize_stcd_index(const std::string& filepath)
{
    std::map<std::string, uint32_t> des_stcd_index;
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file for reading.");
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key;
        uint32_t value;
        if (!(iss >> key >> value))
        {
            continue;
        }
        des_stcd_index[key] = value;
    }

    file.close();
    return des_stcd_index;
}



int main(int argc, char** argv)
{
    _putenv_s("NLS_LANG", "SIMPLIFIED CHINESE_CHINA.UTF8");
    
    silly_export_pptn exporter_pptn;
    exporter_pptn.init("../../../../tools/RWDB/export.json");
    exporter_pptn.otl.load(exporter_pptn.src_odbc);

    silly_export_stbprp exporter_stbprp;
    exporter_stbprp.init("../../../../tools/RWDB/export.json");
    exporter_stbprp.otl.load(exporter_stbprp.src_odbc);
    silly_posix_time now_tm = silly_posix_time::now();
    std::string str_now_tm = silly_posix_time::time_to_string(now_tm, "%Y%m%d%H%M%S");
 
    // 导入导入stbprp
    exporter_stbprp.export_stbprp(str_now_tm);

    // 导入导入pptn
    exporter_pptn.export_pptn(exporter_pptn.btm, exporter_pptn.etm, str_now_tm);

    //export_rsvr(btm, etm);
    //export_river(btm, etm);

    return 0;
}



bool silly_export_pptn::init(const std::string& file)
{
    bool status = false;
    Json::Value jv_root = silly_jsonpp::loadf(file);
    if (jv_root.isNull())
    {
        SFP_ERROR("配置文件读取失败: {}", file);
        return status;
    }
    otl_conn_opt src_opt = otl_tools::conn_opt_from_json(jv_root["src_db"]);
    src_odbc = src_opt.dump_odbc();
    ppnt_select_sql = jv_root["sql"]["ppnt_select_sql"].asString();
    fpath = jv_root["path"].asString();
    btm = jv_root["btm"].asString();
    etm = jv_root["etm"].asString();

    status = true;
    return status;
}


bool silly_export_stbprp::init(const std::string& file)
{
    bool status = false;
    Json::Value jv_root = silly_jsonpp::loadf(file);
    if (jv_root.isNull())
    {
        SFP_ERROR("配置文件读取失败: {}", file);
        return status;
    }
    otl_conn_opt src_opt = otl_tools::conn_opt_from_json(jv_root["src_db"]);
    src_odbc = src_opt.dump_odbc();
    stbprp_select_sql = jv_root["sql"]["stbprp_select_sql"].asString();
    insert_stbprp_sql = jv_root["sql"]["insert_stbprp_sql"].asString();
    fpath = jv_root["path"].asString();

    status = true;
    return status;
}

bool silly_export_stbprp::export_stbprp(const std::string& str_now_tm)
{
    std::vector<silly_stbprp> stbprps;

    if (!otl.select(stbprp_select_sql, [&stbprps](otl_stream* stream) {
            uint32_t index = 0;
            while (!stream->eof())
            {
                otl_value<std::string> STCD, STNM, RVNM, HNNM, BSNM, STLC, ADDVCD, DTMNM, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, PHCD, USFL, COMMENTS, HNNM0, ADCD, ADDVCD1;
                otl_value<double> LGTD, LTTD, DTMEL, DTPR, DSTRVM;
                otl_value<int> STAzt, DRNA;
                otl_datetime MODITIME;

                otl_read_row(*stream, STCD, STNM, RVNM, HNNM, BSNM, LGTD, LTTD, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1);

                silly_stbprp temp;
                temp.index = index;
                temp.STCD = STCD.v;
                stcd_index[temp.STCD] = index;
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
                index++;
            }
    }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

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

    std::filesystem::path stcd_index_file_path(fpath);
    std::string fileName2 = str_now_tm + "_" + "stcd_index.txt";
    stcd_index_file_path.append(fileName2);

    serialize_stcd_index(stcd_index_file_path.string(), stcd_index);

    SLOG_INFO("{} 导出完成", silly_stbprp::FILE_NAME);

    return true;

}

bool silly_export_pptn::export_pptn(const std::string& btm, const std::string& etm, const std::string& str_now_tm)
{
    std::vector<silly_pptn> pptns;

    if (!otl.select(ppnt_select_sql, [&pptns, &btm, &etm](otl_stream* stream) {
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
    SLOG_INFO("{} 导出完成", silly_pptn::FILE_NAME);


    return true;

}
void export_rsvr()
{
    SLOG_ERROR("未实现")
    exit(-1);
}
void export_river()
{
    SLOG_ERROR("未实现")
    exit(-1);
}