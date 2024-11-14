
#ifndef SILLY_RWDB_STBPRP_H
#define SILLY_RWDB_STBPRP_H

#include "tzx/rwdb/silly_stbprp.h"

// 寻找全局变量
extern silly_otl otl;
extern std::string src_encode;
extern std::string dst_encode;
// extern std::string str_now_tm;
// extern std::string select_stbprp_sql;
// extern std::string insert_stbprp_sql;
// extern std::string stbprp_file_path;
// extern std::unordered_map<std::string, uint32_t> g_stcd_index;
// extern std::unordered_map<uint32_t, std::string> g_index_stcd;

class silly_rwdb_stbprp
{
  public:
    // 导出 STBPRP
    static bool output();

    // 导入 STBPRP
    static bool import();

    // 数据库加载 STBPRP 数据(导出)
    static bool loads();

    // 读取stbprp文件中的全部数据(导入)
    static bool dumps(std::string& content);

    // 将stbprp数组序列化成二进制流(导出)
    static bool serialize(std::vector<std::string>& datas);

    // 将二进制流反序列化成stbprp数组(导入)
    static bool deserialize(const std::string& content);

    // 插入stbprp到数据库
    static bool insert();

    // 设置和获取 SQL 查询字符串
    static void setSelectStbprpSql(const std::string& sql)
    {
        m_select_stbprp_sql = sql;
    }
    static void setInsertStbprpSql(const std::string& sql)
    {
        m_insert_stbprp_sql = sql;
    }
    static void setStbprpFilePath(const std::string& path)
    {
        m_stbprp_file_path = path;
    }
    static void setNowTime(const std::string& str_now_tm)
    {
        m_str_now_tm = str_now_tm;
    }

  public:
    static std::vector<silly_stbprp> m_stbprps;

    static std::unordered_map<std::string, uint32_t> m_stcd_index;
    static std::unordered_map<uint32_t, std::string> m_index_stcd;

    static std::string m_select_stbprp_sql;
    static std::string m_insert_stbprp_sql;
    static std::string m_stbprp_file_path;
    static std::string m_str_now_tm;
};

#endif  // SILLY_RWDB_STBPRP_H