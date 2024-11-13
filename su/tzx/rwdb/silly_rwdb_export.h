
#ifndef SILLY_RWDB_EXPORT_H
#define SILLY_RWDB_EXPORT_H

#include <su_marco.h>

#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_stbprp.h>
#include <tzx/rwdb/silly_rsvr.h>
#include <tzx/rwdb/silly_river.h>

class silly_rwdb_export
{
  public:
    static bool init();

    /// <summary>
    /// 导出 STBPRP
    /// </summary>
    /// <returns></returns>
    static bool exportSTBPRP();
    // 数据库加载 STBPRP 数据
    static bool loadSTBPRP(std::vector<silly_stbprp>& stbprps)
    // 序列化 STBPRP 数据为二进制流
    static bool serializeSTBPRP(const std::vector<silly_stbprp>& stbprp, std::vector<std::string>& datas);

    /// <summary>
    /// 导出 PPTN
    /// </summary>
    /// <param name="beginTM">查询总开始时间</param>
    /// <param name="endTM">查询总结束时间</param>
    /// <param name="hour">时间间隔,单位小时</param>
    /// <returns></returns>
    static bool exportPPTN(const std::string& beginTM, const std::string& endTM, const int& hour);
    // 数据库加载 PPTN 数据
    static bool loadPPTN(const std::string& btm, const std::string& etm, std::vector<silly_pptn>& pptns);
    // 序列化 PPTN 数据为二进制流
    static bool serializePPTN(std::vector<silly_pptn>& pptn, std::vector<std::string>& datas);

    /// <summary>
    /// 导出 RSVR
    /// </summary>
    /// <param name="beginTM"></param>
    /// <param name="endTM"></param>
    /// <param name="hour"></param>
    /// <returns></returns>
    static bool exportRiver(const std::string& beginTM, const std::string& endTM, const int& hour);
    // 数据库加载 RSVR 数据
    static bool loadRSVR(const std::string& btm, const std::string& etm, std::vector<silly_rsvr>& rsrvs);
    // 序列化 RSVR 数据为二进制流
    static bool serializeRSVR(std::vector<silly_rsvr>& rsvr, std::vector<std::string>& datas);

    /// <summary>
    /// 导出 RIVER
    /// </summary>
    /// <param name="beginTM"></param>
    /// <param name="endTM"></param>
    /// <param name="hour"></param>
    /// <returns></returns>
    static bool exportRsvr(const std::string& beginTM, const std::string& endTM, const int& hour);
    // 数据库加载 RIVER 数据
    static bool loadRIVER(const std::string& btm, const std::string& etm, std::vector<silly_river>& rivers);
    // 序列化 RIVER 数据为二进制流
    static bool serializeRIVER(std::vector<silly_river>& river, std::vector<std::string>& datas);

    static std::unordered_map<std::string, uint32_t> stcd_index;  // STCD 与 索引 的映射
    static silly_otl otl;

  private:
};

#endif  // SILLY_RWDB_EXPORT_H