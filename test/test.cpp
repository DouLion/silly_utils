
#include <su.h>
int main(int argc, char** argv)
{
    SLOG_MAIN

    silly_otl conn;
    for (const auto& driver : conn.drivers())
    {
        SLOG_INFO("可用 驱动: {}", driver)
    }
    #if _WIN32
    std::string cfg = R"(
    {
        "type": "mysql",
        "ip": "192.168.0.154",
        "port": 3306,
        "schema": "RWDB_XJ",
        "user": "root",
        "password": "3edc9ijn~",
        "driver": "MariaDB ODBC 3.1 Driver"
    }
    )";
    #else
    std::string cfg = R"(
    {
        "type": "mysql",
        "ip": "192.168.0.154",
        "port": 3306,
        "schema": "RWDB_XJ",
        "user": "root",
        "password": "3edc9ijn~",
        "driver": "MySQL ODBC 8.0 Unicode Driver"
    })";
    #endif

    // 连接数据库
    if (!conn.load(cfg))
    {
        SLOG_INFO(conn.err());
    }
    if (conn.check())
    {
        SLOG_INFO(conn.err());
    }
    SLOG_INFO(conn.odbc());

    std::string sql1 = "select ADCD, ADNM, LGTD, LTTD from TZX_FloodDisaster_XJ.AD_CD_B limit 10";

    if (!conn.select(sql1, [](otl_stream* stream) {
            while (!stream->eof())
            {
                std::string stcd, stnm;
                double LGTD, LTTD;
                otl_read_row(*stream, stcd, stnm, LGTD, LTTD);

                // std::cout << stcd << " " << stnm << " " << stnm << ", " << stnm << std::endl;
                SLOG_INFO("{}, {}, {}, {}", stcd, stnm, LGTD, LTTD)
            }
        }))
    {
        SLOG_ERROR(conn.err())
    }

    return 0;
}