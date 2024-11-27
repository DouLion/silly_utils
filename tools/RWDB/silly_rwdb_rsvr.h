
#ifndef SILLY_RWDB_RSVR_H
#define SILLY_RWDB_RSVR_H

#include "silly_rwdb_base.h"
#include "tzx/rwdb/silly_rsvr.h"

extern silly_otl otl;
extern std::string src_encode;
extern std::string dst_encode;

class silly_rwdb_rsvr : public silly_rwdb_base
{
  public:
    // 数据库加载 rsvr 数据(导出)
    bool loads(const std::string& btm, const std::string& etm) override;

    // 将rsvr数组序列化成二进制流(导出)
    bool serialize(std::vector<std::string>& datas) override;

    // 将二进制流反序列化成rsvr数组(导入)
    bool deserialize(const std::string& block_data, int& residue_size) override;

    // 插入rsvr到数据库(导入)
    bool insert() override;

    std::string getFileName() const override
    {
        return silly_rsvr::FILE_NAME;
    }

    bool getIndexs() override
    {
        m_rsvrs = getIndex(m_rsvrs);
        return true;
    }
    bool getStcds() override
    {
        m_rsvrs = getStcd(m_rsvrs);
        return true;
    }
    bool clear() override
    {
        m_rsvrs.clear();
        return true;
    }

  public:
    std::vector<silly_rsvr> m_rsvrs;
};

#endif  // SILLY_RWDB_RSVR_H