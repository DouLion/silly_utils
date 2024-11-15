
#ifndef SILLY_RWDB_STBPRP_H
#define SILLY_RWDB_STBPRP_H

#include "silly_rwdb_base.h"
#include "tzx/rwdb/silly_stbprp.h"

// 寻找全局变量
extern silly_otl otl;
extern std::string src_encode;
extern std::string dst_encode;


class silly_rwdb_stbprp 
{
  public:
    // 导出 STBPRP
    bool output();

    // 导入 STBPRP
    bool import();

    // 数据库加载 STBPRP 数据(导出)
    bool loads();

    // 读取stbprp文件中的全部数据(导入)
    bool dumps(std::string& content);

    // 将stbprp数组序列化成二进制流(导出)
    bool serialize(std::vector<std::string>& datas) ;

    // 将二进制流反序列化成stbprp数组(导入)
    bool deserialize(const std::string& content);

    // 插入stbprp到数据库
    bool insert() ;

        void setSelectSql(const std::string& sql)
    {
        m_select_sql = sql;
    }
    void setInsertSql(const std::string& sql)
    {
        m_insert_sql = sql;
    }
    void setFilePath(const std::string& path)
    {
        m_file_path = path;
    }
    void setNowTm(const std::string& now_tm)
    {
        m_str_now_tm = now_tm;
    }

    std::string getFileName() const 
    {
        return silly_stbprp::FILE_NAME;
    }
  public:
    std::vector<silly_stbprp> m_stbprps;
    static std::unordered_map<std::string, uint32_t> m_stcd_index;
    static std::unordered_map<uint32_t, std::string> m_index_stcd;

        std::string m_select_sql;
    std::string m_insert_sql;
    std::string m_file_path;
    std::string m_str_now_tm;
};

#endif  // SILLY_RWDB_STBPRP_H