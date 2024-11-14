
#include "silly_rwdb_river.h"
#include "files/silly_file.h"
#include "tools.h"

std::vector<silly_river> silly_rwdb_river::m_rivers;

std::string silly_rwdb_river::m_str_now_tm;
std::string silly_rwdb_river::m_select_river_sql;
std::string silly_rwdb_river::m_insert_river_sql;
std::string silly_rwdb_river::m_river_file_path;

bool silly_rwdb_river::output(const std::vector<std::pair<std::string, std::string>>& btm_etm)
{
    bool status = false;
    std::string fileName = m_str_now_tm + "_" + silly_river::FILE_NAME;
    for (const auto& [_btm, _etm] : btm_etm)
    {
        // 数据库查询 PPTN
        m_rivers.clear();
        if (!loads(_btm, _etm))
        {
            SLOG_ERROR("PPTN 数据库加载失败 ({} ~ {})", _btm, _etm);
            continue;
        }
        // 根据stcd 获取index
        m_rivers = getIndex(m_rivers);
        // 序列化 rivers 为二进制文件
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
        SLOG_INFO("river 写入文件成功: {}", fileName);
    }

    status = true;
    return status;
}

bool silly_rwdb_river::import(const size_t block_size)
{
    bool status = false;
    size_t fsize = silly_file::size(m_river_file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(m_river_file_path, block_data, pos, block_size);
        m_rivers.clear();
        int residue_size = 0;
        if (!deserialize(block_data, residue_size))
        {
            SLOG_ERROR("反序列化river失败");
            return status;
        }
        // 获取river中的stcd
        m_rivers = getStcd(m_rivers);
        // 插入river到数据库
        if (!insert())
        {
            SLOG_ERROR("导入river失败");
            return status;
        }
        pos = pos + rsize - residue_size;
    }

    status = true;
    return status;
}

bool silly_rwdb_river::loads(const std::string& btm, const std::string& etm)
{
    bool status = false;
    std::string sql = silly_format::format(m_select_river_sql, btm, etm);
    if (!otl.select(sql, [&](otl_stream* stream) {
            while (!stream->eof())
            {
                otl_value<std::string> STCD, WPTN;
                otl_value<double> Z, Q;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, Z, Q, WPTN);
                silly_river tmp_river;

                CHECK_NULL_VALUE(STCD, tmp_river.stcd);
                CHECK_NULL_VALUE(Z, tmp_river.zz);
                CHECK_NULL_VALUE(Q, tmp_river.qq);
                CHECK_NULL_VALUE(WPTN, tmp_river.wptn);

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
        if (!otl.insert(m_insert_river_sql, [&](otl_stream* stream) {
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
