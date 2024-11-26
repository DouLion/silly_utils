

#include "silly_rwdb_base.h"
#include "silly_rwdb_base.h"

bool silly_rwdb_base::output(const std::vector<std::pair<std::string, std::string>>& btm_etm)
{
    bool status = false;
    std::string fileName = m_str_now_tm + "_" + getFileName();
    for (const auto& [_btm, _etm] : btm_etm)
    {
        // 数据库查询
        if (!loads(_btm, _etm))
        {
            SLOG_ERROR("数据库加载失败 ({} ~ {})", _btm, _etm);
            continue;
        }

        // 根据stcd 获取index
        getIndexs();

        // 序列化记录为二进制文件
        std::vector<std::string> datas;
        if (!serialize(datas))
        {
            SLOG_ERROR("序列化失败  ({} ~ {})", _btm, _etm);
            continue;
        }

        // 写入文件
        if (!saveInfo(fileName, datas))
        {
            SLOG_ERROR("写入文件失败  ({} ~ {})", _btm, _etm);
            continue;
        }
        clear();
        SLOG_INFO("写入文件成功: {}", fileName);
    }

    status = true;
    return status;
}

bool silly_rwdb_base::import(const size_t block_size)
{
    bool status = false;
    if (!std::filesystem::exists(m_file_path))
    {
        SLOG_ERROR("文件不存在: {}", m_file_path);
        return status;
    }
    SLOG_INFO("开始导入文件: {}", m_file_path);
    size_t fsize = silly_file::size(m_file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(m_file_path, block_data, pos, block_size);
        int residue_size = 0;
        if (!deserialize(block_data, residue_size))
        {
            SLOG_ERROR("反序列化pptn失败");
            return status;
        }
        // 获取pptn中的stcd
        getStcds();
        // 插入pptn到数据库
        if (!insert())
        {
            SLOG_ERROR("导入pptn失败");
            return status;
        }
        pos = pos + rsize - residue_size;
    }
   
    status = true;
    return status;
}
