/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: suMqttClient.h
 * @description: mqtt 客户端
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_MQTT_CLIENT_H
#define SILLY_MQTT_CLIENT_H
#include <su_macro.h>
/* 常用的协议
"mqtt://<host>:<port>"   - TCP, unsecure
"tcp://<host>:<port>"    (same)

"mqtts://<host>:<port>"  - SSL/TLS
"ssl://<host>:<port>"     (same)

"ws://<host>:<port>"    - Unsecure websockets
"wss://<host>:<port>"   - Secure websockets
 */

class suMqttClient
{
    /// <summary>
    /// mqtt客户端
    /// 目前仅支持 ws 和 mqtt协议
    /// </summary>
  public:
    typedef std::function<void(std::string, std::string)> subscribe_callback;

  public:
    /// <summary>
    /// 发布主题
    /// </summary>
    /// <param name="topic"></param>
    /// <param name="payload"></param>
    /// <returns></returns>
    bool publish(const std::string& topic, const std::string& payload);

    /// <summary>
    /// 订阅主题, 该函数会一直运行,直到调用disconnect()函数
    /// </summary>
    /// <param name="topic"></param>
    /// <param name="scb"></param>
    void subscribe(const std::string& topic, subscribe_callback scb);

    /// <summary>
    /// 暂时没吊用
    /// </summary>
    /// <returns></returns>
    bool check();

    /// <summary>
    /// 暂时没吊用
    /// </summary>
    /// <param name="ci"></param>
    void client_id(const std::string& ci);

    /// <summary>
    /// 设置项
    /// </summary>
    /// <param name=""></param>
    void user(const std::string& u);
    void password(const std::string& p);
    void host(const std::string& h);
    void port(const int& p);
    void qos(const int& q);
    void protocol(const std::string& p);

    /// <summary>
    /// 接受 协议名://主机IP:端口
    /// </summary>
    /// <param name="s"></param>
    void server(const std::string& s);
    /// <summary>
    /// 订阅时,主动使其断开连接并且退出
    /// </summary>
    void disconnect();

  private:
    void make_uri();

  private:
    std::string m_client_id;
    std::string m_user;
    std::string m_protocol{"mqtt"};
    std::string m_password;
    std::string m_host;
    int m_port{1883};  // 1883 默认非加密端口  8883 默认加密端口
    int m_qos{2};      // 0 最多一次  1 最少一次  2 只有一次
    std::string m_uri;
    bool m_disconnected = false;
};

typedef suMqttClient sumqttcli;

/* 订阅使用示例
int main(int argc, char** argv)
{
    // WINDOWS_UTF8_PAGE
    SLOG_INFO("SU测试模块")

    suMqttClient mqtt;
    mqtt.user("tzx");
    mqtt.password("3edc9ijn");
    mqtt.server("mqtt://192.168.0.60:11883");
    std::string content;
    std::string topic;
    std::thread t(&suMqttClient::subscribe, &mqtt, "warning/#",
    [&content, &topic](std::string rTopic, std::string rMsg) -> void {

        content = rMsg;
        topic = rTopic;
    });
    t.detach();

    while (1)
    {
        if (!content.empty())
        {
            std::cout << topic << std::endl;
            std::cout << content << std::endl;
            content.clear();
            topic.clear();
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;

*/

#endif  // SILLY_MQTT_CLIENT_H
