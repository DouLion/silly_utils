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
        std::cout << "  类型: " << Table::OtlTypeToStr((otl_var_enum)desc_list[i].otl_var_dbtype) << std::endl;
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
                    throw std::runtime_error("未兼容的类型" + Table::OtlTypeToStr(cols[i].type));
            }
        }
        rows.push_back(row);
        row.clear();
        row.resize(colNum);
    }
}
std::string Table::OtlTypeToStr(const otl_var_enum &type)
{
    std::string result = "Unknown";
    switch (type)
    {
        case otl_var_char:
            result = "char";
            break;
        case otl_var_double:
            result = "double";
            break;
        case otl_var_float:
            result = "float";
            break;
        case otl_var_int:
            result = "int ";
            break;
        case otl_var_unsigned_int:
            result = "unsigned_int";
            break;
        case otl_var_short:
            result = "short";
            break;
        case otl_var_long_int:
            result = "long_int";
            break;
        case otl_var_timestamp:
            result = "timestamp";
            break;
        case otl_var_varchar_long:
            result = "varchar_long";
            break;
        case otl_var_raw_long:
            result = "raw_long";
            break;
        case otl_var_clob:
            result = "clob";
            break;
        case otl_var_blob:
            result = "blob";
            break;
        case otl_var_refcur:
            result = "refcur";
            break;
        case otl_var_long_string:
            result = "long_string";
            break;
        case otl_var_db2time:
            result = "db2time";
            break;
        case otl_var_db2date:
            result = "db2date";
            break;
        case otl_var_tz_timestamp:
            result = "tz_timestamp";
            break;
        case otl_var_ltz_timestamp:
            result = "ltz_timestamp";
            break;
        case otl_var_bigint:
            result = "bigint";
            break;
        case otl_var_raw:
            result = "raw";
            break;
        default:
            throw std::runtime_error("不支持此类型: " + std::to_string(type));
    }
    return result;
}
