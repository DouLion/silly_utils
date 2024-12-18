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
    ~silly_websocket_client();
    /// <summary>
    ///
    /// </summary>
    /// <param name="url"></param>
    /// <returns></returns>
    bool connect(const std::string& url);

    /// <summary>
    /// 发送消息
    /// </summary>
    /// <param name="msg"></param>
    /// <returns></returns>
    bool send(const std::string& msg);

    /// <summary>
    /// 断开连接
    /// </summary>
    /// <param name="bye"></param>
    void close(const std::string& bye);

    /// <summary>
    /// 连接成功,绑定响应函数
    /// </summary>
    /// <typeparam name="Func"></typeparam>
    /// <typeparam name="...Args"></typeparam>
    /// <param name="func"></param>
    /// <param name="...args"></param>
    template <typename Func, typename... Args>
    void on_connect(Func&& func, Args&&... args);

    /// <summary>
    /// 收到消息,绑定响应函数
    /// </summary>
    /// <typeparam name="Func"></typeparam>
    /// <typeparam name="...Args"></typeparam>
    /// <param name="func"></param>
    /// <param name="...args"></param>
    template <typename Func, typename... Args>
    void on_receive(Func&& func, Args&&... args);

    /// <summary>
    /// 被动关闭,绑定响应函数
    /// </summary>
    /// <typeparam name="Func"></typeparam>
    /// <typeparam name="...Args"></typeparam>
    /// <param name="func"></param>
    /// <param name="...args"></param>
    template <typename Func, typename... Args>
    void on_close(Func&& func, Args&&... args);

    /// <summary>
    /// 是否连接
    /// </summary>
    /// <returns></returns>
    bool connected();
    std::string err() const;

  private:
    websocketpp::connection_hdl m_hdl;
    client m_client;
    std::string m_err;
    std::atomic<bool> m_closed = true;
    std::condition_variable m_cv;
    std::mutex m_mutex;
};

template <typename Func, typename... Args>
void silly_websocket_client::on_connect(Func&& func, Args&&... args)
{
    m_client.set_open_handler([&](websocketpp::connection_hdl hdl) {
#ifndef NDEBUG
        std::cout << "已连接" << std::endl;
#endif
        m_closed = false;
        func(std::forward<Args>(args)...);
    });
}

template <typename Func, typename... Args>
void silly_websocket_client::on_receive(Func&& func, Args&&... args)
{
    std::function<void(client * c, websocketpp::connection_hdl, message_ptr)> _on_message;

    _on_message = [func1 = std::forward<Func>(func), args...](client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
        std::string message = msg->get_payload();
#ifndef NDEBUG
        std::cout << "收到消息: " << message << std::endl;
#endif
        func1(message, std::forward<Args>(args)...);
    };
    m_client.set_message_handler(websocketpp::lib::bind(_on_message, &m_client, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
}

template <typename Func, typename... Args>
void silly_websocket_client::on_close(Func&& func, Args&&... args)
{
    m_client.set_close_handler([&](websocketpp::connection_hdl hdl) {
#ifndef NDEBUG
        std::cout << "连接断开" << std::endl;
#endif
        func(std::forward<Args>(args)...);
        m_client.stop();
        m_closed = true;
    });
}

#endif  // SILLY_UTILS_SILLY_WEBSOCKET_CLIENT_H
