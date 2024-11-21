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
#include <string/silly_algorithm.h>

const static std::string APP_OCT_STRM_CONTENT_ENCODE_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char* extract_before_at(const char* str)
{
    if (str == NULL)
    {
        return NULL;
    }

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
    if (m_connected)
    {
        return m_connected;
    }
    if (m_conn_opt.user.empty() || m_conn_opt.pwd.empty())
    {
        SLOG_ERROR("设置用户名和授权码!");
        return m_connected;
    }

    if (!create_socket())
    {
        return m_connected;
    }
    char local_host[256];
    if (gethostname(local_host, 256) != 0)
    {
        SLOG_ERROR("Get local host name error!");
        return m_connected;
    }

    std::string msg;

    msg = "HELO ";
    msg += std::string(local_host) + "\r\n";
    req(msg);
    if (!resp("250"))
    {
        return m_connected;
    }

    msg = "AUTH LOGIN\r\n";
    req(msg);
    if (!resp("334"))
    {
        return m_connected;
    }

    msg = Base64Encode(extract_before_at(m_conn_opt.user.c_str())) + "\r\n";
    req(msg);
    if (!resp("334"))
    {
        return m_connected;
    }

    msg = Base64Encode(m_conn_opt.pwd) + "\r\n";
    req(msg);
    if (!resp("235"))
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

bool silly_smtp::create_socket()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        SLOG_ERROR("WSAStartup调用失败!");
        return false;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        return false;
    }
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (m_socket == INVALID_SOCKET)
    {
        SLOG_ERROR("socket创建失败!");
        return false;
    }

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(m_conn_opt.port);  // 发邮件一般都是25端口

    struct hostent* hp = gethostbyname(m_conn_opt.server.c_str());  // 使用名称
    if (hp == NULL)
    {
        DWORD dwErrCode = GetLastError();
        return false;
    }
    serv_addr.sin_addr.s_addr = *(int*)(*hp->h_addr_list);

    int ret = connect(m_socket, (sockaddr*)&serv_addr, sizeof(serv_addr));  // 建立连接
    if (ret == SOCKET_ERROR)
    {
        DWORD dwErr = GetLastError();
        return false;
    }
    if (!resp("220"))
        return false;
    return true;
}
bool silly_smtp::head(const silly_mail_content& content)
{
    std::string msg;

    msg = "MAIL FROM:<";
    msg += m_conn_opt.user + ">\r\n";
    req(msg);
    if (!resp("250"))
    {
        SLOG_ERROR(std::string("邮件地址错误") + m_conn_opt.user);
        return false;
    }

    // 遍历获得receiver
    for (auto [mail, name] : content.receivers)
    {
        msg = "RCPT TO: <";
        msg += mail + ">\r\n";
        req(msg);
        if (!resp("250"))
        {
            return false;
        }
    }

    msg = "DATA\r\n";
    req(msg);
    if (!resp("354"))
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
    req(msg);

    return true;
}
bool silly_smtp::body(const silly_mail_content& content)
{
    std::string msg;
    msg = "--INVT\r\nContent-Type: text/plain;\r\n  charset=\"utf-8\"\r\n\r\n";
    msg += content.body;
    msg += "\r\n\r\n";
    SLOG_DEBUG("\n" + msg)
    int len_s = req(msg, true);

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
        req(msg, true);

        int npos = 0, len = szContent.length();
        while (npos < len)
        {
            std::string szBuffer = Base64Encode(szContent.substr(npos, std::min(len - npos, 3000)));
            szBuffer += "\r\n";
            req(szBuffer);
            npos += std::min(len - npos, 3000);
        }
    }

    return true;
}

bool silly_smtp::resp(const std::string expected_response)
{
    int recv_bytes = 0;
    char response_buffer[256];
    if ((recv_bytes = recv(m_socket, response_buffer, 256, 0)) < 0)
    {
        SLOG_ERROR(std::string("RECV:\n") + expected_response);
        return false;
    }
    // 输出信息
    std::string response(response_buffer, recv_bytes);
    if (response.substr(0, 3) != expected_response)
    {
        return false;
    }
    return true;
}

int silly_smtp::req(const std::string content, bool bout)
{
    int len_s = ::send(m_socket, content.c_str(), content.length(), 0);
    if (len_s < 0)
    {
        SLOG_ERROR(std::string("SEND:\n") + content);
        return false;
    }
    // 输出信息
    if (bout)
    {
        SLOG_ERROR(std::string("SEND:\n") + content);
    }
    return len_s;
}
bool silly_smtp::end()
{
    bool status = true;
    std::string msg;

    msg = "--INVT--\r\n.\r\n";
    req(msg, true);

    msg = "QUIT\r\n";
    SLOG_DEBUG("\n" + msg)
    req(msg, true);

    if (SOCKET_ERROR == closesocket(m_socket))
    {
        SLOG_ERROR("关闭socket失败!")
        WSACleanup();
        status = false;
    }
    WSACleanup();

    return status;
}
