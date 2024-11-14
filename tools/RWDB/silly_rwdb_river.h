
#ifndef SILLY_RWDB_RIVER_H
#define SILLY_RWDB_RIVER_H

#include "tzx/rwdb/silly_river.h"

extern silly_otl otl;
extern std::string src_encode;
extern std::string dst_encode;

class silly_rwdb_river
{
  public:
    // 导出 river
    static bool output(const std::vector<std::pair<std::string, std::string>>& btm_etm);

    // 导入 river
    static bool import(const size_t block_size = 1024 * 1024 * 1024);

    // 数据库加载 river 数据(导出)
    static bool loads(const std::string& btm, const std::string& etm);

    // 将river数组序列化成二进制流(导出)
    static bool serialize(std::vector<std::string>& datas);

    // 将二进制流反序列化成river数组(导入)
    static bool deserialize(const std::string& block_data, int& residue_size);

    // 插入river到数据库(导入)
    static bool insert();

    static void setSelectRiverSql(const std::string& sql)
    {
        m_select_river_sql = sql;
    }
    static void setInsertRiverSql(const std::string& sql)
    {
        m_insert_river_sql = sql;
    }
    static void setRiverFilePath(const std::string& path)
    {
        m_river_file_path = path;
    }
    static void setNowTime(const std::string& tm)
    {
        m_str_now_tm = tm;
    }

  public:
    static std::vector<silly_river> m_rivers;

    static std::string m_str_now_tm;
    static std::string m_select_river_sql;
    static std::string m_insert_river_sql;
    static std::string m_river_file_path;
};

#endif  // SILLY_RWDB_RIVER_H