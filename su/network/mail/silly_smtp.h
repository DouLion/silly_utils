/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_smtp.h
 * @description: silly_smtp 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SMTP_H
#define SILLY_UTILS_SILLY_SMTP_H
#include <network/mail/silly_mail.h>
#include <curl/curl.h>

class silly_smtp
{
  public:
    bool logon();

    bool logon(const std::string& account, const std::string& pwd);

    bool send(const silly_mail_content& content);

    void conn_opt(const silly_mail_conn_opt& mco);

  private:                                                           // 以字符形式读入附件内容

    std::string Base64Encode(std::string in_str);  // 把char类型转换成Base64类型
    // 获取时间
    std::string prepareDate();

    // 通信recv和send的封装
    int req(const std::string content, bool bout = false);  // 返回发送了多少字节
    bool resp(const std::string expected_response);         // 返回接收的结果和expected_response是否相同

    // 工作函数
    bool create_socket();  // 建立socket连接

    bool head(const silly_mail_content& content);

    bool body(const silly_mail_content& content);

    bool attachments(const silly_mail_content& content);

    bool end();

  private:
    bool m_connected{false};

    uint64_t m_socket;

    silly_mail_conn_opt m_conn_opt;
};

#endif  // SILLY_UTILS_SILLY_SMTP_H
