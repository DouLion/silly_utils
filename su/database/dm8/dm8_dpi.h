/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/30 15:14
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 达梦数据库dpi接口
 */
#pragma once

#if SU_DM8_DPI_ENABLED
#ifndef DM8_DPI_H
#define DM8_DPI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <DPI.h>
#include <DPIext.h>
#include <DPItypes.h>

class dm8_dpi
{
  public:
    dm8_dpi() = default;

    bool login(const char *server, const char *user, const char *pwd);

    void logout();

  private:
    static void dpi_err_msg_print(sdint2 hndl_type, dhandle hndl);

  public:
    dhenv h_env;   /* 环境句柄 */
    dhcon h_con;   /* 连接句柄 */
    dhstmt h_stmt; /* 语句句柄 */
    dhdesc h_desc; /* 描述符句柄 */
    DPIRETURN rt;  /* 函数返回值 */
};

#endif  // DM8_DPI_H
#endif
