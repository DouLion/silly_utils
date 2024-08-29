/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/31 11:26
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#if I_NEED_TEST
#include <catch2/catch_test_macros.hpp>

#include <database/otl/otl_tools.h>

#include "database/dm8/dm8_dpi.h"
#include <filesystem>
#include "database/sql_studio.hpp"
#if DM8_ENABLED
void dpi_err_msg_print(sdint2 hndl_type, dhandle hndl)
{
	sdint4 err_code;
	sdint2 msg_len;
	sdbyte err_msg[SDBYTE_MAX];

	/* 获取错误信息集合 */
	dpi_get_diag_rec(hndl_type, hndl, 1, &err_code, err_msg, sizeof(err_msg), &msg_len);
	printf("err_msg = %s, err_code = %d\n", err_msg, err_code);
}
#endif
TEST_CASE("TestDataBase")
{
    SECTION("OTL_CONNECT")  // OTL链接数据库
    {
        std::cout << "\r\n\r\n****************" << "OTL_CONNECT" << "****************" << std::endl;
        otl_connect db;
        try
        {
            db.rlogon("DSN=DM_HUBEI;UID=SYSDBA;PWD=SYSDBA;database=TZX_DYNAMIC_ANALYSIS_HUBEI;");
            otl_stream os;
            os.open(256, "select ADCD,ADNM from TZX_DYNAMIC_ANALYSIS_HUBEI.HUBEI_ADCD_INFO;", db);
            for (auto& s : os)
            {
                otl_value<std::string> adcd, adnm;
                otl_read_row(s, adcd, adnm);
                std::cout << adcd.v << ", " << adnm.v << std::endl;
            }
        }
        catch (otl_exception& e)
        {
            printf("sql:%s \nmessage:%s \n state:%s", e.stm_text, e.msg, e.sqlstate);
        }
        db.logoff();
    };

    SECTION("OTL_INSERT")  // OTL 添加数据
    {
        std::cout << "\r\n\r\n****************" << "OTL_INSERT" << "****************" << std::endl;
    };

    SECTION("OTL_DELETE")  // OTL 删除数据
    {
        std::cout << "\r\n\r\n****************" << "OTL_DELETE" << "****************" << std::endl;
    };

    SECTION("OTL_QUERY")  // OTL 查询数据
    {
        std::cout << "\r\n\r\n****************" << "OTL_QUERY" << "****************" << std::endl;
    };

    SECTION("OTL_MODIFY")  // OTL 修改数据
    {
        std::cout << "\r\n\r\n****************" << "OTL_MODIFY" << "****************" << std::endl;
    };

#if DM8_ENABLED

    SECTION("DM8_DPI_CONNECT")  // 达梦DPI链接数据库
    {
        std::cout << "\r\n\r\n****************" << "DM8_DPI_CONNECT" << "****************" << std::endl;
        dm8_dpi dpi;
        BOOST_CHECK(dpi.login("192.168.0.201:5236", "TZX", "3edc9ijn~"));
        dpi.logout();
    }

    SECTION("DM8_DPI_CREATE")  // 达梦DPI 建表
    {
        std::cout << "\r\n\r\n****************" << "DM8_DPI_INSERT 建表" << "****************" << std::endl;

        std::string create_sql = "CREATE TABLE CITY ( \"CITY_ID\" CHAR(3) NOT NULL, \"CITY_NAME\" VARCHAR(40), \"REGION_ID\" INT); ";
        dm8_dpi dpi;
        BOOST_CHECK(dpi.login("192.168.0.201:5236", "TZX", "3edc9ijn~"));
        dpi.rt = dpi_exec_direct(dpi.h_stmt, (sdbyte*)create_sql.c_str());
        if (!DSQL_SUCCEEDED(dpi.rt))
        {
            dpi_err_msg_print(DSQL_HANDLE_STMT, dpi.h_stmt);
        }
        printf("dpi: create success\n");

        dpi.logout();
    }

    SECTION("DM8_DPI_INSERT")  // 达梦DPI 添加数据
    {
        std::cout << "\r\n\r\n****************" << "DM8_DPI_ADD" << "****************" << std::endl;
        dm8_dpi dpi;
        sdbyte city_id[3] = {0};
        slength city_id_ind = 0;
        ulength row_num;
        BOOST_CHECK(dpi.login("192.168.0.201:5236", "TZX", "3edc9ijn~"));
        dpi.rt = dpi_exec_direct(dpi.h_stmt, (sdbyte*)"insert into CITY(CITY_ID,city_name) values('BJ','北京')");
        if (!DSQL_SUCCEEDED(dpi.rt))
        {
            dpi_err_msg_print(DSQL_HANDLE_STMT, dpi.h_stmt);
        }
        // 查询
        dpi.rt = dpi_exec_direct(dpi.h_stmt, (sdbyte*)"select CITY_ID from CITY");
        if (!DSQL_SUCCEEDED(dpi.rt))
        {
            dpi_err_msg_print(DSQL_HANDLE_STMT, dpi.h_stmt);
        }
        dpi_bind_col(dpi.h_stmt, 1, DSQL_C_NCHAR, &city_id, sizeof(city_id), &city_id_ind);
        while (dpi_fetch(dpi.h_stmt, &row_num) != DSQL_NO_DATA)
        {
            printf("city_id = %s\n", city_id);
        }
        dpi.logout();
        // exit(0);
    }

    SECTION("DM8_DPI_MODIFY")  // 达梦DPI 修改数据
    {
        std::cout << "\r\n\r\n****************" << "DM8_DPI_MODIFY" << "****************" << std::endl;
        dm8_dpi dpi;
        sdbyte city_id[3] = {0};
        slength city_id_ind = 0;
        ulength row_num;
        BOOST_CHECK(dpi.login("192.168.0.201:5236", "TZX", "3edc9ijn~"));
        dpi.rt = dpi_exec_direct(dpi.h_stmt, (sdbyte*)"update CITY set city_id = 'BH' where city_id='BJ' ");
        if (!DSQL_SUCCEEDED(dpi.rt))
        {
            dpi_err_msg_print(DSQL_HANDLE_STMT, dpi.h_stmt);
        }
        // 查询
        dpi.rt = dpi_exec_direct(dpi.h_stmt, (sdbyte*)"select CITY_ID from CITY");
        if (!DSQL_SUCCEEDED(dpi.rt))
        {
            dpi_err_msg_print(DSQL_HANDLE_STMT, dpi.h_stmt);
        }
        dpi_bind_col(dpi.h_stmt, 1, DSQL_C_NCHAR, &city_id, sizeof(city_id), &city_id_ind);
        while (dpi_fetch(dpi.h_stmt, &row_num) != DSQL_NO_DATA)
        {
            printf("city_id = %s\n", city_id);
        }

        dpi.logout();
    }

    SECTION("DM8_DPI_DELETE")  // 达梦DPI 删除数据
    {
        std::cout << "\r\n\r\n****************" << "DM8_DPI_DELETE" << "****************" << std::endl;
        dm8_dpi dpi;
        sdbyte city_id[3] = {0};
        slength city_id_ind = 0;
        ulength row_num;
        BOOST_CHECK(dpi.login("192.168.0.201:5236", "TZX", "3edc9ijn~"));
        dpi.rt = dpi_exec_direct(dpi.h_stmt, (sdbyte*)"delete from CITY where city_id = 'BH'");
        if (!DSQL_SUCCEEDED(dpi.rt))
        {
            dpi_err_msg_print(DSQL_HANDLE_STMT, dpi.h_stmt);
        }
        // 查询
        dpi.rt = dpi_exec_direct(dpi.h_stmt, (sdbyte*)"select CITY_ID from CITY");
        if (!DSQL_SUCCEEDED(dpi.rt))
        {
            dpi_err_msg_print(DSQL_HANDLE_STMT, dpi.h_stmt);
        }
        dpi_bind_col(dpi.h_stmt, 1, DSQL_C_NCHAR, &city_id, sizeof(city_id), &city_id_ind);
        while (dpi_fetch(dpi.h_stmt, &row_num) != DSQL_NO_DATA)
        {
            printf("city_id = %s\n", city_id);
        }

        dpi.logout();
    }

    SECTION("DM8_DPI_DROP")  // 达梦DPI 删除表
    {
        std::cout << "\r\n\r\n****************" << "DM8_DPI_DELETE" << "****************" << std::endl;
    }

    SECTION("DM8_DPI_QUERY")  // 达梦DPI 查询数据
    {
        std::cout << "\r\n\r\n****************" << "DM8_DPI_QUERY" << "****************" << std::endl;

        dm8_dpi dpi;
        BOOST_CHECK(dpi.login("192.168.0.201:5236", "TZX", "3edc9ijn~"));
        sdint4 region_id = 0;
        sdbyte city_id[3] = {0};
        sdbyte city_name[40] = {0};
        slength city_id_ind = 0;
        slength city_name_ind = 0;
        slength region_id_ind = 0;
        ulength row_num;
        dpi_exec_direct(dpi.h_stmt, (sdbyte*)"select * from CITY");
        dpi_bind_col(dpi.h_stmt, 1, DSQL_C_NCHAR, &city_id, sizeof(city_id), &city_id_ind);
        dpi_bind_col(dpi.h_stmt, 2, DSQL_C_NCHAR, &city_name, sizeof(city_name), &city_name_ind);
        dpi_bind_col(dpi.h_stmt, 3, DSQL_C_SLONG, &region_id, sizeof(region_id), &region_id_ind);

        printf("dpi: select from table...\n");
        while (dpi_fetch(dpi.h_stmt, &row_num) != DSQL_NO_DATA)
        {
            printf("city_id = %s, city_name = %s , region_id=%d\n", city_id, city_name, region_id);
        }
        printf("dpi: select success\n");

        dpi.logout();
    }

#endif
}


#endif //SILLY_UTILS_TEST_DATABASE_HPP
