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
class silly_smtp
{
  public:
    silly_smtp();
    ~silly_smtp();

    bool send(const silly_mail_send_content& mail_content);
    void set_smtp_options(const silly_mail_opt& options) {
        smtp_options = options;
    }
  private:
    silly_mail_opt smtp_options;
};

#endif  // SILLY_UTILS_SILLY_SMTP_H
