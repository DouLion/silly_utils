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
#if ENABLE_PAHO_MQTT
#include <mqtt/client.h>
#include <mqtt/ssl_options.h>
#endif

const auto PERIOD = std::chrono::seconds(20);

const int MAX_BUFFERED_MSGS = 120;  // 120 * 5sec => 10min off-line buffering

bool silly_mqtt_client::publish(const std::string& topic, const std::string& payload)
{
    bool status = false;
    std::string uri = m_protocol + "://" + m_host + ":" + std::to_string(m_port);
#if ENABLE_PAHO_MQTT

    auto conn_opts = mqtt::connect_options_builder().keep_alive_interval(PERIOD).clean_session(true).automatic_reconnect(true).finalize();
    conn_opts.set_ssl(mqtt::ssl_options());
    conn_opts.set_mqtt_version(MQTTVERSION_3_1);
    conn_opts.set_user_name(m_user);
    conn_opts.set_password(m_password);
    mqtt::async_client cli(uri, m_client_id);
    // mqtt::topic top(cli, topic, m_qos, true);
    try
    {
        cli.connect(conn_opts)->wait();
        auto msg = mqtt::make_message(topic, payload, m_qos, false);
        status = cli.publish(msg)->wait_for(20000);  // 毫秒
        cli.disconnect()->wait();
    }
    catch (const mqtt::exception& exc)
    {
        SLOG_ERROR("MQTT发布内容 :\n{} \n到主题:[ {} ]失败,错误信息:\n {}.", payload, topic, exc.what());
    }
    catch (const std::exception& exc)
    {
        SLOG_ERROR("MQTT发布内容 :\n{} \n到主题:[ {} ]失败,错误信息:\n {}.", payload, topic, exc.what());
    }
#endif
    return false;
}

bool silly_mqtt_client::subscribe(const std::string& topic)
{
    return false;
}

bool silly_mqtt_client::check()
{
    return false;
}

void silly_mqtt_client::client_id(const std::string& ci)
{
    m_client_id = ci;
}

void silly_mqtt_client::user(const std::string& u)
{
    m_user = u;
}

void silly_mqtt_client::password(const std::string& p)
{
    m_password = p;
}

void silly_mqtt_client::host(const std::string& h)
{
    m_host = h;
}

void silly_mqtt_client::port(const int& p)
{
    m_port = p;
}

void silly_mqtt_client::qos(const int& q)
{
    m_qos = q;
}

void silly_mqtt_client::protocol(const std::string& p)
{
    m_protocol = p;
}

void silly_mqtt_client::server(const std::string& s)
{
    auto pos = s.find("://");
    if (pos != std::string::npos)
    {
        protocol(s.substr(0, pos));
    }
    std::string ss = s.substr(pos + 3);
    pos = ss.find(":");
    if (pos != std::string::npos)
    {
        port(std::stoi(ss.substr(pos + 1)));
        ss = ss.substr(0, pos);
    }
    host(ss);
}
