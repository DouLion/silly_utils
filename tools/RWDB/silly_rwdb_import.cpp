
#include "silly_rwdb_import.h"
#include "tools.h"
#include "files/silly_file.h"

///////////////////////// 公共函数 //////////////////////////////

// 创建 index 和 stcd 的映射
bool creatIndexStcd(const std::vector<silly_stbprp>& stbprps, std::unordered_map<uint32_t, std::string>& indexstcd);

// 模板函数根据index查找stcd
template <typename T>
std::vector<T> getStcd(std::vector<T>& dst_objects)
{
    // 根据 index_stcd 查找并更新 stcd
    std::vector<T> res_objects;
    for (auto& obj : dst_objects)
    {
        uint32_t t_index = obj.index;
        if (index_stcd.find(t_index) != index_stcd.end())
        {
            std::string t_stcd = index_stcd[t_index];
            if (!t_stcd.empty())
            {
                obj.STCD = t_stcd;
                res_objects.push_back(obj);
            }
        }
    }
    return res_objects;
}

///////////////////////// STBPRP //////////////////////////////

bool silly_import_stbprp::importSTBPRP(const std::string& file_path)
{
    bool status = false;
    //  一次性全部读取出stbprp文件中的数据,并反序列化文件字符串中的所有数据
    std::string content;
    if (!loadSTBPRP(file_path, content))
    {
        SLOG_ERROR("读取stbprp文件失败: {}", file_path);
        return status;
    }

    // 反序列化stbprp文件中的全部数据
    std::vector<silly_stbprp> stbprps;
    if (!deserializeSTBPRP(content, stbprps))
    {
        SLOG_ERROR("读取stbprp文件失败");
        return false;
    }

    // 创建 index 和 stcd 的映射
    if (!creatIndexStcd(stbprps, index_stcd))
    {
        SLOG_ERROR("创建index和stcd的映射失败");
        return false;
    }

    if (!src_encode.empty() && !dst_encode.empty())
    {
        encode(stbprps, src_encode, dst_encode);
    }

    status = true;
    return status;
}

bool silly_import_stbprp::loadSTBPRP(const std::string& file_path, std::string& content)
{
    bool status = false;
    if (!std::filesystem::exists(file_path))
    {
        SLOG_ERROR("文件不存在: {}", file_path);
        return status;
    }
    size_t fsize = silly_file::size(file_path);
    if (fsize < 4)
    {
        SLOG_ERROR("文件大小小于 4 字节: {}", fsize);
        return status;
    }
    // 一次性读取
    size_t r = silly_file::read(stbprp_file_path, content, 0, fsize);
    if (r != fsize)
    {
        SLOG_ERROR("文件大小:{}, 读取到的字节数:{},两者不符", fsize, r);
        return status;
    }
    return true;
}

bool silly_import_stbprp::deserializeSTBPRP(const std::string& content, std::vector<silly_stbprp>& stbprps)
{
    bool status = false;
    // 逐块解析
    size_t pos = 0;
    while (pos < content.size())
    {
        uint32_t blockLength = 0;  // 读取块长度
        memcpy(&blockLength, content.data() + pos, 4);
        if (pos + blockLength > content.size())  // 检查是否越界
        {
            break;
        }
        std::string block = content.substr(pos, blockLength);
        silly_stbprp stbprp;
        if (stbprp.deserialize(block))  // 反序列化解析文件中的数据
        {
            stbprps.push_back(stbprp);
        }
        pos += blockLength;
    }

    status = true;
    return true;
}

