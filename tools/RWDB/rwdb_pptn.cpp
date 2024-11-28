
#include "rwdb_pptn.h"
#include "files/silly_file.h"
#include "tools.h"

bool silly_rwdb_pptn::loads(const std::string& btm, const std::string& etm)
{
    bool status = false;
    std::string sql = silly_format::format(m_select_sql, btm, etm);
    m_pptns.clear();
    if (!otl.select(sql, [&](otl_stream* stream) {
            while (!stream->eof())
            {
                silly_pptn tmp_pptn;

                otl_value<std::string> STCD;
                otl_value<double> DRP, INTV;
                otl_datetime tm;

                otl_read_row(*stream, STCD, tm, DRP, INTV);
                tmp_pptn.stcd = STCD.v;
                tmp_pptn.drp = DRP.v;
                tmp_pptn.intv = INTV.v;
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
    m_pptns.clear();
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
        if (!otl.insert(m_insert_sql, [&](otl_stream* stream) {
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
