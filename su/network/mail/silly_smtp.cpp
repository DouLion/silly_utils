/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_smtp.cpp
 * @description: silly_smtp实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_smtp.h"
#include <curl/curl.h>
silly_smtp::silly_smtp()
{
}
silly_smtp::~silly_smtp()
{
}
bool silly_smtp::send(const silly_mail_send_content& mail_content)
{
    CURL* curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);  
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl." << std::endl;
        return false;
    }

    // 设置SMTP服务器
    std::string smtp_url = "smtp://" + smtp_options.server + ":" + smtp_options.port;
    curl_easy_setopt(curl, CURLOPT_URL, smtp_url.c_str());

    // 启用TLS
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    // 设置CA证书路径
    curl_easy_setopt(curl, CURLOPT_CAINFO, "D:/cacert-2024-09-24.pem");
    // 设置SMTP认证
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtp_options.user.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, smtp_options.password.c_str());

    // 设置发件人和收件人
    std::string from = "<" + mail_content.sender.first + ">";
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());

    // 设置收件人列表
    struct curl_slist* recipient_list = NULL;
    for (const auto& receiver : mail_content.receivers) {
        std::string to = "<" + receiver.first + ">";
        recipient_list = curl_slist_append(recipient_list, to.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipient_list);

    // 设置邮件内容
    std::string message = "To: " + mail_content.receivers[0].first + "\r\n"
                          "From: " + mail_content.sender.first + "\r\n"
                          "Subject: " + mail_content.title + "\r\n"
                          "MIME-Version: 1.0\r\n"
                          "Content-Type: text/plain; charset=UTF-8\r\n"
                          "\r\n" + mail_content.content + "\r\n";

    // 设置邮件内容
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message.c_str());

    // 启用调试模式
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // 发送邮件
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(recipient_list);
        return false;
    }

    // 清理
    curl_slist_free_all(recipient_list);
    return true;

}
