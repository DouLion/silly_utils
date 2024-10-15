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

silly_websocket_client::silly_websocket_client()
{
    try
    {
        // Set logging to be pretty verbose (everything except message payloads)
        m_client.set_access_channels(websocketpp::log::alevel::none);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        m_client.init_asio();
        m_client.set_open_handler([this](websocketpp::connection_hdl hdl) {
            //std::cout << "Disconnected from server" << std::endl;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_closed = true;
            }
            m_cv.notify_one();
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
}

bool silly_websocket_client::connect(const std::string& url)
{
    bool status = false;
    try
    {
        websocketpp::lib::error_code ec;
        client::connection_ptr conn = m_client.get_connection(url, ec);
        if (ec)
        {
            m_err = ec.message();
            return status;
        }
        m_hdl = conn->get_handle();
        m_client.connect(conn);
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
    return status;
}

void silly_websocket_client::run()
{
    try
    {
        m_client.run();
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

void silly_websocket_client::close(const std::string& bye)
{
    try
    {
        if (m_hdl.expired())
        {
            m_client.close(m_hdl, websocketpp::close::status::normal, bye);
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
}

bool silly_websocket_client::send(const std::string& msg)
{
    bool status = false;
    try
    {
        m_client.send(m_hdl, msg, websocketpp::frame::opcode::text);
    }
    catch (websocketpp::exception const& e)
    {
        m_err = e.what();
    }
    catch (const std::exception& e)
    {
        m_err = e.what();
    }
    return status;
}
std::string silly_websocket_client::err() const
{
    {
        return m_err;
    }
}
