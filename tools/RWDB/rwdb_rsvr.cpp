
#include "rwdb_rsvr.h"

#include "files/silly_file.h"
#include "tools.h"


bool rwdb_rsvr::loads(const std::string& btm, const std::string& etm)
{
    bool status = false;
    std::string sql = silly_format::format(m_select_sql, btm, etm);
    if (!otl.select(sql, [&](otl_stream* stream) {
            while (!stream->eof())
            {
                silly_rsvr tmp_rsvr;

                otl_value<std::string> STCD, RWCHRCD, RWPTN, MSQMT;
                otl_value<double> RZ, INQ, W, OTQ, INQDR, BLRZ;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, RZ, INQ, W, OTQ, RWCHRCD, RWPTN, INQDR, MSQMT, BLRZ);

                tmp_rsvr.stcd = STCD.v;
                tmp_rsvr.rz = RZ.v;
                tmp_rsvr.inq = INQ.v;
                tmp_rsvr.w = W.v;
                tmp_rsvr.otq = OTQ.v;
                tmp_rsvr.rwchrcd = RWCHRCD.v;
                tmp_rsvr.rwptn = RWPTN.v;
                tmp_rsvr.inqdr = INQDR.v;
                tmp_rsvr.msqmt = MSQMT.v;
                tmp_rsvr.blrz = BLRZ.v;
                tmp_rsvr.stamp = otl_to_timestamp(tm);

                m_rsvrs.push_back(tmp_rsvr);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        m_rsvrs.clear();  // 查询失败,返回空
        return status;
    }

    SLOG_INFO("查询到:{} 条 Rsvr 数据", m_rsvrs.size());
    status = true;
    return status;
}

bool rwdb_rsvr::serialize(std::vector<std::string>& datas)
{
    for (const auto& rsvr : m_rsvrs)
    {
        datas.push_back(rsvr.serialize());
    }
    return true;
}

bool rwdb_rsvr::deserialize(const std::string& block_data, int& residue_size)
{
    m_rsvrs.clear();
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

bool rwdb_rsvr::insert()
{
    otl_datetime tm0;
    tm0.second= 0;
    tm0.minute = 0;
    tm0.hour = 1;
    tm0.day = 31;
    tm0.month = 7;
    tm0.year = 2024;
    bool status = false;
    std::time_t stamp_0 = otl_to_timestamp(tm0);

    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, m_rsvrs.size());
    while (bi < m_rsvrs.size())
    {
        if (!otl.insert(m_insert_sql, [&](otl_stream* stream) {
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
                    if (entry.stcd == "600K0050" && entry.stamp == stamp_0)
                    {
                        int a = 0;
                    }

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
