
#ifndef SILLY_RWDB_IMPORT_H
#define SILLY_RWDB_IMPORT_H

#include <su_marco.h>

#include <tzx/rwdb/silly_pptn.h>
#include <tzx/rwdb/silly_stbprp.h>
#include <tzx/rwdb/silly_rsvr.h>
#include <tzx/rwdb/silly_river.h>

// 寻找全局变量
extern silly_otl otl;
extern std::string src_encode;
extern std::string dst_encode;
extern std::string insert_stbprp_sql;
extern std::string insert_pptn_sql;
extern std::string insert_river_sql;
extern std::string insert_rsvr_sql;
extern std::string stbprp_file_path;
extern std::string pptn_file_path;
extern std::string river_file_path;
extern std::string rsvr_file_path;
extern std::unordered_map<uint32_t, std::string> index_stcd;

class silly_import_stbprp
{
  public:
    // 读取strprp文件并序列化,生成index和stcd的映射关系
    static bool importSTBPRP(const std::string& file_path);

    // 读取stbprp文件中的全部数据
    static bool loadSTBPRP(const std::string& file_path, std::string& content);

    // 反序列化stbprp文件中的全部数据
    static bool deserializeSTBPRP(const std::string& content, std::vector<silly_stbprp>& stbprps);

    // 插入stbprp到数据库
    static bool insertSTBPRP(std::vector<silly_stbprp>& stbprps);
};

class silly_import_pptn
{
  public:
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
};

class silly_import_river
{
  public:
    /// <summary>
    /// 导入river
    /// </summary>
    /// <param name="file_path"></param>
    /// <param name="block_size"></param>
    /// <returns></returns>
    static bool importRiver(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024);
    /// <summary>
    /// 从字符串解析river数据
    /// </summary>
    /// <param name="block_data"></param>
    /// <param name="rivers"></param>
    /// <param name="residue_size">剩余数据大小</param>
    /// <returns></returns>
    static bool deserializeRiver(const std::string& block_data, std::vector<silly_river>& rivers, int& residue_size);

    /// <summary>
    /// 插入river到数据库
    /// </summary>
    /// <param name="rivers"></param>
    /// <returns></returns>
    static bool insertRiver(std::vector<silly_river>& rivers);
};

class silly_import_rsvr
{
  public:
    /// <summary>
    /// 导入rsvr
    /// </summary>
    /// <param name="file_path"></param>
    /// <param name="block_size"></param>
    /// <returns></returns>
    static bool importRsvr(const std::string& file_path, const size_t block_size = 1024 * 1024 * 1024);

    /// <summary>
    /// 从字符串解析rsvr数据
    /// </summary>
    /// <param name="block_data"></param>
    /// <param name="rsvrs"></param>
    /// <param name="residue_size">剩余数据大小</param>
    /// <returns></returns>
    static bool deserializeRsvr(const std::string& block_data, std::vector<silly_rsvr>& rsvrs, int& residue_size);

    /// <summary>
    /// 插入rsvr到数据库
    /// </summary>
    /// <param name="rsvrs"></param>
    /// <returns></returns>
    static bool insertRsvr(std::vector<silly_rsvr>& rsvrs);
};

#endif  // SILLY_RWDB_IMPORT_H