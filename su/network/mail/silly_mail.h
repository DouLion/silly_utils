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

using silly_mail_user = std::pair<std::string, std::string>;

enum class silly_mail_server_type
{
    smtp,
    pop3,
    imap

};
class silly_mail_opt
{
  public:
    bool ssl= false;
    std::string server;
    std::string port;
    std::string user;
    std::string password;
};

class silly_mail_send_content
{
  public:
    silly_mail_send_content();
    ~silly_mail_send_content();

  public:
    std::string title;
    std::string content;
    silly_mail_user sender;
    std::vector<silly_mail_user> receivers;
    std::vector<std::string> attachments; // 附件

};

class silly_mail_receive_content
{
  public:
    silly_mail_receive_content();
    ~silly_mail_receive_content();

};

#endif  // SILLY_UTILS_SILLY_MAIL_H
