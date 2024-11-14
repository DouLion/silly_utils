
#include "silly_rwdb_rsvr.h"

#include "files/silly_file.h"
#include "tools.h"

std::vector<silly_rsvr> silly_rwdb_rsvr::m_rsvrs;
std::string silly_rwdb_rsvr::m_str_now_tm;
std::string silly_rwdb_rsvr::m_select_rsvr_sql;
std::string silly_rwdb_rsvr::m_insert_rsvr_sql;
std::string silly_rwdb_rsvr::m_rsvr_file_path;

bool silly_rwdb_rsvr::output(const std::vector<std::pair<std::string, std::string>>& btm_etm)
{
    bool status = false;
    std::string fileName = m_str_now_tm + "_" + silly_rsvr::FILE_NAME;
    for (const auto& [_btm, _etm] : btm_etm)
    {
        // 数据库查询 PPTN
        m_rsvrs.clear();
        if (!loads(_btm, _etm))
        {
            SLOG_ERROR("PPTN 数据库加载失败 ({} ~ {})", _btm, _etm);
            continue;
        }
        // 根据stcd 获取index
        m_rsvrs = getIndex(m_rsvrs);
        // 序列化 rsvrs 为二进制文件
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

bool silly_rwdb_rsvr::import(const size_t block_size)
{
    bool status = false;
    size_t fsize = silly_file::size(m_rsvr_file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(m_rsvr_file_path, block_data, pos, block_size);
        m_rsvrs.clear();
        int residue_size = 0;
        if (!deserialize(block_data, residue_size))
        {
            SLOG_ERROR("反序列化rsvr失败");
            return status;
        }
        // 获取rsvr中的stcd
        m_rsvrs = getStcd(m_rsvrs);
        // 插入rsvr到数据库
        if (!insert())
        {
            SLOG_ERROR("导入rsvr失败");
            return status;
        }
        pos = pos + rsize - residue_size;
    }

    status = true;
    return status;
}

bool silly_rwdb_rsvr::loads(const std::string& btm, const std::string& etm)
{
    bool status = false;
    std::string sql = silly_format::format(m_select_rsvr_sql, btm, etm);
    if (!otl.select(sql, [&](otl_stream* stream) {
            while (!stream->eof())
            {
                silly_rsvr tmp_rsvr;

                otl_value<std::string> STCD, RWCHRCD, RWPTN, MSQMT;
                otl_value<double> RZ, INQ, W, OTQ, INQDR, BLRZ;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, RZ, INQ, W, OTQ, RWCHRCD, RWPTN, INQDR, MSQMT, BLRZ);

                CHECK_NULL_VALUE(STCD, tmp_rsvr.stcd);
                CHECK_NULL_VALUE(RZ, tmp_rsvr.rz);
                CHECK_NULL_VALUE(INQ, tmp_rsvr.inq);
                CHECK_NULL_VALUE(W, tmp_rsvr.w);
                CHECK_NULL_VALUE(OTQ, tmp_rsvr.otq);
                CHECK_NULL_VALUE(RWCHRCD, tmp_rsvr.rwchrcd);
                CHECK_NULL_VALUE(RWPTN, tmp_rsvr.rwptn);
                CHECK_NULL_VALUE(INQDR, tmp_rsvr.inqdr);
                CHECK_NULL_VALUE(MSQMT, tmp_rsvr.msqmt);
                CHECK_NULL_VALUE(BLRZ, tmp_rsvr.blrz);

                tmp_rsvr.stamp = otl_to_timestamp(tm);

                m_rsvrs.push_back(tmp_rsvr);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        m_rsvrs.clear();  // 查询失败,返回空
        return status;
    }

    SLOG_INFO("查询到:{} 条 RIVER 数据", m_rsvrs.size());
    status = true;
    return status;
}

bool silly_rwdb_rsvr::serialize(std::vector<std::string>& datas)
{
    for (const auto& rsvr : m_rsvrs)
    {
        datas.push_back(rsvr.serialize());
    }
    return true;
}

bool silly_rwdb_rsvr::deserialize(const std::string& block_data, int& residue_size)
{
    bool status = false;
    unsigned int object_size = silly_rsvr::SIZE_V1;

    size_t currentPos = 0;
    while (currentPos < block_data.size())
    {
        if (currentPos + object_size > block_data.size())
        {
            residue_size = block_data.size() - currentPos;
            break;
        }
        std::string once_opt_str = block_data.substr(currentPos, object_size);
        silly_rsvr rsvr;
        if (rsvr.deserialize(once_opt_str))
        {
            m_rsvrs.push_back(rsvr);
        }
        currentPos += object_size;
    }

    status = true;
    return true;
}

bool silly_rwdb_rsvr::insert()
{
    bool status = false;

    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, m_rsvrs.size());
    while (bi < m_rsvrs.size())
    {
        if (!otl.insert(m_insert_rsvr_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = m_rsvrs[i];

                    otl_value<std::string> STCD(entry.stcd);
                    otl_datetime OTM = otl_from_timestamp(entry.stamp);
                    otl_value<double> RZ(entry.rz);
                    otl_value<double> INQ(entry.inq);
                    otl_value<double> W(entry.w);
                    otl_value<double> OTQ(entry.otq);
                    otl_value<std::string> RWCHRCD(entry.rwchrcd);
                    otl_value<std::string> RWPTN(entry.rwptn);
                    otl_value<double> INQDR(entry.inqdr);
                    otl_value<std::string> MSQMT(entry.msqmt);
                    otl_value<double> BLRZ(entry.blrz);

                    otl_write_row(*stream, STCD, OTM, RZ, INQ, W, OTQ, RWCHRCD, RWPTN, INQDR, MSQMT, BLRZ);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return status;
        }
        SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, m_rsvrs.size());
    }

    SLOG_INFO("导入数量: {}", m_rsvrs.size());

    status = true;
    return status;
}
