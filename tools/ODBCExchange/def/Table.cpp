/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: Table.cpp
 * @description: Table实现
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#include "Table.h"

using namespace X;
void Table::Print(const uint64_t &num)
{
    int realNum = SU_MIN(20, std::min(num, rows.size()));

    // 打印列信息
    for (auto &col : cols)
    {
        std::cout << col.name << ": " << silly_otl::otl_type_name(col.type) << std::endl;
    }

    std::cout << "输出" << realNum << "行数据" << std::endl;
    int i = 0;
    for (auto &row : rows)
    {
        if (i >= realNum)
        {
            break;
        }
        std::cout << "-----------------------";
        std::cout << i + 1;
        std::cout << "-----------------------" << std::endl;
        for (auto &col : row)
        {
            switch (col.type)
            {
                case otl_var_char:
                    std::cout << col.str << " ";
                    break;
                case otl_var_timestamp:
                    std::cout << col.dt.StrFTime() << " ";
                    break;
                case otl_var_double:
                case otl_var_float:
                    std::cout << col.d << " ";
                    break;
                case otl_var_varchar_long:
                case otl_var_raw_long:
                    std::cout << col.str << " ";
                    break;
                case otl_var_int:
                    std::cout << col.i32 << " ";
                    break;
                case otl_var_blob:
                    std::cout << "blob" << " ";
                    break;
            }
        }
        std::cout << std::endl;
        i++;
    }
}

bool Table::Read(otl_stream *stream)
{
    ReadColDesc(stream);
    ReadRowData(stream);
    return true;
}
void Table::ReadColDesc(otl_stream *stream)
{
    int col_num = 0;
    otl_column_desc *desc_list = (*stream).describe_select(col_num);
    std::cout << "列数: " << col_num << std::endl;
    cols.resize(col_num);
    for (int i = 0; i < col_num; ++i)
    {
        ColDesc desc;
        cols[i].index = i + 1;
        cols[i].name = desc_list[i].name;
        cols[i].type = (ColType)desc_list[i].otl_var_dbtype;
        std::cout << "[" << cols[i].index << "] ";
        std::cout << "列名: " << desc_list[i].name;
        std::cout << "  类型: " << OtlTypeToStr((otl_var_enum)desc_list[i].otl_var_dbtype) << std::endl;
    }
}
void Table::ReadRowData(otl_stream *stream)
{
    otl_value<int32_t> i32;
    otl_value<int16_t> i16;
    otl_value<uint32_t> u32;
    otl_value<int64_t> i64;
    otl_value<double> p1;
    otl_value<std::string> str;
    otl_long_string olstring;
    otl_lob_stream olstream;
    otl_value<otl_datetime> dt;
    ColVal cv;
    int colNum = cols.size();
    std::vector<ColVal> row(colNum);
    for (auto &s : *stream)
    {
        for (int i = 0; i < colNum; ++i)
        {
            switch (cols[i].type)
            {
                case otl_var_int:
                    s >> i32;
                    cv.Int32(i32);
                    row[i] = cv;
                    break;
                case otl_var_unsigned_int:
                    s >> u32;
                    cv.UInt32(u32);
                    row[i] = cv;
                    break;
                case otl_var_short:
                    s >> i16;
                    cv.Int16(i16);
                    row[i] = cv;
                    break;
                case otl_var_long_int:
                    s >> i64;
                    cv.Int64(i64);
                    row[i] = cv;
                    break;
                case otl_var_double:
                case otl_var_float:
                    s >> p1;
                    cv.Double(p1);
                    row[i] = cv;
                    break;
                case otl_var_char:
                    s >> str;
                    cv.Str(str);
                    row[i] = cv;
                    break;
                case otl_var_timestamp:
                    s >> dt;
                    cv.Time(dt);
                    row[i] = cv;
                    break;
                case otl_var_varchar_long:
                case otl_var_raw_long:
                    s >> olstring;
                    cv.LStr(olstring);
                    row[i] = cv;
                    break;
                case otl_var_blob:
                    s >> olstream;
                    break;
                default:
                    throw std::runtime_error("未兼容的类型" + OtlTypeToStr(cols[i].type));
            }
        }
        rows.push_back(row);
        row.clear();
        row.resize(colNum);
    }
}

bool Table::Write(const std::string &file)
{
    std::ofstream out(file);

    return false;
}
bool Table::WriteHeader(std::string &file)
{
    std::ofstream out;

    // 覆盖写
    out.open(file, std::ios::out | std::ios::trunc | std::ios::binary);
    return false;
}
bool Table::WriteRowData(std::string &file)
{
    const static int64_t LIMIT_SIZE = 10 * SU_MB; // 超过10M就写入文件一次,避免过多占用内存
    int64_t totalSize = 0;
    std::string buff;

    for(auto &row : rows)
    {
        // TODO: 将每一行记录转为二进制, 并且 使用CRC32检验数据
        for(auto &col : row)
        {

        }

        if(buff.size()> LIMIT_SIZE)
        {
            totalSize += buff.size();
            std::ofstream out;
            out.open(file, std::ios::out | std::ios::app | std::ios::binary);
            out.write(buff.c_str(), buff.size());
            buff.clear();
            out.close();
        }
    }

    return totalSize > 0;
}
bool Table::Connect(const std::string &otlCfg)
{
    if(!otl.load(otlCfg))
    {
        SLOG_ERROR(otl.err())
        return false;
    }
    return true;
}
void Table::Pull(const std::string &sql)
{
    otl_connect db;
    otl_stream stream;
    std::string m_err;
    try
    {
        db.auto_commit_off();
        db.set_timeout(5);
        db.rlogon(otl.odbc().c_str(), false);

        stream.open(1, sql.c_str(), db);

        Read(&stream);

        stream.close();
    }
    catch (otl_exception& e)
    {

        db.rollback();
        m_err = "OTL_ERR \nCONN:";
        m_err.append(otl.odbc());
        m_err.append("\nCODE:").append(std::to_string(e.code));
        m_err.append("\nMSG:").append(std::string((char*)e.msg));
        m_err.append("\nSTATE:").append(std::string((char*)e.sqlstate));
        m_err.append("\nSTMT:").append(std::string((char*)e.stm_text));
    }
    catch (std::exception& p)
    {
        db.rollback();
        m_err = "OTL_UNKNOWN " + std::string(p.what());
    }
    stream.close();
    db.logoff();
    if(!m_err.empty())
    {
        SLOG_ERROR(m_err);
    }
}
