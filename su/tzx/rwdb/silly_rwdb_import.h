
#ifndef SILLY_RWDB_IMPORT_H
#define SILLY_RWDB_IMPORT_H

#include <su_marco.h>

class silly_rwdb_import
{
  public:
    static bool init();

    /// <summary>
    /// 导入STBPRP
    /// </summary>
    /// <param name="file_path"></param>
    /// <param name="import"></param>
    /// <returns></returns>
    static bool importSTBPRP(const std::string& file_path, bool import = false);
    // 从文件中读取全部的stbprp数据
    static bool loadSTBPRP(const std::string& file_path, std::vector<silly_stbprp>& stbprps);
    // 插入stbprp到数据库
    static bool insertSTBPRP(std::vector<silly_stbprp>& stbprps);

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

    static bool importRiver(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024);
    static bool deserializeRiver(const std::string& block_data, std::vector<silly_river>& rivers, int& residue_size);
    static bool insertRiver(std::vector<silly_river>& rivers);

    static bool importRsvr(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024);
    static bool deserializeRsvr(const std::string& block_data, std::vector<silly_rsvr>& rsvrs, int& residue_size);
    static bool insertRsvr(std::vector<silly_rsvr>& rsvrs);

    static std::unordered_map<uint32_t, std::string> index_stcd;  // 索引和stcd的映射
    static silly_otl otl;

  private:
};

#endif  // SILLY_RWDB_IMPORT_H