
#ifndef SILLY_RWDB_RSVR_H
#define SILLY_RWDB_RSVR_H

#include "tzx/rwdb/silly_rsvr.h"

extern silly_otl otl;
extern std::string src_encode;
extern std::string dst_encode;

class silly_rwdb_rsvr
{
  public:
    // 导出 rsvr
    static bool output(const std::vector<std::pair<std::string, std::string>>& btm_etm);

    // 导入 rsvr
    static bool import(const size_t block_size = 1024 * 1024 * 1024);

    // 数据库加载 rsvr 数据(导出)
    static bool loads(const std::string& btm, const std::string& etm);

    // 将rsvr数组序列化成二进制流(导出)
    static bool serialize(std::vector<std::string>& datas);

    // 将二进制流反序列化成rsvr数组(导入)
    static bool deserialize(const std::string& block_data, int& residue_size);

    // 插入rsvr到数据库(导入)
    static bool insert();

    static void setSelectRsvrSql(const std::string& sql)
    {
        m_select_rsvr_sql = sql;
    }
    static void setInsertRsvrSql(const std::string& sql)
    {
        m_insert_rsvr_sql = sql;
    }
    static void setRsvrFilePath(const std::string& path)
    {
        m_rsvr_file_path = path;
    }
    static void setNowTime(const std::string& tm)
    {
        m_str_now_tm = tm;
    }

  public:
    static std::vector<silly_rsvr> m_rsvrs;

    static std::string m_str_now_tm;
    static std::string m_select_rsvr_sql;
    static std::string m_insert_rsvr_sql;
    static std::string m_rsvr_file_path;
};

#endif  // SILLY_RWDB_RSVR_H