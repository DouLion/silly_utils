//
// Created by dly on 2023/5/17.
//

#include "otl_tools.h"
#include <datetime/silly_posix_time.h>

otl_conn_opt otl_tools::conn_opt_from_json(const Json::Value &root)
{
    otl_conn_opt oco_ret_opt;
    if (root.isMember(SILLY_OTL_OPT_S_TYPE))
    {
        oco_ret_opt.m_type = str_to_db_type(root[SILLY_OTL_OPT_S_TYPE].asString());
    }
    else
    {
        return oco_ret_opt;
    }

    if (root.isMember(SILLY_OTL_OPT_S_IP))
    {
        oco_ret_opt.m_ip = root[SILLY_OTL_OPT_S_IP].asString();
    }

    if (root.isMember(SILLY_OTL_OPT_S_PORT))
    {
        oco_ret_opt.m_port = root[SILLY_OTL_OPT_S_PORT].asInt();
    }

    if (root.isMember(SILLY_OTL_OPT_S_DRIVER))
    {
        oco_ret_opt.m_driver = root[SILLY_OTL_OPT_S_DRIVER].asString();
    }

    if (root.isMember(SILLY_OTL_OPT_S_SCHEMA))
    {
        oco_ret_opt.m_schema = root[SILLY_OTL_OPT_S_SCHEMA].asString();
    }

    if (root.isMember(SILLY_OTL_OPT_S_USER))
    {
        oco_ret_opt.m_user = root[SILLY_OTL_OPT_S_USER].asString();
    }
    else
    {
        return oco_ret_opt;
    }
    if (root.isMember(SILLY_OTL_OPT_S_PASSWORD))
    {
        oco_ret_opt.m_password = root[SILLY_OTL_OPT_S_PASSWORD].asString();
    }
    else
    {
        return oco_ret_opt;
    }

    if (root.isMember(SILLY_OTL_OPT_S_DSN))
    {
        oco_ret_opt.m_dsn = root[SILLY_OTL_OPT_S_DSN].asString();
    }

    return oco_ret_opt;
}

otl_datetime otl_tools::otl_time_from_string(const std::string &str)
{
    otl_datetime odt;
    silly_posix_time spt;
    spt = spt.from_string(str);
    odt.year = spt.year();
    odt.month = spt.month();
    odt.day = spt.day();
    odt.hour = spt.hour();
    odt.minute = spt.minute();
    odt.second = spt.second();
    odt.fraction = 0;
    return odt;
}

std::string otl_tools::otl_time_to_string(const otl_datetime &tm)
{
    char datetime_buff[32] = {};
    sprintf(datetime_buff, "%04d-%02d-%02d %02d:%02d:%02d", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);

    return datetime_buff;
}

otl_conn_opt otl_tools::conn_opt_from_json(const std::string &json_str)
{
    otl_conn_opt ret_opt;
    Json::Reader reader;
    Json::Value root;
    if (reader.parse(json_str, root))
    {
        ret_opt = otl_tools::conn_opt_from_json(root);
    }
    return ret_opt;
}

#ifdef IS_WIN32
#include <odbcinst.h>
#include <tchar.h>
#include <cstring>
#include "encode/silly_encode.h"
#define SILLY_OTL_TOOLS_DRIVER_BUFF_LEN 10240
#pragma comment(lib, "odbccp32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

#endif

std::vector<std::string> otl_tools::get_local_odbc_drivers()
{
    {
        std::vector<std::string> vs_drivers;
#ifdef IS_WIN32
        WCHAR *szBuf = new WCHAR[SILLY_OTL_TOOLS_DRIVER_BUFF_LEN];
        memset(szBuf, 0, SILLY_OTL_TOOLS_DRIVER_BUFF_LEN * sizeof(WCHAR));
        WORD cbBufMax = SILLY_OTL_TOOLS_DRIVER_BUFF_LEN - 1;
        WORD cbBufOut;
        WCHAR *pszBuf = szBuf;
        if (SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
        {
            do
            {
                pszBuf = wcschr(pszBuf, '\0') + 1;
                vs_drivers.push_back(silly_encode::cxx11_wstring_string(pszBuf));
            } while (pszBuf[1] != '\0');
        }
        delete[] szBuf;
#else
        FILE *fp;
        char buffer[4096];
        fp = popen("odbcinst -q -d", "r");
        while (nullptr != fgets(buffer, 4096, fp))
        {
            // printf("%s", buffer);
            std::string tmp_odbc_driver(buffer);
            tmp_odbc_driver = tmp_odbc_driver.substr(1, tmp_odbc_driver.size() - 3);  // 每一行的结果 [MySQL ODBC 8.0 Unicode Driver]\r    最后有个换行符,所以是 -3
            vs_drivers.push_back(tmp_odbc_driver);
            memset(buffer, 0, 4096);
        }

        pclose(fp);
#endif
        return vs_drivers;
    }
}
