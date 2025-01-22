/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_mail.h
 * @description: silly_mail 类声明
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_MAIL_H
#define SILLY_UTILS_SILLY_MAIL_H
#include <su_marco.h>

using silly_mail_user = std::pair<std::string, std::string>;  // 邮箱 用户名

enum class silly_mail_app_type
{
    smtp,
    pop3,
    imap

};

enum class silly_mail_security_type
{
    normal = 1,
    ssl = 2
};

class silly_mail_conn_opt
{
  public:
    silly_mail_conn_opt()
    {
        server = "smtp.qq.com";
    }
    ~silly_mail_conn_opt() = default;
    silly_mail_conn_opt(const std::string& u, const std::string& p) : user(u), pwd(p)
    {
        server = "smtp.qq.com";
    }
    silly_mail_conn_opt(const std::string& u, const std::string& p, const std::string& s) : user(u), pwd(p), server(s)
    {
    }
    silly_mail_app_type app = silly_mail_app_type::smtp;
    silly_mail_security_type security = silly_mail_security_type::ssl;
    std::string server;
    int port = 465;  // SSL/TLS: 465  STARTTLS: 587
    std::string user;
    std::string pwd;
};

class silly_mail_content
{
  public:
    silly_mail_content() = default;
    ~silly_mail_content() = default;

  public:
    silly_mail_user sender;
    std::vector<silly_mail_user> receivers;
    std::string subject;
    std::string body;
    std::vector<std::string> attachments;  // 附件
};

class silly_mail_receive_content
{
  public:
    silly_mail_receive_content();
    ~silly_mail_receive_content();
};

#endif  // SILLY_UTILS_SILLY_MAIL_H
