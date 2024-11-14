
#ifndef SILLY_RWDB_PPTN_H
#define SILLY_RWDB_PPTN_H

#include "tzx/rwdb/silly_pptn.h"

extern silly_otl otl;
extern std::string src_encode;
extern std::string dst_encode;

class silly_rwdb_pptn
{
  public:
    // 导出 PPTN
    static bool output(const std::vector<std::pair<std::string, std::string>>& btm_etm);

    // 导入 PPTN
    static bool import(const size_t block_size = 1024 * 1024 * 1024);

    // 数据库加载 PPTN 数据(导出)
    static bool loads(const std::string& btm, const std::string& etm);

    // 将pptn数组序列化成二进制流(导出)
    static bool serialize(std::vector<std::string>& datas);

    // 将二进制流反序列化成pptn数组(导入)
    static bool deserialize(const std::string& block_data, int& residue_size);

    // 插入pptn到数据库(导入)
    static bool insert();

    static void setSelectPPTNsql(const std::string& sql)
    {
        m_select_pptn_sql = sql;
    }
    static void setInsertPPTNsql(const std::string& sql)
    {
        m_insert_pptn_sql = sql;
    }
    static void setPPTNFilePath(const std::string& path)
    {
        m_pptn_file_path = path;
    }
    static void setNowTime(const std::string& now_tm)
    {
        m_str_now_tm = now_tm;
    }

  public:
    static std::vector<silly_pptn> m_pptns;

    static std::string m_select_pptn_sql;
    static std::string m_insert_pptn_sql;
    static std::string m_pptn_file_path;
    static std::string m_str_now_tm;
};

#endif  // SILLY_RWDB_PPTN_H