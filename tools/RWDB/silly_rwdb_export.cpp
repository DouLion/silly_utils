
#include "silly_rwdb_export.h"
#include "tools.h"

///////////////////////// 公共函数 //////////////////////////////

// 生成 stcd 和 index 的映射
bool createStcdIndex(std::vector<silly_stbprp>& stbprps, std::unordered_map<std::string, uint32_t>& stcdindex);

// 将data中的字符串都写入到文件中,追加的写入
bool saveInfo(const std::string& filename, const std::vector<std::string>& data);

// 根据stcd_index获取stcd对应的index,并写入datas中stcd字段
template <typename T>
std::vector<T>& getIndex(std::vector<T>& datas)
{
    std::vector<T> res_opt;
    for (auto& data : datas)
    {
        std::string stcd = data.STCD;
        if (stcd_index.find(stcd) != stcd_index.end())
        {
            data.index = stcd_index[stcd];
            res_opt.push_back(data);
        }
    }
    return res_opt;
}

///////////////////////// STBPRP //////////////////////////////

bool silly_export_stbprp::exportSTBPRP()
{
    bool status = false;
    // 数据库查询 STBPRP
    std::vector<silly_stbprp> stbprps;
    if (!loadSTBPRP(stbprps))
    {
        SLOG_ERROR("STBPRP 数据库加载失败");
        return status;
    }

    // 生成 stcd对应的index 关系
    if (!createStcdIndex(stbprps, stcd_index))
    {
        SLOG_ERROR("STBPRP stcd index 对应关系生成失败");
        return status;
    }

    // 判断是否需要转码
    if (!src_encode.empty() && !dst_encode.empty())
    {
        encode(stbprps, src_encode, dst_encode);
    }

    // 序列化 stbprps 为二进制文件
    std::vector<std::string> datas;
    if (!serializeSTBPRP(stbprps, datas))
    {
        SLOG_ERROR("STBPRP 序列化失败");
        return status;
    }

    // 写入文件
    std::string fileName = str_now_tm + "_" + silly_stbprp::FILE_NAME;
    if (!saveInfo(fileName, datas))
    {
        SLOG_ERROR("STBPRP 写入文件失败");
        return status;
    }

    status = true;
    return status;
}

