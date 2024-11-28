
#include "rwdb_river.h"
#include "files/silly_file.h"
#include "tools.h"

bool silly_rwdb_river::loads(const std::string& btm, const std::string& etm)
{
    bool status = false;
    std::string sql = silly_format::format(m_select_sql, btm, etm);
    if (!otl.select(sql, [&](otl_stream* stream) {
            while (!stream->eof())
            {
                silly_river tmp_river;

                otl_value<std::string> STCD, WPTN;
                otl_value<double> Z, Q;
                otl_datetime tm;

                otl_read_row(*stream, STCD, tm, Z, Q, WPTN);
                tmp_river.stcd = STCD.v;
                tmp_river.zz = Z.v;
                tmp_river.qq = Q.v;
                tmp_river.wptn = WPTN.v;
                tmp_river.stamp = otl_to_timestamp(tm);

                m_rivers.push_back(tmp_river);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        m_rivers.clear();  // 查询失败,返回空
        return status;
    }

    SLOG_INFO("查询到:{} 条 RIVER 数据", m_rivers.size());
    status = true;
    return status;
}

bool silly_rwdb_river::serialize(std::vector<std::string>& datas)
{
    for (auto& river : m_rivers)
    {
        datas.push_back(river.serialize(SILLY_RIVER_FORMAT_V2));
    }
    return true;
}

bool silly_rwdb_river::deserialize(const std::string& block_data, int& residue_size)
{
    m_rivers.clear();
    bool status = false;
    unsigned int object_size = silly_river::SIZE_V2;

    size_t currentPos = 0;
    while (currentPos < block_data.size())
    {
        if (currentPos + object_size > block_data.size())
        {
            residue_size = block_data.size() - currentPos;
            break;
        }
        std::string once_opt_str = block_data.substr(currentPos, object_size);
        silly_river river;
        if (river.deserialize(once_opt_str))
        {
            m_rivers.push_back(river);
        }
        currentPos += object_size;
    }

    status = true;
    return true;
}

bool silly_rwdb_river::insert()
{
    bool status = false;

    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, m_rivers.size());
    while (bi < m_rivers.size())
    {
        if (!otl.insert(m_insert_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = m_rivers[i];
                    otl_value<std::string> stcd(entry.stcd);
                    otl_datetime tm = otl_from_timestamp(entry.stamp);
                    otl_value<double> zz(entry.zz);
                    otl_value<double> qq(entry.qq);
                    otl_value<std::string> wptn(entry.wptn);

                    otl_write_row(*stream, stcd, tm, zz, qq, wptn);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return status;
        }
        // SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, m_rivers.size());
    }

    SLOG_INFO("导入数量: {}", m_rivers.size());

    status = true;
    return true;
}
