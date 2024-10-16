
#include <database/otl/silly_otl.h>
#include <database/otl/otl_tools.h>
#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_stbprp.h>
#include "export_tool.h"

// 全局变量
struct encode
{
    std::string src;
    std::string dst;
};
silly_otl src_otl;
silly_otl des_otl;
std::string src_select_pptn_sql;
std::string src_select_stbprp_sql;
std::string des_select_pptn_sql;
std::string des_select_stbprp_sql;
struct encode src_encode;
struct encode des_encode;
std::string btm;
std::string etm;

//std::string src_s_encode;
//std::string stc_d_encode;
//std::string des_s_encode;
//std::string dst_d_encode;

// 结构体用于存储不一致的数据
struct ComparisonResult
{
    std::string STCD;
    std::string src_value;
    std::string des_value;
};


bool init(const std::string& file);

bool select_stbprp(silly_otl otl, const std::string select_stbprp_sql, struct encode cvt, std::vector<silly_stbprp>& stbprps);

bool select_pptn(silly_otl otl, const std::string select_pptn_sql, std::vector<silly_pptn>& pptns);

std::map<std::string, std::string> compareStbprps(const std::vector<silly_stbprp>& src_stbprps, const std::vector<silly_stbprp>& des_stbprps);
std::map<std::string, std::string> comparePptn(const std::vector<silly_pptn>& src_pptns, const std::vector<silly_pptn>& des_pptns);

    int main(int argc, char* argv[])
{
#ifndef NDEBUG

    std::string configPath = std::filesystem::path(DEFAULT_SU_ROOT_DIR).append("tools").append("RWDB").append("compare_db.json").string();
#else
    std::string configPath = "./compare_db.json";
#endif

    if (!init(configPath))
    {
        return -1;
    }


#if IS_WIN32
    if (src_otl.type() == enum_database_type::dbORACLE || des_otl.type() == enum_database_type::dbORACLE)
    {
        _putenv_s("NLS_LANG", "SIMPLIFIED CHINESE_CHINA.UTF8");
    }
#else
#endif


    std::vector<silly_stbprp> src_stbprps;
    if (!select_stbprp(src_otl,src_select_stbprp_sql, src_encode, src_stbprps))
    {
        SFP_ERROR("src select_stbprp failed");

        return -1;
    }
    std::vector<silly_stbprp> des_stbprps;
    if (!select_stbprp(des_otl,des_select_stbprp_sql, des_encode, des_stbprps))
    {
        SFP_ERROR("des select_stbprp failed");
        return -1;
    }

    std::vector<silly_pptn> src_pptns;
    if (!select_pptn(src_otl,src_select_pptn_sql, src_pptns))
    {
        SFP_ERROR("src select_pptn failed");
        return -1;
    }

    std::vector<silly_pptn> des_pptns;
    if (!select_pptn(des_otl, des_select_pptn_sql, des_pptns))
    {
        SFP_ERROR("des select_pptn failed");
        return -1;
    }

    // 对比 
    std::map<std::string, std::string> stbprp_stcd_dif = compareStbprps(src_stbprps, des_stbprps);

    for (const auto& [stcd, dif] : stbprp_stcd_dif)
    {
        std::cout << stcd << " " << dif << std::endl;
    }

    std::map<std::string, std::string> pptn_stcd_dif = comparePptn(src_pptns, des_pptns);
    for (const auto& [stcd, dif] : pptn_stcd_dif)
    {
        std::cout << stcd << " " << dif << std::endl;
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
    src_otl = otl_tools::conn_opt_from_json(jv_root["src_db"]);
    src_otl.dump_odbc();
    des_otl = otl_tools::conn_opt_from_json(jv_root["des_db"]);
    des_otl.dump_odbc();

    src_select_pptn_sql = jv_root["sql"]["src_select_pptn_sql"].asString();
    src_select_stbprp_sql = jv_root["sql"]["src_select_stbprp_sql"].asString();

    des_select_pptn_sql = jv_root["sql"]["des_select_pptn_sql"].asString();
    des_select_stbprp_sql = jv_root["sql"]["des_select_stbprp_sql"].asString();

    btm = jv_root["btm"].asString();
    etm = jv_root["etm"].asString();

    src_encode.src = jv_root["src_encode"]["src"].asString();
    src_encode.dst = jv_root["src_encode"]["dst"].asString();

    des_encode.src = jv_root["des_encode"]["src"].asString();
    des_encode.dst = jv_root["des_encode"]["dst"].asString();

    return true;
}



bool select_stbprp(silly_otl otl, const std::string select_stbprp_sql, struct encode cvt, std::vector<silly_stbprp>& stbprps)
{

    if (!otl.select(select_stbprp_sql, [&stbprps](otl_stream* stream) {
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

    if (!cvt.src.empty() && !cvt.dst.empty())
    {
        for (auto& stbprp : stbprps)
        {
            encode(stbprp, cvt.src, cvt.dst);
        }
    }
    return true;
}


bool select_pptn(silly_otl otl, const std::string select_pptn_sql, std::vector<silly_pptn>& pptns)
{

    if (!otl.select(select_pptn_sql, [&pptns](otl_stream* stream) {
            otl_write_row(*stream, btm, etm);  // 传入参数
            while (!stream->eof())
            {
                std::string stcd;
                double drp, intv;
                otl_datetime tm;
                otl_read_row(*stream, stcd, tm, intv, drp);
                std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
                std::time_t stamp = std::mktime(&t);
                silly_pptn pptn(stcd, stamp, intv, drp);
                pptns.push_back(pptn);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    return true;
}





// 比较两个 std::vector<silly_stbprp> 的函数
std::map<std::string, std::string> compareStbprps(const std::vector<silly_stbprp>& src_stbprps, const std::vector<silly_stbprp>& des_stbprps)
{
    std::map<std::string, std::string> discrepancies;

    // 首先检查向量大小是否相同
    if (src_stbprps.size() != des_stbprps.size())
    {
        std::cout << "数据大小不一致\n";
        return discrepancies;  // 返回空向量
    }

    // 逐个比较元素
    for (size_t i = 0; i < src_stbprps.size(); ++i)
    {
        const auto& src = src_stbprps[i];
        const auto& des = des_stbprps[i];

        // 比较每个成员
        if (src.STCD != des.STCD)
        {
            std::string dif = "(" + src.STCD + "/" + des.STCD + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.STNM != des.STNM)
        {
            std::string dif = "(" + src.STNM + "/" + des.STNM + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.RVNM != des.RVNM)
        {
            std::string dif = "(" + src.RVNM + "/" + des.RVNM + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.HNNM != des.HNNM)
        {
            std::string dif = "(" + src.HNNM + "/" + des.HNNM + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.BSNM != des.BSNM)
        {
            std::string dif = "(" + src.BSNM + "/" + des.BSNM + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (std::abs(src.LGTD - des.LGTD)>0.00001)
        {
            std::string dif = "(" + std::to_string(src.LGTD) + "/" + std::to_string(des.LGTD) + ")\
\t";
            discrepancies[src.STCD] += dif;
        }
        if (std::abs(src.LTTD - des.LTTD) > 0.00001)
        {
            std::string dif = "(" + std::to_string(src.LTTD) + "/" + std::to_string(des.LTTD) + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.STLC != des.STLC)
        {
            std::string dif = "(" + src.STLC + "/" + des.STLC + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.ADDVCD != des.ADDVCD)
        {
            std::string dif = "(" + src.ADDVCD + "/" + des.ADDVCD + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.DTMNM != des.DTMNM)
        {
            std::string dif = "(" + src.DTMNM + "/" + des.DTMNM + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (std::abs(src.DTMEL - des.DTMEL) > 0.00001)
        {
            std::string dif = "(" + std::to_string(src.DTMEL) + "/" + std::to_string(des.DTMEL) + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (std::abs(src.DTPR - des.DTPR) > 0.00001)
        {
            std::string dif = "(" + std::to_string(src.DTPR) + "/" + std::to_string(des.DTPR) + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.STTP != des.STTP)
        {
            std::string dif = "(" + src.STTP + "/" + des.STTP + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.FRGRD != des.FRGRD)
        {
            std::string dif = "(" + src.FRGRD + "/" + des.FRGRD + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.ESSTYM != des.ESSTYM)
        {
            std::string dif = "(" + src.ESSTYM + "/" + des.ESSTYM + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.BGFRYM != des.BGFRYM)
        {
            std::string dif = "(" + src.BGFRYM + "/" + des.BGFRYM + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.ATCUNIT != des.ATCUNIT)
        {
            std::string dif = "(" + src.ATCUNIT + "/" + des.ATCUNIT + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.ADMAUTH != des.ADMAUTH)
        {
            std::string dif = "(" + src.ADMAUTH + "/" + des.ADMAUTH + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.LOCALITY != des.LOCALITY)
        {
            std::string dif = "(" + src.LOCALITY + "/" + des.LOCALITY + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.STBK != des.STBK)
        {
            std::string dif = "(" + src.STBK + "/" + des.STBK + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.STAzt != des.STAzt)
        {
            std::string dif = "(" + std::to_string(src.STAzt) + "/" + std::to_string(des.STAzt) + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (std::abs(src.DSTRVM - des.DSTRVM) > 0.00001)
        {
            std::string dif = "(" + std::to_string(src.DSTRVM) + "/" + std::to_string(des.DSTRVM) + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.DRNA != des.DRNA)
        {
            std::string dif = "(" + std::to_string(src.DRNA) + "/" + std::to_string(des.DRNA) + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.PHCD != des.PHCD)
        {
            std::string dif = "(" + src.PHCD + "/" + des.PHCD + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.USFL != des.USFL)
        {
            std::string dif = "(" + src.USFL + "/" + des.USFL + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.COMMENTS != des.COMMENTS)
        {
            std::string dif = "(" + src.COMMENTS + "/" + des.COMMENTS + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.MODITIME != des.MODITIME)
        {
            std::string dif = "(" + src.MODITIME + "/" + des.MODITIME + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.HNNM0 != des.HNNM0)
        {
            std::string dif = "(" + src.HNNM0 + "/" + des.HNNM0 + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.ADCD != des.ADCD)
        {
            std::string dif = "(" + src.ADCD + "/" + des.ADCD + ")\t";
            discrepancies[src.STCD] += dif;
        }
        if (src.ADDVCD1 != des.ADDVCD1)
        {
            std::string dif = "(" + src.ADDVCD1 + "/" + des.ADDVCD1 + ")\t";
            discrepancies[src.STCD] += dif;
        }
    }

    return discrepancies;  // 返回不一致的结果
}

std::map<std::string, std::string> comparePptn(const std::vector<silly_pptn>& src_pptns, const std::vector<silly_pptn>& des_pptns)
{
    std::map<std::string, std::string> discrepancies;
    //float drp{0};
    //std::string stcd;
    //uint32_t index{0};
    //std::time_t stamp{0};
    //float intv{0};
    for (size_t i = 0; i < src_pptns.size(); ++i)
    {
        const auto& src = src_pptns[i];
        const auto& des = des_pptns[i];

        if (src.stcd != des.stcd)
        {
            std::string dif = "(" + src.stcd + "/" + des.stcd + ")\t";
            discrepancies[src.stcd] += dif;
        }
        if (src.drp != des.drp)
        {
            std::string dif = "(" + std::to_string(src.drp) + "/" + std::to_string(des.drp) + ")\t";
            discrepancies[src.stcd] += dif;
        }
        if (src.stamp != des.stamp)
        {
            std::string dif = "(" + std::to_string(src.stamp) + "/" + std::to_string(des.stamp) + ")\t";
            discrepancies[src.stcd] += dif;
        }
        if (src.intv != des.intv)
        {
            std::string dif = "(" + std::to_string(src.intv) + "/" + std::to_string(des.intv) + ")\t";
            discrepancies[src.stcd] += dif;
        }

    }
    return discrepancies;
}















