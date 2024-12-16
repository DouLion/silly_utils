/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-16
 * @file: ColType.h
 * @description: ColType 类声明
 * @version: v1.0.1 2024-12-16 dou li yang
 */
#ifndef SILLY_UTILS_COLTYPE_H
#define SILLY_UTILS_COLTYPE_H

#include <database/otl/silly_otl.h>
namespace X
{

typedef otl_var_enum ColType;

static std::string OtlTypeToStr(const otl_var_enum &type)
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

static const std::map<otl_var_enum, int> Otl2MySQLType = {

};

static const std::map<otl_var_enum, int> Otl2DMType = {

};

static const std::map<otl_var_enum, int> Otl2OracleType = {

};

static const std::map<otl_var_enum, int> Otl2MSSQLType = {

};

}  // namespace X

#endif  // SILLY_UTILS_COLTYPE_H
