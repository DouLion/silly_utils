/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/8/9 10:46
 * @version: 1.0.1
 * @description: silly_mail_send 类实现
 */
#include "silly_mail_send.h"
#include <files/silly_file.h>
#include <curl/curl.h>

struct silly_mail_send_upload_status
{
    size_t bytes_read;
};

silly_mail_send::~silly_mail_send()
{
}

void silly_mail_send::attach(const silly_mail_attachment& sma)
{
}

bool silly_mail_send::send()
{
    bool status = false;
    std::string readBuffer;
    // std::string payload_content = R"{}";
    CURL* curl;
    CURLcode res = CURLE_OK;
    struct curl_slist* recipients = NULL;
    struct silly_mail_send_upload_status upload_ctx = {0};
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: text/plain");
    headers = curl_slist_append(headers, "To: 734546831@qq.com");
    headers = curl_slist_append(headers, "Subject: Test email");
    curl = curl_easy_init();
    if (curl)
    {
        // 设置smtp服务器
        curl_easy_setopt(curl, CURLOPT_URL, m_smtp.c_str());
        // 设置用户验证
        if (!m_user.empty() && !m_password.empty())
        {
            /* Set the username and password */
            curl_easy_setopt(curl, CURLOPT_USERNAME, m_user.c_str());
            curl_easy_setopt(curl, CURLOPT_PASSWORD, m_password.c_str());

            /* Set the authorization identity (identity to act as) */
            curl_easy_setopt(curl, CURLOPT_SASL_AUTHZID, "ursel");

            /* Force PLAIN authentication */
            curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=PLAIN");
        }

        // 设置发送邮箱
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, m_from.c_str());
        for (auto rcvr : m_receivers)
        {
            recipients = curl_slist_append(recipients, std::get<0>(rcvr).c_str());
        }
        // 设置接受邮箱
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // 发送内容
       /*curl_easy_setopt(curl, CURLOPT_READFUNCTION, [payload_content](char* ptr, size_t size, size_t nmemb, void* userp) -> size_t {
            {
                struct silly_mail_send_upload_status* upload_ctx_bi = (struct silly_mail_send_upload_status*)userp;
                const char* data;
                size_t room = size * nmemb;

                if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1))
                {
                    return 0;
                }

                data = &payload_content[upload_ctx_bi->bytes_read];

                if (data)
                {
                    size_t len = strlen(data);
                    if (room < len)
                        len = room;
                    memcpy(ptr, data, len);
                    upload_ctx_bi->bytes_read += len;

                    return len;
                }

                return 0;
            }
        });*/
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_READDATA, &m_content);

        // 设置上传模式
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
#ifndef NDEBUG
        // 设置详细模式
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

        auto write_call_back = [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
            ((std::string*)userp)->append((char*)contents, size * nmemb);
            return size * nmemb;
        };
        // 设置回调函数
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_call_back);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // 设置 HTTP 头部
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        /* Send the message */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            status = true;
        }
            

        /* Free the list of recipients */
        curl_slist_free_all(recipients);

        /* curl does not send the QUIT command until you call cleanup, so you
         * should be able to reuse this connection for additional messages
         * (setting CURLOPT_MAIL_FROM and CURLOPT_MAIL_RCPT as required, and
         * calling curl_easy_perform() again. It may not be a good idea to keep
         * the connection open for a long time though (more than a few minutes may
         * result in the server timing out the connection), and you do want to
         * clean up in the end.
         */
        curl_easy_cleanup(curl);
    }

    return status;
}
void silly_mail_send::title(const std::string& str)
{
    m_title = str;
}

void silly_mail_send::content(const std::string& str)
{
    m_content = str;
}

void silly_mail_send::add_receiver(const silly_mail_addr_name& sman)
{
    m_receivers.push_back(sman);
}

void silly_mail_send::smtp(const std::string& str)
{
    m_smtp = str;
}

void silly_mail_send::imap(const std::string& str)
{
    m_imap = str;
}

void silly_mail_send::pop3(const std::string& str)
{
    m_pop3 = str;
}

void silly_mail_send::user(const std::string& str)
{
    m_user = str;
}

void silly_mail_send::password(const std::string& str)
{
    m_password = str;
}

void silly_mail_send::name(const std::string& str)
{
    m_name = str;
}
void silly_mail_send::from(const std::string& str)
{
    m_from = str;
}

void silly_mail_send::receivers(const std::vector<silly_mail_addr_name>& smans)
{
    m_receivers = smans;
}
