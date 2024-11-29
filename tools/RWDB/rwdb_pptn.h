
#ifndef RWDB_PPTN_H
#define RWDB_PPTN_H

#include "rwdb_base.h"
#include "tzx/rwdb/silly_pptn.h"

class rwdb_pptn : public rwdb_base
{
  public:
    // 数据库加载 PPTN 数据(导出)
    bool loads(const std::string& btm, const std::string& etm) override;

    // 将pptn数组序列化成二进制流(导出)
    bool serialize(std::vector<std::string>& datas) override;

    // 将二进制流反序列化成pptn数组(导入)
    bool deserialize(const std::string& block_data, int& residue_size) override;

    // 插入pptn到数据库(导入)
    bool insert() override;

    std::string getFileName() const override
    {
        return silly_pptn::FILE_NAME;
    }

    bool getIndexs() override
    {
        m_pptns = getIndex(m_pptns);
        return true;
    }
    bool getStcds() override
    {
        m_pptns = getStcd(m_pptns);
        return true;
    }
    bool clear() override
    {
        m_pptns.clear();
        return true;
    }
  public:
    std::vector<silly_pptn> m_pptns;
};

#endif  // RWDB_PPTN_H