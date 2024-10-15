/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_websocket_client.h
 * @description: silly_websocket_client 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_WEBSOCKET_CLIENT_H
#define SILLY_UTILS_SILLY_WEBSOCKET_CLIENT_H
#include <su_marco.h>
#ifndef _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#endif
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class silly_websocket_client
{
  public:
    silly_websocket_client();
    ~silly_websocket_client() = default;

    template <typename Func, typename... Args>
    void on_connect(Func&& func, Args&&... args);

    bool connect(const std::string& url);

    bool send(const std::string& msg);

    template <typename Func, typename... Args>
    void on_receive(Func&& func, Args&&... args);
    /*template <typename Func>
    void on_receive(Func&& func);*/

    template <typename Func, typename... Args>
    void on_close(Func&& func, Args&&... args);
    void close(const std::string& bye);

    void run();
    std::string err() const;

  private:
    websocketpp::connection_hdl m_hdl;
    client m_client;
    std::string m_err;
    std::atomic<bool> m_closed = false;
    std::condition_variable m_cv;
    std::mutex m_mutex;
};

template <typename Func, typename... Args>
void silly_websocket_client::on_connect(Func&& func, Args&&... args)
{
    /* m_client.set_open_handler([this, func, &args...](websocketpp::connection_hdl hdl) {
         m_conn = m_client.get_con_from_hdl(hdl);
         func(m_conn, args...);
     });*/
}

template <typename Func, typename... Args>
void silly_websocket_client::on_receive(Func&& func, Args&&... args)
{
    std::function<void(client * c, websocketpp::connection_hdl, message_ptr)> on_message;

    on_message = [func1 = std::forward<Func>(func), args...](client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
        std::string message = msg->get_payload();
        std::cout << "Received message: " << message << std::endl;
        func1(message, std::forward<Args>(args)...);
    };
    m_client.set_message_handler(websocketpp::lib::bind(on_message, &m_client, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
}

/*
template <typename Func>
void silly_websocket_client::on_receive(Func&& func)
{
    auto on_message = [](client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
        std::string message = msg->get_payload();
        func(message);
    };
    m_client.set_message_handler(websocketpp::lib::bind(func, &m_client, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
}
*/

template <typename Func, typename... Args>
void silly_websocket_client::on_close(Func&& func, Args&&... args)
{
    /*if(m_hdl.expired())
    {

    }
    m_client.set_close_handler(std::bind(func, std::forward<Args>(args)...));*/
}

#endif  // SILLY_UTILS_SILLY_WEBSOCKET_CLIENT_H