bool silly_import_stbprp::insertSTBPRP(std::vector<silly_stbprp>& stbprps)
{
    //  --------------插入数据库--------------
    if (!otl.insert(insert_stbprp_sql, [&stbprps](otl_stream* stream) {
            for (const auto& entry : stbprps)
            {
                otl_value<std::string> STCD(entry.STCD);
                otl_value<std::string> STNM(entry.STNM);
                // otl_value<std::string> RVNM(entry.RVNM);
                // otl_value<std::string> HNNM(entry.HNNM);
                // otl_value<std::string> BSNM(entry.BSNM);
                otl_value<double> LGTD(entry.LGTD);
                otl_value<double> LTTD(entry.LTTD);
                // otl_value<std::string> STLC(entry.STLC);
                // otl_value<std::string> ADDVCD(entry.ADDVCD);
                // otl_value<std::string> DTMNM(entry.DTMNM);
                // otl_value<double> DTMEL(entry.DTMEL);
                // otl_value<double> DTPR(entry.DTPR);
                // otl_value<std::string> STTP(entry.STTP);
                // otl_value<std::string> FRGRD(entry.FRGRD);
                // otl_value<std::string> ESSTYM(entry.ESSTYM);
                // otl_value<std::string> BGFRYM(entry.BGFRYM);
                // otl_value<std::string> ATCUNIT(entry.ATCUNIT);
                // otl_value<std::string> ADMAUTH(entry.ADMAUTH);
                // otl_value<std::string> LOCALITY(entry.LOCALITY);
                // otl_value<std::string> STBK(entry.STBK);
                // otl_value<int> STAzt(entry.STAzt);
                // otl_value<double> DSTRVM(entry.DSTRVM);
                // otl_value<int> DRNA(entry.DRNA);
                // otl_value<std::string> PHCD(entry.PHCD);
                // otl_value<std::string> USFL(entry.USFL);
                // otl_value<std::string> COMMENTS(entry.COMMENTS);
                // otl_datetime MODITIME = otl_tools::otl_time_from_string(entry.MODITIME);
                // otl_value<std::string> HNNM0(entry.HNNM0);
                // otl_value<std::string> ADCD(entry.ADCD);
                // otl_value<std::string> ADDVCD1(entry.ADDVCD1);

                otl_write_row(
                    *stream, STCD, STNM /*, RVNM, HNNM, BSNM*/, LGTD, LTTD /*, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1*/);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        return false;
    }

    SLOG_INFO("{} 导入完成, 导入数量: {}", stbprp_file_path, stbprps.size());
    return true;
}

///////////////////////// PPTN //////////////////////////////

bool silly_import_pptn::importPPTN(const std::string& file_path, const size_t block_size)
{
    bool status = false;
    size_t fsize = silly_file::size(file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(file_path, block_data, pos, block_size);
        std::vector<silly_pptn> pptns;
        int residue_size = 0;
        if (deserializePPTN(block_data, pptns, residue_size))
        {
            SLOG_ERROR("反序列化pptn失败");
            return status;
        }
        // 获取pptn中的stcd
        getStcd(pptns);
        // 插入pptn到数据库
        if (!insertPPTN(pptns))
        {
            SLOG_ERROR("导入pptn失败");
            return status;
        }
        pos = pos + rsize - residue_size;
    }

    status = true;
    return status;
}

bool silly_import_pptn::deserializePPTN(const std::string& block_data, std::vector<silly_pptn>& pptns, int& residue_size)
{
    bool status = false;
    std::vector<silly_pptn> dst_pptns;
    unsigned int object_size = silly_pptn::SIZE_V1;

    size_t currentPos = 0;
    while (currentPos < block_data.size())
    {
        if (currentPos + object_size > block_data.size())
        {
            std::string leftoverData = block_data.substr(currentPos);  // 该块不完整的数据
            residue_size = block_data.size() - currentPos;
            break;
        }
        std::string once_opt_str = block_data.substr(currentPos, object_size);
        silly_pptn pptn;
        if (pptn.deserialize(once_opt_str))
        {
            pptns.push_back(pptn);
        }
        currentPos += object_size;
    }

    status = true;
    return true;
}

bool silly_import_pptn::insertPPTN(std::vector<silly_pptn>& pptns)
{
    bool status = false;

    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, pptns.size());
    while (bi < pptns.size())
    {
        if (!otl.insert(insert_pptn_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = pptns[i];

                    otl_value<std::string> stcd(entry.STCD.c_str());
                    otl_datetime tm = otl_from_timestamp(entry.stamp);
                    otl_value<double> intv(entry.intv);
                    otl_value<double> drp(entry.drp);

                    otl_write_row(*stream, stcd, tm, drp, intv);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return false;
        }
        // SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, pptns.size());
    }

    SLOG_INFO("导入数量: {}", pptns.size());

    status = true;
    return true;
}

///////////////////////// river //////////////////////////////

bool silly_import_river::importRiver(const std::string& file_path, const size_t block_size)
{
    bool status = false;
    size_t all_size = 0;
    size_t fsize = silly_file::size(file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(file_path, block_data, pos, block_size);
        std::vector<silly_river> rivers;
        int residue_size = 0;
        if (deserializeRiver(block_data, rivers, residue_size))
        {
            SLOG_ERROR("反序列化river失败");
            return status;
        }

        // 根据index获取stcd
        getStcd(rivers);

        // 插入数据库
        if (!insertRiver(rivers))
        {
            SLOG_ERROR("导入river失败");
            return status;
        }
        pos = pos + rsize - residue_size;
    }
    status = true;
    return status;
}

bool silly_import_river::deserializeRiver(const std::string& block_data, std::vector<silly_river>& rivers, int& residue_size)
{
    bool status = false;
    unsigned int object_size = silly_river::SIZE_V2;

    size_t currentPos = 0;
    while (currentPos < block_data.size())
    {
        if (currentPos + object_size > block_data.size())
        {
            std::string leftoverData = block_data.substr(currentPos);  // 该块不完整的数据
            residue_size = block_data.size() - currentPos;
            break;
        }
        std::string once_opt_str = block_data.substr(currentPos, object_size);
        silly_river river;
        if (river.deserialize(once_opt_str))
        {
            rivers.push_back(river);
        }
        currentPos += object_size;
    }

    status = true;
    return true;
}

bool silly_import_river::insertRiver(std::vector<silly_river>& rivers)
{
    bool status = false;

    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, rivers.size());
    while (bi < rivers.size())
    {
        if (!otl.insert(insert_river_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = rivers[i];
                    otl_value<std::string> stcd(entry.STCD);
                    otl_datetime tm = otl_from_timestamp(entry.stamp);
                    otl_value<double> zz(entry.zz);
                    otl_value<double> qq(entry.qq);
                    otl_value<std::string> wptn(entry.wptn);

                    otl_write_row(*stream, stcd, tm, zz, qq, wptn);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return false;
        }
        // SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, rivers.size());
    }

    SLOG_INFO("导入数量: {}", rivers.size());

    status = true;
    return true;
}

///////////////////////// rsvr //////////////////////////////

bool silly_import_rsvr::importRsvr(const std::string& file_path, const size_t block_size)
{
    bool status = false;
    size_t fsize = silly_file::size(file_path);
    for (size_t pos = 0; pos < fsize;)
    {
        std::string block_data;
        size_t rsize = silly_file::read(file_path, block_data, pos, block_size);
        std::vector<silly_rsvr> rsvrs;
        int residue_size = 0;
        if (deserializeRsvr(block_data, rsvrs, residue_size))
        {
            SLOG_ERROR("反序列化rsvr失败");
            return status;
        }

        // 根据index获取stcd
        getStcd(rsvrs);

        // 插入数据库
        if (!insertRsvr(rsvrs))
        {
            SLOG_ERROR("导入rsvr失败");
            return status;
        }

        pos = pos + rsize - residue_size;
    }
    status = true;
    return status;
}

bool silly_import_rsvr::deserializeRsvr(const std::string& block_data, std::vector<silly_rsvr>& rsvrs, int& residue_size)
{
    bool status = false;
    unsigned int object_size = silly_rsvr::SIZE_V1;

    size_t currentPos = 0;
    while (currentPos < block_data.size())
    {
        if (currentPos + object_size > block_data.size())
        {
            std::string leftoverData = block_data.substr(currentPos);  // 该块不完整的数据
            residue_size = block_data.size() - currentPos;
            break;
        }
        std::string once_opt_str = block_data.substr(currentPos, object_size);
        silly_rsvr rsvr;
        if (rsvr.deserialize(once_opt_str))
        {
            rsvrs.push_back(rsvr);
        }
        currentPos += object_size;
    }
    status = true;
    return status;
}

bool silly_import_rsvr::insertRsvr(std::vector<silly_rsvr>& rsvrs)
{
    bool status = false;

    int bi = 0, ei = 0;
    int step = 5000;
    ei = SU_MIN(step, rsvrs.size());
    while (bi < rsvrs.size())
    {
        if (!otl.insert(insert_rsvr_sql, [&](otl_stream* stream) {
                int count = 0;
                for (int i = bi; i < ei; i++)
                {
                    auto entry = rsvrs[i];

                    otl_value<std::string> STCD(entry.STCD);
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
        ei = SU_MIN(ei + step, rsvrs.size());
    }

    SLOG_INFO("导入数量: {}", rsvrs.size());

    status = true;
    return status;
}

///////////////////////////////////////////////////

bool creatIndexStcd(const std::vector<silly_stbprp>& stbprps, std::unordered_map<uint32_t, std::string>& indexstcd)
{
    // 根据 stbprps 生成说有 stcd 和 index 的映射
    for (const auto& stbprp : stbprps)
    {
        indexstcd[stbprp.index] = stbprp.STCD;
    }
    return true;
}
