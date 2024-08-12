/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/8/9 10:46
 * @version: 1.0.1
 * @description: silly_mail_send 类声明
 */
#ifndef SILLY_UTILS_SILLY_MAIL_SEND_H
#define SILLY_UTILS_SILLY_MAIL_SEND_H
#include <log/silly_log.h>

using silly_mail_addr_name = std::tuple<std::string, std::string>;
/*

[Sender]
#发送邮件种类
smtp=smtp.qq.com
address=2292405399@qq.com
#smtp授权码
pwd=fevvtczwqnvjdicc
[Receiver]
#收件人邮件，用分号分隔开
address=851749525@qq.com;hmc@tianzhixiang.com.cn

*/

class silly_mail_attachment
{
  public:
    std::string name;
    std::string type;
    std::string content;
};

class silly_mail_send
{
  public:
    ~silly_mail_send();

    /// <summary>
    /// 设置邮件标题
    /// </summary>
    /// <param name="str"></param>
    void title(const std::string& str);

    /// <summary>
    /// 设置邮件正文
    /// </summary>
    /// <param name="str"></param>
    void content(const std::string& str);

    /// <summary>
    /// 添加附件
    /// </summary>
    /// <param name="sma"></param>
    void attach(const silly_mail_attachment& sma);

    /// <summary>
    /// 添加邮件接收者
    /// </summary>
    /// <param name="sman"></param>
    void add_receiver(const silly_mail_addr_name& sman);
   
    /// <summary>
    /// 设置邮件接收者
    /// </summary>
    /// <param name="smans"></param>
    void receivers(const std::vector<silly_mail_addr_name>& smans);

    /// <summary>
    /// 发送邮件
    /// </summary>
    /// <returns></returns>
    bool send();

  public:
    void smtp(const std::string& str);
    void imap(const std::string& str);
    void pop3(const std::string& str);
    void user(const std::string& str);
    void password(const std::string& str);
    void name(const std::string& str);
    void from(const std::string& str);

  private:
    /////////////////////////邮件信息///////////////////////////////////
    /////////////////////////connent///////////////////////////////////
    std::string m_smtp;      // smtp服务器地址
    std::string m_imap;      // imap服务器地址
    std::string m_pop3;      // pop3服务器地址
    std::string m_user;      // 邮箱用户
    std::string m_password;  // 邮箱用户密
    /////////////////////////SendMail//////////////////////////////////
    std::string m_name;     // 发送者的名
    std::string m_from;     // 发送者的邮箱地址
    std::string m_title;    // 邮件标题(subject)
    std::string m_content;  // 邮件正文

    std::vector<silly_mail_addr_name> m_receivers;  // 邮件接收者（name,email_address)
};

#endif  // SILLY_UTILS_SILLY_MAIL_SEND_H
