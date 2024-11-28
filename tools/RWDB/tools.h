
#ifndef TOOLS_H
#define TOOLS_H

#include "su_marco.h"
#include "encode/silly_encode.h"
#include "tzx/rwdb/silly_stbprp.h"
#include "silly_rwdb_stbprp.h"

struct option
{
    bool stbprp{false};
    bool pptn{false};
    bool river{false};
    bool rsvr{false};
};

// 定义全局变量 _opt
static option _opt;

// otl_datetime 转 时间戳
static std::time_t otl_to_timestamp(const otl_datetime& olt_tm)
{
    std::tm t;
    t.tm_sec = olt_tm.second;
    t.tm_min = olt_tm.minute;
    t.tm_hour = olt_tm.hour;
    t.tm_mday = olt_tm.day;
    t.tm_mon = olt_tm.month - 1;
    t.tm_year = olt_tm.year - 1900;
    std::time_t stamp = std::mktime(&t);
    return stamp;
}

// 转换时间戳为otl_datetime
static otl_datetime otl_from_timestamp(std::time_t timestamp)
{
    std::tm* t = std::localtime(&timestamp);
    otl_datetime olt_tm;
    olt_tm.year = t->tm_year + 1900;
    olt_tm.month = t->tm_mon + 1;  // tm_mon 是从 0 开始的，所以加 1
    olt_tm.day = t->tm_mday;
    olt_tm.hour = t->tm_hour;
    olt_tm.minute = t->tm_min;
    olt_tm.second = t->tm_sec;

    return olt_tm;
}

static void paramAnalysis(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp("pptn", argv[i]) == 0)
        {
            _opt.pptn = true;
            SLOG_INFO("导出 pptn");
        }
        else if (std::strcmp("river", argv[i]) == 0)
        {
            _opt.river = true;
            SLOG_INFO("导出 river");
        }
        else if (std::strcmp("rsvr", argv[i]) == 0)
        {
            _opt.rsvr = true;
            SLOG_INFO("导出 rsvr");
        }
        else if (std::strcmp("stbprp", argv[i]) == 0)
        {
            _opt.stbprp = true;
            SLOG_INFO("导出 stbprp");
        }
    }
}

// 时间分段函数,interval 单位为小时
static std::vector<std::pair<std::string, std::string>> splitTime(std::string beginTM, std::string endTM, int interval)
{
    std::vector<std::pair<std::string, std::string>> b_e_tms;

    silly_posix_time pt_btm = silly_posix_time::time_from_string(beginTM);
    silly_posix_time pt_etm = silly_posix_time::time_from_string(endTM);
    silly_posix_time t_tm = pt_btm;
    silly_time_duration td(interval, 0, 0);
    while (t_tm < pt_etm)
    {
        std::string sbt = t_tm.to_string();
        t_tm += td;
        if (t_tm >= pt_etm)
        {
            t_tm = pt_etm;
        }
        std::string set = t_tm.to_string();
        b_e_tms.push_back(std::make_pair(sbt, set));
    }
    return b_e_tms;
}

