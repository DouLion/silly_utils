
#include "encode/silly_encode.h"

struct option
{
    bool stbprp{false};
    bool pptn{false};
    bool river{false};
    bool rsvr{false};
} _opt;

// otl_datetime 转 时间戳
static std::time_t otl_to_timestamp(const otl_datetime& olt_tm)
{
    // std::tm t{tm.second, tm.minute, tm.hour, tm.day, tm.month - 1, tm.year - 1900};
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
otl_datetime otl_from_timestamp(std::time_t timestamp)
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

static void paramAnalysis(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if ("pptn" == argv[i])
        {
            _opt.pptn = true;
        }
        else if ("rsvr" == argv[i])
        {
            _opt.river = true;
        }
        else if ("river" == argv[i])
        {
            _opt.rsvr = true;
        }
        else if("stbprp" == argv[i])
        {
            _opt.stbprp = true;
        }
    }
}

// 检查查询数据,不为null 则赋值
#define CHECK_NULL_VALUE(var, dest) \
    if (!(var).is_null())           \
    {                               \
        (dest) = (var).v;           \
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
