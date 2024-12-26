/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/7/16 13:40
 * @version: 1.0.1
 * @description:
 */
//
// Created by dell on 2024/7/16. 实现
//

#include "silly_curl.h"
#include <curl/curl.h>

using namespace silly_curl;

static size_t smtp_read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::ifstream *file = static_cast<std::ifstream *>(stream);
    if (!file || file->eof())
    {
        return 0;
    }
    file->read(static_cast<char *>(ptr), size * nmemb);
    return file->gcount();
}

bool smtp::send(const silly_curl::smtp_req &req)
{
    CURL *curl;
    CURLcode res = CURLE_OK;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, req.server.c_str());

        if (req.ssl)
        {
            curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        }

        //   curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<sender@example.com>");
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, req.sender.c_str());

        struct curl_slist *recipients = nullptr;
        for (auto [nike, receiver] : req.nick_receivers)
        {
            //  recipients = curl_slist_append(recipients, "<recipient@example.com>");
            recipients = curl_slist_append(recipients, receiver.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // curl_easy_setopt(curl, CURLOPT_USERNAME, "<your_username>");
        //         curl_easy_setopt(curl, CURLOPT_PASSWORD, "<your_password>");
        curl_easy_setopt(curl, CURLOPT_USERNAME, req.user.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, req.pwd.c_str());

        // 定义邮件的boundary，用于分割邮件的各个部分
        std::stringstream boundary_stream;
        boundary_stream << "BOUNDARY_" << std::hex << std::random_device{}();
        std::string boundary = boundary_stream.str();

        // 构建邮件正文的初始部分
        std::ostringstream maildata;
        maildata << "To: Recipient <recipient@example.com>\r\n"
                 << "From: Sender <sender@example.com>\r\n"
                 << "Subject: Email with multiple attachments\r\n"
                 << "MIME-Version: 1.0\r\n"
                 << "Content-Type: multipart/mixed; boundary=" << boundary << "\r\n\r\n";

        // 添加邮件正文文本
        maildata << "--" << boundary << "\r\n"
                 << "Content-Type: text/plain; charset=UTF-8\r\n\r\n"
                 << "Hello,\r\nThis email contains multiple attachments.\r\n"
                 << "--" << boundary << "\r\n";

        // 准备附件
        std::vector<std::pair<std::string, std::string>> attachments = {
            {"path_to_attachment1.txt", "attachment1.txt"}, {"path_to_attachment2.pdf", "attachment2.pdf"}
            // 可以继续添加更多附件
        };

        // 遍历附件列表并添加到邮件中
        for (const auto &attachment : attachments)
        {
            // 打开附件文件
            std::ifstream file(attachment.first, std::ios::binary);
            if (!file.is_open())
            {
                std::cerr << "Failed to open attachment file: " << attachment.first << std::endl;
                return false;
            }

            // 添加附件部分的头部信息
            maildata << "Content-Type: application/octet-stream; name=\"" << attachment.second << "\"\r\n"
                     << "Content-Disposition: attachment; filename=\"" << attachment.second << "\"\r\n\r\n";

            // 将文件内容读入邮件正文
            std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            maildata << buffer << "\r\n";

            // 关闭文件
            file.close();
            maildata << "--" << boundary << "\r\n";
        }

        // 结束边界
        maildata << "--" << boundary << "--\r\n";

        // 设置邮件正文
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, smtp_read_callback);

        // 注意：这里直接将整个邮件内容构造成一个字符串可能不适合大型邮件或大量附件。
        // 实际应用中，可以考虑分块读取和发送数据。
        std::istringstream iss(maildata.str());
        curl_easy_setopt(curl, CURLOPT_READDATA, &iss);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)maildata.str().size());

        // 执行发送
        res = curl_easy_perform(curl);

        // 清理
        if (recipients)
            curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    return false;
}