static bool encode(silly_stbprp& stbprp, const std::string& from, const std::string& to)
{
    silly_encode::iconv_convert(from, to, stbprp.STCD, stbprp.STCD);
    silly_encode::iconv_convert(from, to, stbprp.STNM, stbprp.STNM);
    silly_encode::iconv_convert(from, to, stbprp.RVNM, stbprp.RVNM);
    silly_encode::iconv_convert(from, to, stbprp.HNNM, stbprp.HNNM);
    silly_encode::iconv_convert(from, to, stbprp.BSNM, stbprp.BSNM);
    silly_encode::iconv_convert(from, to, stbprp.STLC, stbprp.STLC);
    silly_encode::iconv_convert(from, to, stbprp.ADDVCD, stbprp.ADDVCD);
    silly_encode::iconv_convert(from, to, stbprp.DTMNM, stbprp.DTMNM);
    silly_encode::iconv_convert(from, to, stbprp.STTP, stbprp.STTP);
    silly_encode::iconv_convert(from, to, stbprp.FRGRD, stbprp.FRGRD);
    silly_encode::iconv_convert(from, to, stbprp.ESSTYM, stbprp.ESSTYM);
    silly_encode::iconv_convert(from, to, stbprp.BGFRYM, stbprp.BGFRYM);
    silly_encode::iconv_convert(from, to, stbprp.ATCUNIT, stbprp.ATCUNIT);
    silly_encode::iconv_convert(from, to, stbprp.ADMAUTH, stbprp.ADMAUTH);
    silly_encode::iconv_convert(from, to, stbprp.LOCALITY, stbprp.LOCALITY);
    silly_encode::iconv_convert(from, to, stbprp.STBK, stbprp.STBK);
    silly_encode::iconv_convert(from, to, stbprp.PHCD, stbprp.PHCD);
    silly_encode::iconv_convert(from, to, stbprp.USFL, stbprp.USFL);
    silly_encode::iconv_convert(from, to, stbprp.COMMENTS, stbprp.COMMENTS);
    silly_encode::iconv_convert(from, to, stbprp.HNNM0, stbprp.HNNM0);
    silly_encode::iconv_convert(from, to, stbprp.ADCD, stbprp.ADCD);
    silly_encode::iconv_convert(from, to, stbprp.ADDVCD1, stbprp.ADDVCD1);

    return true;
}

static bool encode(std::vector<silly_stbprp>& stbprps, const std::string& from, const std::string& to)
{
    for (auto& stbprp : stbprps)
    {
        encode(stbprp, from, to);
    }
    return true;
}

// 将data中的字符串都写入到文件中,追加的写入
static bool saveInfo(const std::string& filename, const std::vector<std::string>& data)
{
    bool status = false;
    std::ofstream file(filename, std::ios::app | std::ios::binary);
    if (!file.is_open())
    {
        SLOG_ERROR("文件打开失败:{}", filename);
        return status;
    }
    for (const auto& str : data)
    {
        file << str;
    }
    file.close();
    status = true;
    return status;
}

// 创建 stcd 和 index 的映射 (导出, 给stbprp中的stcd赋予index,然后给pptn..表中的stcd 赋予index)
static bool createStcdIndex(std::vector<silly_stbprp>& stbprps, std::unordered_map<std::string, uint32_t>& stcdindex)
{
    uint32_t index = 1;
    for (auto& stbprp : stbprps)
    {
        stbprp.index = index;
        stcdindex[stbprp.STCD] = index;
        index++;
    }
    return true;
}

// 创建 index 和 stcd 的映射 (导入,pptn..中的index去stbprp中找stcd)
static bool creatIndexStcd(const std::vector<silly_stbprp>& stbprps, std::unordered_map<uint32_t, std::string>& indexstcd)
{
    // 根据 stbprps 生成说有 stcd 和 index 的映射
    for (const auto& stbprp : stbprps)
    {
        indexstcd[stbprp.index] = stbprp.STCD;
    }
    return true;
}

// (导入) 模板函数根据index查找stcd
template <typename T>
std::vector<T> getStcd(const std::vector<T>& opts)
{
    std::unordered_map<unsigned int, std::string> index_stcd = silly_rwdb_stbprp::m_index_stcd;
    // 根据 index_stcd 查找并更新 stcd
    std::vector<T> res_opt;
    for (auto& opt : opts)
    {
        uint32_t t_index = opt.index;
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            if (!t_stcd.empty())
            {
                auto temp = opt;
                temp.stcd = t_stcd;
                res_opt.push_back(temp);
            }
        }
    }
    return res_opt;
}

// (导出) 根据stcd_index获取stcd对应的index,并写入datas中stcd字段
template <typename T>
std::vector<T> getIndex(const std::vector<T>& opts)
{
    std::unordered_map<std::string, uint32_t> stcd_index = silly_rwdb_stbprp::m_stcd_index;
    std::vector<T> res_opt;
    for (auto& opt : opts)
    {
        std::string stcd = opt.stcd;
        if (stcd_index.find(stcd) != stcd_index.end())
        {
            auto temp = opt;
            temp.index = stcd_index[stcd];
            res_opt.push_back(temp);
        }
    }
    return res_opt;
}

#endif  // TOOLS_H
