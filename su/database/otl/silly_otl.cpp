//
// Created by dell on 2023/12/21.
//

#include "silly_otl.h"
#include "su_marco.h"

bool otl_conn_opt::load(const std::string &s_opt) {
    if(s_opt.empty()) return false;
    conn =s_opt;
    return true;
}

std::string otl_conn_opt::dump_odbc(const bool& rebuild) {
    if (conn.empty() || rebuild)
    {
        if (!dsn.empty()) // 如果ODBC连接串不好使,设置DSN,并且优先使用DSN链接方式
        {
            conn = "UID=" + user + ";PWD=" + password + ";DSN=" + dsn + ";";
            return conn;
        }
        switch (type) // 使用ODBC连接串
        {
            case enum_database_type::dbMYSQL:
                conn =
                        "Driver={" + driver + "};Server=" + ip + ";Port=" + std::to_string(port) + ";Database=" +
                        schema + ";User=" + user + ";Password=" + password + ";Option=3;charset=UTF8;";
                break;
            case enum_database_type::dbSQLSERVER:
                conn =
                        "Driver={" + driver + "};Server=" + ip + ";Port:" + std::to_string(port) + ";Database=" +
                        schema + ";UID=" + user + ";PWD=" + password + ";";
                break;
            case enum_database_type::dbORACLE:
                conn = "Driver={" + driver + "};DBQ=" + ip + ":" + std::to_string(port) + "/" + schema +
                                ";Uid=" + user + ";Pwd=" + password + ";";
                break;
            default:
                break;
        }
    }
    return conn;

}

void otl_conn_opt::help() {
    SU_INFO_PRINT("OTL 连接串帮助信息:\nSQL Server:\n\tDRIVER={驱动名称};SERVER=IP;PORT=端口;UID=账号;PWD=密码;DATABASE=数据库;\nMySQL:\n\tDriver={MySQL ODBC 8.0 ANSI Driver};Server=IP;Port=端口;Database=数据库;User=账号;Password=密码;Option=3;charset=UTF8;\nOracle:\n\tDriver={ODBC驱动名称};DBQ=IP:端口/表空间名称;UID=用户;PWD=密码;\n达梦(DM8):\n\t达梦数据库目前只能使用DSN的方式  UID=用户;PWD=密码;DSN=DSN名称;\n")
}