bool silly_export_stbprp::loadSTBPRP(std::vector<silly_stbprp>& stbprps)
{
    // ---------查询数据库-----------
    if (!otl.select(select_stbprp_sql, [&stbprps](otl_stream* stream) {
            while (!stream->eof())
            {
                otl_value<std::string> STCD, STNM, RVNM, HNNM, BSNM, STLC, ADDVCD, DTMNM, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, PHCD, USFL, COMMENTS, HNNM0, ADCD, ADDVCD1;
                otl_value<double> LGTD, LTTD, DTMEL, DTPR, DSTRVM;
                otl_value<int> STAzt, DRNA;
                otl_datetime MODITIME;

                otl_read_row(
                    *stream, STCD, STNM /*, RVNM, HNNM, BSNM*/, LGTD, LTTD /*, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1*/);
                silly_stbprp temp;

                // 检查并赋值
                CHECK_NULL_VALUE(STCD, temp.STCD);
                CHECK_NULL_VALUE(STNM, temp.STNM);
                /* if (!RVNM.is_null()) { temp.RVNM = RVNM.v; }
                 if (!HNNM.is_null()) { temp.HNNM = HNNM.v; }
                 if (!BSNM.is_null()) { temp.BSNM = BSNM.v; }*/
                CHECK_NULL_VALUE(LGTD, temp.LGTD);
                CHECK_NULL_VALUE(LTTD, temp.LTTD);
                /* if (!STLC.is_null()) { temp.STLC = STLC.v; }
                if (!ADDVCD.is_null()) { temp.ADDVCD = ADDVCD.v; }
                if (!DTMNM.is_null()) { temp.DTMNM = DTMNM.v; }
                if (!DTMEL.is_null()) { temp.DTMEL = DTMEL.v; }
                if (!DTPR.is_null()) { temp.DTPR = DTPR.v; }
                if (!STTP.is_null()) { temp.STTP = STTP.v; }
                if (!FRGRD.is_null()) { temp.FRGRD = FRGRD.v; }
                if (!ESSTYM.is_null()) { temp.ESSTYM = ESSTYM.v; }
                if (!BGFRYM.is_null()) { temp.BGFRYM = BGFRYM.v; }
                if (!ATCUNIT.is_null()) { temp.ATCUNIT = ATCUNIT.v; }
                if (!ADMAUTH.is_null()) { temp.ADMAUTH = ADMAUTH.v; }
                if (!LOCALITY.is_null()) { temp.LOCALITY = LOCALITY.v; }
                if (!STBK.is_null()) { temp.STBK = STBK.v; }
                if (!STAzt.is_null()) { temp.STAzt = STAzt.v; }
                if (!DSTRVM.is_null()) { temp.DSTRVM = DSTRVM.v; }
                if (!DRNA.is_null()) { temp.DRNA = DRNA.v; }
                if (!PHCD.is_null()) { temp.PHCD = PHCD.v; }
                if (!USFL.is_null()) { temp.USFL = USFL.v; }
                if (!COMMENTS.is_null()) { temp.COMMENTS = COMMENTS.v; }
                temp.MODITIME = otl_tools::otl_time_to_string(MODITIME);
                if (!HNNM0.is_null()) { temp.HNNM0 = HNNM0.v; }
                if (!ADCD.is_null()) { temp.ADCD = ADCD.v; }
                if (!ADDVCD1.is_null()) { temp.ADDVCD1 = ADDVCD1.v; }*/

                stbprps.push_back(temp);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        SLOG_ERROR(silly_encode::gbk_utf8(otl.err()));
        stbprps.clear();
        return false;
    }

    SLOG_INFO("查询到 stbprp 数据:{} 条数据开始导出到文件", stbprps.size());
    return true;
}

bool silly_export_stbprp::serializeSTBPRP(const std::vector<silly_stbprp>& stbprps, std::vector<std::string>& datas)
{
    for (const auto& stbprp : stbprps)
    {
        datas.push_back(stbprp.serialize());
    }
    return true;
}

///////////////////////// PPTN //////////////////////////////

bool silly_export_pptn::exportPPTN(const std::vector<std::pair<std::string, std::string>>& btm_etm)
{
    bool status = false;
    std::string fileName = str_now_tm + "_" + silly_pptn::FILE_NAME;
    for (const auto& [_btm, _etm] : btm_etm)
    {
        // 数据库查询 PPTN
        std::vector<silly_pptn> pptns;
        if (!loadPPTN(_btm, _etm, pptns))
        {
            SLOG_ERROR("PPTN 数据库加载失败 ({} ~ {})", _btm, _etm);
            continue;
        }
        // 根据stcd 获取index
        getIndex(pptns);
        // 序列化 pptns 为二进制文件
        std::vector<std::string> datas;
        if (!serializePPTN(pptns, datas))
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

    status = true;
    return status;
}

bool silly_export_pptn::loadPPTN(const std::string& btm, const std::string& etm, std::vector<silly_pptn>& pptns)
{
    bool status = false;
    std::string sql = silly_format::format(select_pptn_sql, btm, etm);
    if (!otl.select(sql, [&pptns](otl_stream* stream) {
            while (!stream->eof())
            {
                otl_value<std::string> STCD;
                otl_value<double> DRP, INTV;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, DRP, INTV);
                silly_pptn tmp_pptn;
                CHECK_NULL_VALUE(STCD, tmp_pptn.STCD);
                CHECK_NULL_VALUE(DRP, tmp_pptn.drp);
                CHECK_NULL_VALUE(INTV, tmp_pptn.intv);
                tmp_pptn.stamp = otl_to_timestamp(tm);

                pptns.push_back(tmp_pptn);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        pptns.clear();
        return status;
    }
    SLOG_INFO("查询到:{} 条 PPTN 数据", pptns.size());
    status = true;
    return status;
}

bool silly_export_pptn::serializePPTN(std::vector<silly_pptn>& pptn, std::vector<std::string>& datas)
{
    for (const auto& pptn : pptn)
    {
        datas.push_back(pptn.serialize());
    }
    return true;
}

///////////////////////// river //////////////////////////////

bool silly_export_river::exportRiver(const std::vector<std::pair<std::string, std::string>>& btm_etm)
{
    bool status = false;
    std::string fileName = str_now_tm + "_" + silly_river::FILE_NAME;
    for (const auto& [_btm, _etm] : btm_etm)
    {
        // 数据库查询 RIVER
        std::vector<silly_river> rivers;
        if (!loadRiver(_btm, _etm, rivers))
        {
            SLOG_ERROR("RIVER 数据库加载失败 ({} ~ {})", _btm, _etm);
            continue;
        }
        // 根据stcd 获取index
        getIndex(rivers);
        // 序列化 rivers 为二进制文件
        std::vector<std::string> datas;
        if (!serializeRiver(rivers, datas))
        {
            SLOG_ERROR("RIVER 序列化失败  ({} ~ {})", _btm, _etm);
            continue;
        }
        // 写入文件
        if (!saveInfo(fileName, datas))
        {
            SLOG_ERROR("RIVER 写入文件失败  ({} ~ {})", _btm, _etm);
            continue;
        }
    }
    status = true;
    return status;
}

bool silly_export_river::loadRiver(const std::string& btm, const std::string& etm, std::vector<silly_river>& rivers)
{
    bool status = false;
    std::string sql = silly_format::format(select_river_sql, btm, etm);
    if (!otl.select(sql, [&rivers](otl_stream* stream) {
            while (!stream->eof())
            {
                otl_value<std::string> STCD, WPTN;
                otl_value<double> Z, Q;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, Z, Q, WPTN);
                silly_river tmp_river;

                CHECK_NULL_VALUE(STCD, tmp_river.STCD);
                CHECK_NULL_VALUE(Z, tmp_river.zz);
                CHECK_NULL_VALUE(Q, tmp_river.qq);
                CHECK_NULL_VALUE(WPTN, tmp_river.wptn);

                tmp_river.stamp = otl_to_timestamp(tm);

                rivers.push_back(tmp_river);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        rivers.clear();  // 查询失败,返回空
        return status;
    }

    SLOG_INFO("查询到:{} 条 RIVER 数据", rivers.size());
    status = true;
    return status;
}

bool silly_export_river::serializeRiver(std::vector<silly_river>& rivers, std::vector<std::string>& datas)
{
    for (auto& river : rivers)
    {
        datas.push_back(river.serialize(SILLY_RIVER_FORMAT_V2));
    }
    return true;
}

///////////////////////// rsvr //////////////////////////////

bool silly_export_rsvr::exportRsvr(const std::vector<std::pair<std::string, std::string>>& btm_etm)
{
    bool status = false;

    std::string fileName = str_now_tm + "_" + silly_rsvr::FILE_NAME;
    for (const auto& [_btm, _etm] : btm_etm)
    {
        // 数据库查询 RSVR
        std::vector<silly_rsvr> rsvrs;
        if (!loadRsvr(_btm, _etm, rsvrs))
        {
            SLOG_ERROR("RSVR 数据库加载失败 ({} ~ {})", _btm, _etm);
            continue;
        }
        // m 根据stcd 获取index
        getIndex(rsvrs);

        // 序列化 rivers 为二进制文件
        std::vector<std::string> datas;
        if (!serializeRsvr(rsvrs, datas))
        {
            SLOG_ERROR("RSVR 序列化失败  ({} ~ {})", _btm, _etm);
            continue;
        }
        // 写入文件
        if (!saveInfo(fileName, datas))
        {
            SLOG_ERROR("RSVR 写入文件失败  ({} ~ {})", _btm, _etm);
            continue;
        }
    }

    status = true;
    return status;
}

bool silly_export_rsvr::loadRsvr(const std::string& btm, const std::string& etm, std::vector<silly_rsvr>& rsvrs)
{
    bool status = false;
    std::string sql = silly_format::format(select_rsvr_sql, btm, etm);
    if (!otl.select(sql, [&rsvrs](otl_stream* stream) {
            while (!stream->eof())
            {
                silly_rsvr tmp_rsvr;

                otl_value<std::string> STCD, RWCHRCD, RWPTN, MSQMT;
                otl_value<double> RZ, INQ, W, OTQ, INQDR, BLRZ;
                otl_datetime tm;
                otl_read_row(*stream, STCD, tm, RZ, INQ, W, OTQ, RWCHRCD, RWPTN, INQDR, MSQMT, BLRZ);

                CHECK_NULL_VALUE(STCD, tmp_rsvr.STCD);
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

                rsvrs.push_back(tmp_rsvr);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        rsvrs.clear();  // 查询失败,返回空
        return status;
    }

    SLOG_INFO("查询到:{} 条 RIVER 数据", rsvrs.size());
    status = true;
    return status;
}

bool silly_export_rsvr::serializeRsvr(std::vector<silly_rsvr>& rsvrs, std::vector<std::string>& datas)
{
    for (auto& rsvr : rsvrs)
    {
        datas.push_back(rsvr.serialize(SILLY_RSVR_FORMAT_V1));
    }
    return true;
}

///////////////////////////////////////////////////

bool createStcdIndex(std::vector<silly_stbprp>& stbprps, std::unordered_map<std::string, uint32_t>& stcdindex)
{
    for (const auto& stbprp : stbprps)
    {
        stcdindex[stbprp.STCD] = stbprp.index;
    }
    return true;
}

bool saveInfo(const std::string& filename, const std::vector<std::string>& data)
{
    bool status = false;
    std::ofstream file(filename, std::ios::app | std::ios::binary);
    if (!file.is_open())
    {
        SLOG_ERROR("文件打开失败:{}", filename);
        return status;
    }
    for (const auto& str : data)
    {
        file << str;
    }
    file.close();
    status = true;
    return status;
}
