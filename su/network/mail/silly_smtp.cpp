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
#include <log/silly_log.h>
#include <files/silly_file.h>

const static std::string APP_OCT_STRM_CONTENT_ENCODE_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char* extract_before_at(const char* str)
{
    if (str == NULL)
    {
        return NULL;
    }
    std::string msg, code;
    auto ff = [msg, code]() -> bool {
        std::string _msg(msg), _code(code);
        for (int i = 0; i < msg.length(); i++)
        {
            if (msg[i] != code[i])
            {
                return false;
            }
        }
        return true;
    };

    // 查找 '@' 符号的位置
    const char* at_pos = strchr(str, '@');
    if (at_pos == NULL)
    {
        return NULL;  // 没有找到 '@'
    }

    // 计算 '@' 前面部分的长度
    size_t len = at_pos - str;

    // 分配内存并复制 '@' 前面的部分
    char* result = (char*)malloc(len + 1);
    if (result == NULL)
    {
        return NULL;  // 内存分配失败
    }

    strncpy(result, str, len);
    result[len] = '\0';  // 确保字符串以 null 结尾

    return result;
}
bool silly_smtp::logon()
{
#ifndef NDEBUG

    m_socket.verbose(true);
#endif
    if (m_connected)
    {
        return m_connected;
    }
    if (m_conn_opt.user.empty() || m_conn_opt.pwd.empty())
    {
        SLOG_ERROR("设置用户名和授权码!");
        return m_connected;
    }

    if (!m_socket.create(m_conn_opt.server, m_conn_opt.port, m_conn_opt.security == silly_mail_security_type::ssl))
    {
        SLOG_ERROR(m_socket.err());
        return m_connected;
    }
    std::string msg;
    if (!m_socket.read(msg) || !compare(msg, "220"))
    {
        SLOG_ERROR("连接服务器失败!");
        return m_connected;
    }

    char local_host[256];
    if (gethostname(local_host, 256) != 0)
    {
        SLOG_ERROR("Get local host name error!");
        return m_connected;
    }

    msg = "HELO ";
    msg += std::string(local_host) + "\r\n";
    m_socket.write(msg);
    if (!m_socket.read(msg) || !compare(msg, "250"))
    {
        return m_connected;
    }

    msg = "AUTH LOGIN\r\n";
    m_socket.write(msg);
    if (!m_socket.read(msg) || !compare(msg, "334"))
    {
        return m_connected;
    }

    // msg = Base64Encode(extract_before_at(m_conn_opt.user.c_str())) + "\r\n";
    msg = Base64Encode(m_conn_opt.user) + "\r\n";
    m_socket.write(msg);
    if (!m_socket.read(msg) || !compare(msg, "334"))
    {
        return m_connected;
    }

    msg = Base64Encode(m_conn_opt.pwd) + "\r\n";
    m_socket.write(msg);
    if (!m_socket.read(msg) || !compare(msg, "235"))
    {
        return m_connected;
    }
    m_connected = true;
    return m_connected;  // 登录成功
}
bool silly_smtp::logon(const std::string& account, const std::string& pwd)
{
    m_conn_opt.user = account;
    m_conn_opt.pwd = pwd;
    return logon();
}
bool silly_smtp::send(const silly_mail_content& content)
{
    bool status = false;
    if (!m_connected)
    {
        SLOG_ERROR("未登录!")
        return status;
    }
    if (!(status = head(content)))
    {
        SLOG_ERROR("发送头部信息失败!")
        return status;
    }
    SLOG_DEBUG("发送头部信息成功")
    if (!(status = body(content)))
    {
        SLOG_ERROR("发送正文失败!")
        return status;
    }
    SLOG_DEBUG("发送正文成功")
    if (!(status = attachments(content)))
    {
        SLOG_ERROR("发送附件失败!")
        return status;
    }

    SLOG_DEBUG("发送附件成功")
    if (!(status = end()))
    {
        SLOG_ERROR("结束发送失败!")
        return status;
    }
    SLOG_DEBUG("发送结束")

    return status;
}
void silly_smtp::conn_opt(const silly_mail_conn_opt& mco)
{
    m_conn_opt = mco;
}
std::string silly_smtp::Base64Encode(std::string str)
{
    std::string result;
    unsigned char c1, c2, c3;
    int i = 0;
    int len = str.length();

    while (i < len)
    {
        // read the first byte
        c1 = str[i++];
        if (i == len)  // pad with "="
        {
            result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[c1 >> 2];
            result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[(c1 & 0x3) << 4];
            result += "==";
            break;
        }

        // read the second byte
        c2 = str[i++];
        if (i == len)  // pad with "="
        {
            result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[c1 >> 2];
            result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
            result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[(c2 & 0xF) << 2];
            result += "=";
            break;
        }

        // read the third byte
        c3 = str[i++];
        // convert into four bytes string
        result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[c1 >> 2];
        result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
        result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];
        result += APP_OCT_STRM_CONTENT_ENCODE_CHARS[c3 & 0x3F];
    }

    return result;
}
std::string silly_smtp::prepareDate()
{
    char buff[256];
    time_t seconds;
    time(&seconds);
    // +0800 maybe hard code
    strftime(buff, 256, "%a, %d %b %y %H:%M:%S +0800", localtime(&seconds));

    return std::string(buff);
}

