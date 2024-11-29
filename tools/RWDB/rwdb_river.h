
#ifndef RWDB_RIVER_H
#define RWDB_RIVER_H

#include "rwdb_base.h"
#include "tzx/rwdb/silly_river.h"

class rwdb_river : public rwdb_base
{
  public:
    // 数据库加载 river 数据(导出)
    bool loads(const std::string& btm, const std::string& etm) override;

    // 将river数组序列化成二进制流(导出)
    bool serialize(std::vector<std::string>& datas) override;

    // 将二进制流反序列化成river数组(导入)
    bool deserialize(const std::string& block_data, int& residue_size) override;

    // 插入river到数据库(导入)
    bool insert() override;

    // 实现纯虚函数：返回文件名
    std::string getFileName() const override
    {
        return silly_river::FILE_NAME;
    }

    bool getIndexs() override
    {
        m_rivers = getIndex(m_rivers);
        return true;
    }
    bool getStcds() override
    {
        m_rivers = getStcd(m_rivers);
        return true;
    }
    bool clear() override
    {
        m_rivers.clear();
        return true;
    }

  public:
    std::vector<silly_river> m_rivers;
};

#endif  // RWDB_RIVER_H