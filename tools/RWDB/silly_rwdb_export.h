
#ifndef SILLY_RWDB_EXPORT_H
#define SILLY_RWDB_EXPORT_H

#include <su_marco.h>

#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_stbprp.h>
#include <tzx/rwdb/silly_rsvr.h>
#include <tzx/rwdb/silly_river.h>

// 寻找全局变量
extern silly_otl otl;
extern std::string src_encode;
extern std::string dst_encode;
extern std::string str_now_tm;
extern std::string select_stbprp_sql;
extern std::string select_pptn_sql;
extern std::string select_river_sql;
extern std::string select_rsvr_sql;
extern std::unordered_map<std::string, uint32_t> stcd_index;

class silly_export_stbprp
{
  public:
    /// <summary>
    /// 导出 STBPRP
    /// </summary>
    /// <returns></returns>
    static bool exportSTBPRP();

    // 数据库加载 STBPRP 数据
    static bool loadSTBPRP(std::vector<silly_stbprp>& stbprps);

    // 序列化 STBPRP 数据为二进制流
    static bool serializeSTBPRP(const std::vector<silly_stbprp>& stbprp, std::vector<std::string>& datas);
};

class silly_export_pptn
{
  public:
    /// <summary>
    /// 导出 PPTN
    /// </summary>
    /// <param name="btm_etm"></param>
    /// <returns></returns>
    static bool exportPPTN(const std::vector<std::pair<std::string, std::string>>& btm_etm);
    // 数据库加载 PPTN 数据
    static bool loadPPTN(const std::string& btm, const std::string& etm, std::vector<silly_pptn>& pptns);
    // 序列化 PPTN 数据为二进制流
    static bool serializePPTN(std::vector<silly_pptn>& pptn, std::vector<std::string>& datas);
};

class silly_export_river
{
  public:
    /// <summary>
    /// 导出 RIVER
    /// </summary>
    /// <param name="btm_etm"></param>
    /// <returns></returns>
    static bool exportRiver(const std::vector<std::pair<std::string, std::string>>& btm_etm);
    // 数据库加载 RSVR 数据
    static bool loadRiver(const std::string& btm, const std::string& etm, std::vector<silly_river>& rivers);
    // 序列化 RSVR 数据为二进制流
    static bool serializeRiver(std::vector<silly_river>& rivers, std::vector<std::string>& datas);
};

class silly_export_rsvr
{
  public:
    /// <summary>
    /// 导出 RSVR
    /// </summary>
    /// <param name="btm_etm"></param>
    /// <returns></returns>
    static bool exportRsvr(const std::vector<std::pair<std::string, std::string>>& btm_etm);
    // 数据库加载 RIVER 数据
    static bool loadRsvr(const std::string& btm, const std::string& etm, std::vector<silly_rsvr>& rsvrs);
    // 序列化 RIVER 数据为二进制流
    static bool serializeRsvr(std::vector<silly_rsvr>& rsvrs, std::vector<std::string>& datas);
};

#endif  // SILLY_RWDB_EXPORT_H