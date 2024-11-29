
#ifndef RWDB_BASE_H
#define RWDB_BASE_H

#include <tzx/rwdb/silly_rwdb_record.h>
#include "tzx/rwdb/silly_stbprp.h"
#include <database/otl/silly_otl.h>
#include "tools.h"
#include "files/silly_file.h"

class rwdb_base
{
  public:
    // 导出数据
    virtual bool output(const std::map<std::string, std::string>& btm_etm);

    virtual bool import(const size_t block_size = 1024 * 1024 * 1024);

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

    // 插入数据到数据库
    virtual bool insert()
    {
        return false;
    }
    // 加载数据
    virtual bool loads(const std::string& btm, const std::string& etm)
    {
        return false;
    }

    virtual bool serialize(std::vector<std::string>& datas)
    {
        return false;
    }
    virtual bool deserialize(const std::string& block_data, int& residue_size)
    {
        return false;
    }

  protected:

    virtual std::string getFileName() const
    {
        return "";
    }
    virtual bool getIndexs()
    {
        return false;
    }
    virtual bool getStcds()
    {
        return false;
    }
    virtual bool clear()
    {
        return false;
    }

  protected:
    std::string m_select_sql;
    std::string m_insert_sql;
    std::string m_file_path;
    std::string m_str_now_tm;
};

#endif  // RWDB_BASE_H