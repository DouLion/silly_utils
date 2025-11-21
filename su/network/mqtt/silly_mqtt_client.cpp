/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_mqtt_client.cpp
 * @description: silly_mqtt_client实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_mqtt_client.h"
#if SU_THIRD_SUPPORT_PAHO_MQTT
#include <mqtt/client.h>
#include <mqtt/ssl_options.h>
#endif
#include <log/silly_log.h>

const auto PERIOD = std::chrono::seconds(20);

const int MAX_BUFFERED_MSGS = 120;  // 120 * 5sec => 10min off-line buffering

const int N_RETRY_ATTEMPTS = 5;
#if SU_THIRD_SUPPORT_PAHO_MQTT
class action_listener : public virtual mqtt::iaction_listener
{
    std::string name_;

    void on_failure(const mqtt::token& tok) override
    {
        std::cout << name_ << " failure";
        if (tok.get_message_id() != 0)
            std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
        std::cout << std::endl;
    }

    void on_success(const mqtt::token& tok) override
    {
        std::cout << name_ << " success";
        if (tok.get_message_id() != 0)
            std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
        auto top = tok.get_topics();
        if (top && !top->empty())
            std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
        std::cout << std::endl;
    }

  public:
    action_listener(const std::string& name) : name_(name)
    {
    }
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */
class message_callback : public virtual mqtt::callback, public virtual mqtt::iaction_listener

{
    // Counter for the number of connection retries
    int m_nretry = 0;
    // The MQTT client
    mqtt::async_client& m_client;
    // Options to use if we need to reconnect
    mqtt::connect_options& m_opts;
    // An action listener to display the result of actions.
    action_listener m_sub_listener;
    int m_qos = 2;
    std::string m_topic;

    suMqttClient::subscribe_callback m_func;

    // This deomonstrates manually reconnecting to the broker by calling
    // connect() again. This is a possibility for an application that keeps
    // a copy of it's original connect_options, or if the app wants to
    // reconnect with different options.
    // Another way this can be done manually, if using the same options, is
    // to just call the async_client::reconnect() method.
    void reconnect()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        try
        {
            m_client.connect(m_opts, nullptr, *this);
        }
        catch (const mqtt::exception& exc)
        {
            std::cerr << "Error: " << exc.what() << std::endl;
            return;
        }
    }

    // Re-connection failure
    void on_failure(const mqtt::token& tok) override
    {
        SLOG_WARN("尝试连接失败({})", m_nretry)
        if (++m_nretry > N_RETRY_ATTEMPTS)
        {
            return;
        }
        reconnect();
    }

    // (Re)connection success
    // Either this or connected() can be used for callbacks.
    void on_success(const mqtt::token& tok) override
    {
    }

    // (Re)connection success
    void connected(const std::string& cause) override
    {
        SLOG_INFO("连接成功, 订阅主题 : {}, 连接质量: {}", m_topic, m_qos)
        m_client.subscribe(m_topic, m_qos, nullptr, m_sub_listener);
    }

    // Callback for when the connection is lost.
    // This will initiate the attempt to manually reconnect.
    void connection_lost(const std::string& cause) override
    {
        SLOG_WARN("连接断开:{},准备重连...", cause)
        m_nretry = 0;
        reconnect();
    }

    // Callback for when a message arrives.
    void message_arrived(mqtt::const_message_ptr msg) override
    {
        SLOG_DEBUG("新消息!!!\n主题:{}\n内容:\n{}", msg->get_topic(), msg->to_string())
        std::thread t(m_func, msg->get_topic(), msg->to_string());
        t.detach();
    }

    void delivery_complete(mqtt::delivery_token_ptr token) override
    {
    }

