/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: Table.h
 * @description: Table 类声明
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#ifndef SILLY_UTILS_TABLE_H
#define SILLY_UTILS_TABLE_H
#include <def/ColDesc.h>
#include <def/Row.h>
#include <json/silly_jsonpp.h>
#include <files/silly_file.h>

namespace X
{

class Table
{
    class Info
    {
      public:
        std::string sql;
        std::string db;
        size_t rows=0;
        size_t cols=0;
        uint32_t  crc32 = 0;

    };

  public:

    bool Connect(const std::string& otlCfg);

    bool Connect(const silly_otl& otl);

    void Pull(const std::string& sql);

    bool Read(otl_stream* stream);

    void Print(const uint64_t& num =10);

    bool Write(const std::string& file);

    Json::Value Jsonify();

  private:
    void ReadColDesc(otl_stream* stream);
    void ReadRowData(otl_stream* stream);

    bool WriteHeader(std::string& file);

    bool WriteRowData(std::string& file);

    bool Write(const std::string& file, std::stringstream & ss);

    bool Write(const std::vector<Row>& tmp);

  public:
    std::string name;
    std::vector<ColDesc> cols;
    std::vector<Row> rows;
    bool crc32 = false;
    silly_otl m_otl;
    Info m_info;
    std::stringstream m_ss;
    std::mutex m_rw_mutex;
};

}  // namespace X

#endif  // SILLY_UTILS_TABLE_H
