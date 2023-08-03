//
// Created by dly on 2023/7/30.
//

#include "dm8_dpi.h"

#if DM8_ENABLED

bool dm8_dpi::login(const char* server, const char* user, const char* pwd)
{
	rt = dpi_alloc_env(&h_env);
	rt = dpi_alloc_con(h_env, &h_con);
	rt = dpi_login(h_con, (sdbyte*)server, (sdbyte*)user, (sdbyte*)pwd);
	if (!DSQL_SUCCEEDED(rt))
	{
		dpi_err_msg_print(DSQL_HANDLE_DBC, h_con);
		return false;
	}
	rt = dpi_alloc_stmt(h_con, &h_stmt);
	if (!DSQL_SUCCEEDED(rt))
	{
		dpi_err_msg_print(DSQL_HANDLE_DBC, h_con);
		return false;
	}

	return true;
}

void dm8_dpi::dpi_err_msg_print(sdint2 hndl_type, dhandle hndl)
{
	sdint4 err_code;
	sdint2 msg_len;
	sdbyte err_msg[SDBYTE_MAX];

	/* 获取错误信息集合 */
	dpi_get_diag_rec(hndl_type, hndl, 1, &err_code, err_msg, sizeof(err_msg), &msg_len);
	printf("err_msg = %s, err_code = %d\n", err_msg, err_code);
}

void dm8_dpi::logout()
{
	rt = dpi_logout(h_con);
	if (!DSQL_SUCCEEDED(rt))
	{
		dpi_err_msg_print(DSQL_HANDLE_DBC, h_con);
		return;
	}

	rt = dpi_free_con(h_con);
	rt = dpi_free_env(h_env);
}

#endif