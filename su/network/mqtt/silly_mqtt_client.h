/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_mqtt_client.h
 * @description: mqtt 客户端
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_MQTT_CLIENT_H
#define SILLY_UTILS_SILLY_MQTT_CLIENT_H
#include <su_marco.h>
/* 支持的协议
"mqtt://<host>:<port>"   - TCP, unsecure
"tcp://<host>:<port>"    (same)

"mqtts://<host>:<port>"  - SSL/TLS
"ssl://<host>:<port>"     (same)

"ws://<host>:<port>"    - Unsecure websockets
"wss://<host>:<port>"   - Secure websockets
 */
class silly_mqtt_client
{
  public:
    /// <summary>
    /// 
    /// </summary>
    /// <param name="topic"></param>
    /// <param name="payload"></param>
    /// <returns></returns>
    bool publish(const std::string& topic, const std::string& payload);


    bool subscribe(const std::string& topic);

    bool check();

    void client_id(const std::string& ci);

    void user(const std::string& u);
    void password(const std::string& p);
    void host(const std::string& h);
    void port(const int& p);
    void qos(const int& q);
    void protocol(const std::string& p);

    void server(const std::string& s);



  private:
     std::string m_client_id;
    std::string m_user;
     std::string m_protocol{"mqtt"};
    std::string m_password;
    std::string m_host;
    int m_port{1883}; // 1883 默认非加密端口  8883 默认加密端口
    int m_qos{2}; // 0 最多一次  1 最少一次  2 只有一次
};

#endif  // SILLY_UTILS_SILLY_MQTT_CLIENT_H
