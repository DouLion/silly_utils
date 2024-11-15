
#include "silly_rwdb_stbprp.h"
#include "files/silly_file.h"
#include "tools.h"

// 定义静态成员变量

std::unordered_map<std::string, unsigned int> silly_rwdb_stbprp::m_stcd_index;
std::unordered_map<unsigned int, std::string> silly_rwdb_stbprp::m_index_stcd;


bool silly_rwdb_stbprp::output()
{
    bool status = false;
    // 数据库查询 STBPRP
    if (!loads())
    {
        SLOG_ERROR("STBPRP 数据库加载失败");
        return status;
    }

    // 生成 stcd对应的index 关系
    if (!createStcdIndex(m_stbprps, m_stcd_index))
    {
        SLOG_ERROR("STBPRP stcd index 对应关系生成失败");
        return status;
    }

    // 判断是否需要转码
    if (!src_encode.empty() && !dst_encode.empty())
    {
        encode(m_stbprps, src_encode, dst_encode);
    }

    // 序列化 stbprps 为二进制文件
    std::vector<std::string> datas;
    if (!serialize(datas))
    {
        SLOG_ERROR("STBPRP 序列化失败");
        return status;
    }

    // 写入文件
    std::string fileName = m_str_now_tm + "_" + silly_stbprp::FILE_NAME;
    if (!saveInfo(fileName, datas))
    {
        SLOG_ERROR("STBPRP 写入文件失败: {}", fileName);
        return status;
    }
    SLOG_INFO("STBPRP 写入文件成功: {}", fileName);

    status = true;
    return status;
}

bool silly_rwdb_stbprp::import()
{
    bool status = false;
    //  一次性全部读取出stbprp文件中的数据,并反序列化文件字符串中的所有数据
    std::string content;
    if (!dumps(content))
    {
        SLOG_ERROR("读取stbprp文件失败: {}", m_file_path);
        return status;
    }

    // 反序列化stbprp文件中的全部数据
    if (!deserialize(content))
    {
        SLOG_ERROR("读取stbprp文件失败");
        return status;
    }

    // 创建 index 和 stcd 的映射
    if (!creatIndexStcd(m_stbprps, m_index_stcd))
    {
        SLOG_ERROR("创建index和stcd的映射失败");
        return status;
    }

    if (!src_encode.empty() && !dst_encode.empty())
    {
        encode(m_stbprps, src_encode, dst_encode);
    }

    status = true;
    return status;
}

bool silly_rwdb_stbprp::loads()
{
    bool status = false;
    // ---------查询数据库-----------
    if (!otl.select(m_select_sql, [&](otl_stream* stream) {
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

                // silly_rwdb_stbprp::stbprps.push_back(temp);  // 通过类名访问静态成员
                m_stbprps.push_back(temp);
            }
        }))
    {
        SLOG_ERROR(otl.err());
        // SLOG_ERROR(silly_encode::gbk_utf8(otl.err()));
        m_stbprps.clear();
        return status;
    }

    SLOG_INFO("查询到 stbprp 数据:{} 条数据开始导出到文件", m_stbprps.size());
    return true;
}

bool silly_rwdb_stbprp::dumps(std::string& content)
{
    bool status = false;
    std::string file_path = m_file_path;
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
    size_t r = silly_file::read(file_path, content, 0, fsize);
    if (r != fsize)
    {
        SLOG_ERROR("文件大小:{}, 读取到的字节数:{},两者不符", fsize, r);
        return status;
    }
    SLOG_INFO("解析文件完成:{}", file_path);
    return true;
}

bool silly_rwdb_stbprp::serialize(std::vector<std::string>& datas)
{
    for (const auto& stbprp : m_stbprps)
    {
        datas.push_back(stbprp.serialize());
    }
    return true;
}

bool silly_rwdb_stbprp::deserialize(const std::string& content)
{
    m_stbprps.clear();
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
            m_stbprps.push_back(stbprp);
        }
        pos += blockLength;
    }
    SLOG_INFO("解析出 stbprp 数据:{} 条数据", m_stbprps.size());
    status = true;
    return true;
}

bool silly_rwdb_stbprp::insert()
{
    bool status = false;

    int bi = 0, ei = 0;
    int step = 1000;                      // 每批次插入1000条记录
    ei = SU_MIN(step, m_stbprps.size());  // 初始批次结束位置

    while (bi < m_stbprps.size())
    {
        // 使用 OTL 执行批量插入
        if (!otl.insert(m_insert_sql, [&](otl_stream* stream) {
                for (int i = bi; i < ei; i++)
                {
                    auto entry = m_stbprps[i];

                    // 将每条记录的字段封装成 OTL 值
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

                    // 插入一条记录
                    otl_write_row(*stream,
                                  STCD,
                                  STNM /*, RVNM, HNNM, BSNM*/,
                                  LGTD,
                                  LTTD /*, STLC, ADDVCD, DTMNM, DTMEL, DTPR, STTP, FRGRD, ESSTYM, BGFRYM, ATCUNIT, ADMAUTH, LOCALITY, STBK, STAzt, DSTRVM, DRNA, PHCD, USFL, COMMENTS, MODITIME, HNNM0, ADCD, ADDVCD1*/);
                }
            }))
        {
            SLOG_ERROR(otl.err());
            return status;
        }

        // 更新批次的开始和结束位置
        SLOG_INFO("插入第{} - {} 条记录", bi, ei);
        bi = ei;
        ei = SU_MIN(ei + step, m_stbprps.size());
    }

    SLOG_INFO("{} 导入完成, 导入数量: {}", m_file_path, m_stbprps.size());
    status = true;
    return true;
}
