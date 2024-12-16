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
#include <def/ColVal.h>

namespace X
{

typedef std::vector<ColVal> Row;
class Table
{
  public:


    bool Read(otl_stream* stream);

    void Print(const uint64_t& num =10);

    static std::string OtlTypeToStr(const otl_var_enum& type);

  private:
    void ReadColDesc(otl_stream* stream);
    void ReadRowData(otl_stream* stream);

  public:
    std::string name;
    std::vector<ColDesc> cols;
    std::vector<Row> rows;
    bool crc32 = false;
    silly_otl otl;



};

}  // namespace X

#endif  // SILLY_UTILS_TABLE_H
