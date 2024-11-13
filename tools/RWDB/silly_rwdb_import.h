
#ifndef SILLY_RWDB_IMPORT_H
#define SILLY_RWDB_IMPORT_H

#include <su_marco.h>

class silly_import_stbprp
{
  public:
    static bool init();

    // 读取strprp文件并序列化,生成index和stcd的映射关系
    bool importSTBPRP(const std::string& file_path);

    // 读取stbprp文件中的全部数据
    bool loadSTBPRP(const std::string& file_path, std::string& content);

    // 反序列化stbprp文件中的全部数据
    bool deserializeSTBPRP(const std::string& content, std::vector<silly_stbprp>& stbprps);

    // 插入stbprp到数据库
    bool insertSTBPRP(std::vector<silly_stbprp>& stbprps);

  public:
    static std::unordered_map<uint32_t, std::string> index_stcd;  // 索引和stcd的映射
    static silly_otl otl;
};

class silly_import_pptn
{
  public:
    static bool init();

    /// <summary>
    /// 导入PPTN
    /// </summary>
    /// <param name="file_path"></param>
    /// <param name="block_size">每次读取文件的字节数,默认1G</param>
    /// <returns></returns>
    static bool importPPTN(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024);
    /// <summary>
    /// 从字符串解析pptn数据
    /// </summary>
    /// <param name="block_data"></param>
    /// <param name="pptns"></param>
    /// <param name="residue_size">剩余数据大小</param>
    /// <returns></returns>
    static bool deserializePPTN(const std::string& block_data, std::vector<silly_pptn>& pptns, int& residue_size);
    // 导入pptn到数据库
    static bool insertPPTN(std::vector<silly_pptn>& pptns);

  public:
    static std::unordered_map<uint32_t, std::string> index_stcd;  // 索引和stcd的映射
    static silly_otl otl;
};

class silly_import_river
{
  public:
    static bool init();

    static bool importRiver(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024);
    static bool deserializeRiver(const std::string& block_data, std::vector<silly_river>& rivers, int& residue_size);
    static bool insertRiver(std::vector<silly_river>& rivers);

  public:
    static std::unordered_map<uint32_t, std::string> index_stcd;  // 索引和stcd的映射
    static silly_otl otl;
};

class silly_import_rsvr
{
  public:
    static bool init();

    static bool importRsvr(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024);
    static bool deserializeRsvr(const std::string& block_data, std::vector<silly_rsvr>& rsvrs, int& residue_size);
    static bool insertRsvr(std::vector<silly_rsvr>& rsvrs);

  public:
    static std::unordered_map<uint32_t, std::string> index_stcd;  // 索引和stcd的映射
    static silly_otl otl;
};

#endif  // SILLY_RWDB_IMPORT_H