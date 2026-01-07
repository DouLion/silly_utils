/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_websocket_server.h
 * @description: silly_websocket_server 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_WEBSOCKET_SERVER_H
#define SILLY_UTILS_SILLY_WEBSOCKET_SERVER_H
#include <su_macro.h>
#if ENABLE_WEBSOCKET_PP
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#endif
class silly_websocket_server
{
  public:
    silly_websocket_server();
    ~silly_websocket_server() = default;

    bool listen(const int& port);

  private:
    int m_port = 9003;
#if ENABLE_WEBSOCKET_PP
    websocketpp::server<websocketpp::config::asio> m_server;
#endif
    std::string m_err;
};

#endif  // SILLY_UTILS_SILLY_WEBSOCKET_SERVER_H