bool silly_smtp::head(const silly_mail_content& content)
{
    std::string msg;

    msg = "MAIL FROM:<";
    msg += m_conn_opt.user + ">\r\n";
    m_socket.write(msg);
    if (!m_socket.read(msg) || !compare(msg, "250"))
    {
        SLOG_ERROR(std::string("邮件地址错误") + m_conn_opt.user);
        return false;
    }

    // 遍历获得receiver
    for (auto [mail, name] : content.receivers)
    {
        msg = "RCPT TO: <";
        msg += mail + ">\r\n";
        m_socket.write(msg);
        if (!m_socket.read(msg) || !compare(msg, "250"))
        {
            return false;
        }
    }

    msg = "DATA\r\n";
    m_socket.write(msg);
    if (!m_socket.read(msg) || !compare(msg, "354"))
    {
        return false;
    }

    // 发送Headers
    msg = "From:\"" + content.sender.second + "\"<" + content.sender.first + ">\r\n";

    // 遍历receiver
    msg += "To: ";
    for (auto [mail, name] : content.receivers)
    {
        std::string szRecv;
        szRecv = "\"" + name + "\"<" + mail + ">, ";
        msg += szRecv;
    }
    msg += "\r\n";

    msg += "Date: ";
    msg += prepareDate() + "\r\n";

    msg += "Subject: ";
    msg += content.subject + "\r\n";

    msg += "X-Mailer: six_beauty \r\n";

    msg += "MIME-Version: 1.0\r\n";
    msg += "Content-type: multipart/mixed;  boundary=\"INVT\"\r\n\r\n";

    msg += "\r\n";
    SLOG_DEBUG("\n" + msg)
    m_socket.write(msg);

    return true;
}
bool silly_smtp::body(const silly_mail_content& content)
{
    std::string msg;
    msg = "--INVT\r\nContent-Type: text/plain;\r\n  charset=\"utf-8\"\r\n\r\n";
    msg += content.body;
    msg += "\r\n\r\n";
    SLOG_DEBUG("\n" + msg)
    int len_s = m_socket.write(msg);

    if (len_s != msg.length())
    {
        SLOG_ERROR("发送邮件正文出错，应该发送长度（{}}）：实际发送长度（{}）", msg.length(), len_s);
        return false;
    }

    return true;
}
bool silly_smtp::attachments(const silly_mail_content& content)
{
    std::string msg;
    // 遍历发送附件文件
    for (const auto& file : content.attachments)
    {
        std::string fileName = std::filesystem::path(file).filename().string();
        std::string szContent;
        if (!silly_file::read(file, szContent))
        {
            SLOG_ERROR("读取附件失败：{}", file)
            return false;
        }

        msg = "--INVT\r\nContent-Type: application/octet-stream;\r\n  name=\"";
        msg += fileName;
        msg += "\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;\r\n  filename=\"";
        msg += fileName;
        msg += "\"\r\n\r\n";
        m_socket.write(msg);

        int npos = 0, len = szContent.length();
        while (npos < len)
        {
            std::string szBuffer = Base64Encode(szContent.substr(npos, std::min(len - npos, 3000)));
            szBuffer += "\r\n";
            m_socket.write(szBuffer);
            npos += std::min(len - npos, 3000);
        }
    }

    return true;
}

bool silly_smtp::end()
{
    bool status = true;
    std::string msg;

    msg = "--INVT--\r\n.\r\n";
    m_socket.write(msg);

    msg = "QUIT\r\n";
    SLOG_DEBUG("\n" + msg)
    m_socket.write(msg);

    return status;
}
bool silly_smtp::quit()
{
    m_socket.release();
    return true;
}
bool silly_smtp::compare(const std::string& msg, const std::string& code)
{
    for (int i = 0; i < code.length(); i++)
        if (msg[i] != code[i])
            return false;
    return true;
}