  public:
    message_callback(mqtt::async_client& cli, mqtt::connect_options& connOpts, std::string topic, int& qos, suMqttClient::subscribe_callback fc)
        : m_nretry(0), m_client(cli), m_opts(connOpts), m_sub_listener("Subscription"), m_topic(topic), m_qos(qos), m_func(fc)
    {
    }
};
#endif
bool suMqttClient::publish(const std::string& topic, const std::string& payload)
{
    bool status = false;

#if SU_THIRD_SUPPORT_PAHO_MQTT

    auto connBuilder = mqtt::connect_options_builder();
    mqtt::connect_options conn_opts = connBuilder.keep_alive_interval(PERIOD).finalize();
    conn_opts.set_ssl(mqtt::ssl_options());
    conn_opts.set_mqtt_version(MQTTVERSION_3_1);
    if (!m_user.empty() && !m_password.empty())
    {
        conn_opts.set_user_name(m_user);
        conn_opts.set_password(m_password);
    }
    make_uri();

    try
    {
        mqtt::async_client cli(m_uri, m_client_id);
        cli.connect(conn_opts)->wait();
        auto msg = mqtt::make_message(topic, payload, m_qos, false);
        status = cli.publish(msg)->wait_for(20000);  // 毫秒
        cli.disconnect()->wait();
    }
    catch (const mqtt::exception& exc)
    {
        SLOG_ERROR("MQTT({})\n发布内容 :\n{} \n到主题:[ {} ]失败,错误信息:\n {}.", m_uri, payload, topic, exc.what());
    }
    catch (const std::exception& exc)
    {
        SLOG_ERROR("MQTT({})\n发布内容 :\n{} \n到主题:[ {} ]失败,错误信息:\n {}.", m_uri, payload, topic, exc.what());
    }
#endif
    return status;
}

void suMqttClient::subscribe(const std::string& topic, subscribe_callback scb)
{
    bool status = false;

#if SU_THIRD_SUPPORT_PAHO_MQTT

    auto connBuilder = mqtt::connect_options_builder();
    mqtt::connect_options conn_opts = connBuilder.keep_alive_interval(PERIOD).finalize();
    conn_opts.set_ssl(mqtt::ssl_options());
    conn_opts.set_mqtt_version(MQTTVERSION_3_1);
    if (!m_user.empty() && !m_password.empty())
    {
        conn_opts.set_user_name(m_user);
        conn_opts.set_password(m_password);
    }
    make_uri();

    try
    {
        m_disconnected = false;
        mqtt::async_client cli(m_uri, m_client_id);
        message_callback mcb(cli, conn_opts, topic, m_qos, scb);
        cli.set_callback(mcb);
        cli.connect(conn_opts, nullptr, mcb);
        while (!m_disconnected)
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        cli.disconnect()->wait();
    }
    catch (const mqtt::exception& exc)
    {
        SLOG_ERROR("MQTT({})\n订阅主题:[ {} ]失败,错误信息:\n {}.", m_uri, topic, exc.what());
    }
    catch (const std::exception& exc)
    {
        SLOG_ERROR("MQTT({})\n订阅主题:[ {} ]失败,错误信息:\n {}.", m_uri, topic, exc.what());
    }

#endif
}

bool suMqttClient::check()
{
    return false;
}

void suMqttClient::client_id(const std::string& ci)
{
    m_client_id = ci;
}

void suMqttClient::user(const std::string& u)
{
    m_user = u;
}

void suMqttClient::password(const std::string& p)
{
    m_password = p;
}

void suMqttClient::host(const std::string& h)
{
    m_host = h;
}

void suMqttClient::port(const int& p)
{
    m_port = p;
}

void suMqttClient::qos(const int& q)
{
    m_qos = q;
}

void suMqttClient::protocol(const std::string& p)
{
    m_protocol = p;
}

void suMqttClient::make_uri()
{
    if ("mqtt" == m_protocol)
    {
        m_uri = m_host + ":" + std::to_string(m_port);
    }
    else if ("ws" == m_protocol)
    {
        m_uri = m_protocol + "://" + m_host + ":" + std::to_string(m_port);
    }
}

void suMqttClient::server(const std::string& s)
{
    auto pos = s.find("://");
    int nPos = 0;
    if (pos != std::string::npos)
    {
        protocol(s.substr(0, pos));
        nPos = pos + 3;
    }
    std::string ss = s.substr(nPos);
    pos = ss.find(":");
    if (pos != std::string::npos)
    {
        port(std::stoi(ss.substr(pos + 1)));
        ss = ss.substr(0, pos);
    }
    host(ss);
}

void suMqttClient::disconnect()
{
    m_disconnected = true;
}
