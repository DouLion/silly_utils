/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_websocket_server.cpp
 * @description: silly_websocket_server实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_websocket_server.h"
silly_websocket_server::silly_websocket_server()
{
    try
    {
        // Set logging to be pretty verbose (everything except message payloads)
        m_server.set_access_channels(websocketpp::log::alevel::none);
        m_server.clear_access_channels(websocketpp::log::alevel::none);

        // Initialize ASIO
        m_server.init_asio();

    }
    catch (websocketpp::exception const& e)
    {
        m_err = e.what();
    }
    catch (const std::exception& e)
    {
        m_err = e.what();
    }
}
