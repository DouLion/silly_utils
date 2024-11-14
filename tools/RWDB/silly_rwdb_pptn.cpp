
#include "silly_rwdb_pptn.h"
#include "files/silly_file.h"
#include "tools.h"

std::vector<silly_pptn> silly_rwdb_pptn::m_pptns;
std::string silly_rwdb_pptn::m_select_pptn_sql;
std::string silly_rwdb_pptn::m_insert_pptn_sql;
std::string silly_rwdb_pptn::m_pptn_file_path;
std::string silly_rwdb_pptn::m_str_now_tm;

bool silly_rwdb_pptn::output(const std::vector<std::pair<std::string, std::string>>& btm_etm)
{
    bool status = false;
    std::string fileName = m_str_now_tm + "_" + silly_pptn::FILE_NAME;
    for (const auto& [_btm, _etm] : btm_etm)
    {
        // 数据库查询 PPTN
        m_pptns.clear();
        if (!loads(_btm, _etm))
        {
            SLOG_ERROR("PPTN 数据库加载失败 ({} ~ {})", _btm, _etm);
            continue;
        }
        // 根据stcd 获取index
        m_pptns = getIndex(m_pptns);
        // 序列化 pptns 为二进制文件
        std::vector<std::string> datas;
        if (!serialize(datas))
        {
            SLOG_ERROR("PPTN 序列化失败  ({} ~ {})", _btm, _etm);
            continue;
        }
        // 写入文件
        if (!saveInfo(fileName, datas))
        {
            SLOG_ERROR("PPTN 写入文件失败  ({} ~ {})", _btm, _etm);
            continue;
        }
    }
    SLOG_INFO("PPTN 写入文件成功: {}", fileName);

    status = true;
    return status;
}

bool silly_rwdb_pptn::import(const size_t block_size)
{
    bool status = false;
    size_t fsize = silly_file::size(m_pptn_file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(m_pptn_file_path, block_data, pos, block_size);
        m_pptns.clear();
        int residue_size = 0;
        if (!deserialize(block_data, residue_size))
        {
            SLOG_ERROR("反序列化pptn失败");
            return status;
        }
        // 获取pptn中的stcd
        m_pptns = getStcd(m_pptns);
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

bool silly_rwdb_pptn::loads(const std::string& btm, const std::string& etm)
{
    bool status = false;
    std::string sql = silly_format::format(m_select_pptn_sql, btm, etm);
    if (!otl.select(sql, [&](otl_stream* stream) {
            while (!stream->eof())
            {
                otl_value<std::string> STCD;
                otl_value<double> DRP, INTV;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, DRP, INTV);
                silly_pptn tmp_pptn;
                CHECK_NULL_VALUE(STCD, tmp_pptn.stcd);
                CHECK_NULL_VALUE(DRP, tmp_pptn.drp);
                CHECK_NULL_VALUE(INTV, tmp_pptn.intv);
                tmp_pptn.stamp = otl_to_timestamp(tm);

                m_pptns.push_back(tmp_pptn);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        m_pptns.clear();
        return status;
    }
    SLOG_INFO("查询到:{} 条 PPTN 数据", m_pptns.size());
    status = true;
    return status;
}

bool silly_rwdb_pptn::serialize(std::vector<std::string>& datas)
{
    for (const auto& pptn : m_pptns)
    {
        datas.push_back(pptn.serialize());
    }
    return true;
}

bool silly_rwdb_pptn::deserialize(const std::string& block_data, int& residue_size)
{
    bool status = false;
    unsigned int object_size = silly_pptn::SIZE_V1;

    size_t currentPos = 0;
    while (currentPos < block_data.size())
    {
        if (currentPos + object_size > block_data.size())
        {
            residue_size = block_data.size() - currentPos;
            break;
        }
        std::string once_opt_str = block_data.substr(currentPos, object_size);
        silly_pptn pptn;
        if (pptn.deserialize(once_opt_str))
        {
            m_pptns.push_back(pptn);
        }
        currentPos += object_size;
    }

    status = true;
    return true;
}

bool silly_rwdb_pptn::insert()
{
    bool status = false;

    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, m_pptns.size());
    while (bi < m_pptns.size())
    {
        if (!otl.insert(m_insert_pptn_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = m_pptns[i];

                    otl_value<std::string> stcd(entry.stcd.c_str());
                    otl_datetime tm = otl_from_timestamp(entry.stamp);
                    otl_value<double> intv(entry.intv);
                    otl_value<double> drp(entry.drp);

                    otl_write_row(*stream, stcd, tm, drp, intv);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return status;
        }
        // SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, m_pptns.size());
    }

    SLOG_INFO("导入数量: {}", m_pptns.size());

    status = true;
    return true;
}
