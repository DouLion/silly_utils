/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_websocket_client.cpp
 * @description: silly_websocket_client实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_websocket_client.h"
#include "encode/silly_encode.h"

silly_websocket_client::silly_websocket_client()
{
#if ENABLE_WEBSOCKET_PP
    try
    {
        // Set logging to be pretty verbose (everything except message payloads)
        m_client.set_access_channels(websocketpp::log::alevel::none);
        m_client.clear_access_channels(websocketpp::log::alevel::none);

        // Initialize ASIO
        m_client.init_asio();
        m_client.set_open_handler([this](websocketpp::connection_hdl hdl) {
            // std::cout << "Disconnected from server" << std::endl;
            {
#ifndef NDEBUG
                std::cout << "已连接" << std::endl;
#endif
                m_closed = false;
            }
        });
        m_client.set_close_handler([this](websocketpp::connection_hdl hdl) {
            m_client.stop();
            m_closed = true;
        });
    }
    catch (websocketpp::exception const& e)
    {
        m_err = e.what();
    }
    catch (const std::exception& e)
    {
        m_err = e.what();
    }
#endif
}
silly_websocket_client::~silly_websocket_client()
{
#if ENABLE_WEBSOCKET_PP
    m_hdl.reset();
    m_client.stop();
#endif
}

bool silly_websocket_client::connect(const std::string& url)
{
#if ENABLE_WEBSOCKET_PP
    try
    {
        websocketpp::lib::error_code ec;
        client::connection_ptr conn = m_client.get_connection(url, ec);
        if (ec.value())
        {
            m_err = ec.message();
            return !m_closed;
        }
        m_hdl = conn->get_handle();
        m_client.connect(conn);
        std::thread t([this]() { m_client.run(); });
        t.detach();
        int wait = 10;
        while (m_closed && wait--)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (websocketpp::exception const& e)
    {
        m_err = e.what();
    }
    catch (const std::exception& e)
    {
        m_err = e.what();
    }
#endif
    return !m_closed;
}

void silly_websocket_client::close(const std::string& bye)
{
#if ENABLE_WEBSOCKET_PP
    try
    {
        if (m_hdl.expired())
        {
            m_client.close(m_hdl, websocketpp::close::status::normal, bye);
        }
        m_client.stop();
    }
    catch (websocketpp::exception const& e)
    {
        m_err = e.what();
    }
    catch (const std::exception& e)
    {
        m_err = e.what();
    }
#endif
}

bool silly_websocket_client::send(const std::string& msg)
{
    bool status = false;
#if ENABLE_WEBSOCKET_PP
    try
    {
        m_client.send(m_hdl, msg, websocketpp::frame::opcode::text);
        status = true;
    }
    catch (websocketpp::exception const& e)
    {
        m_err = e.what();
    }
    catch (const std::exception& e)
    {
        m_err = e.what();
    }
#endif
    return status;
}
std::string silly_websocket_client::err() const
{
#if WIN32
    return silly_encode::gbk_utf8(m_err);
#endif
    return m_err;
}

bool silly_websocket_client::connected()
{
    return !m_closed;
}
