/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-22
 * @file: silly_socket.cpp
 * @description: silly_socket实现
 * @version: v1.0.1 2024-11-22 dou li yang
 */
#include "silly_socket.h"

bool silly_socket::create(const std::string& hostname, const int& port, const bool& ssl)
{
    std::scoped_lock<std::mutex> lock(m_mutex);
    if (m_connected)
    {
        return m_connected;
    }
    m_use_ssl = ssl;
#if defined(WIN32)
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    int ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret)
    {
        SLOG_ERROR("WSAStartup失败, {}!", WSAGetLastError())
        return m_connected;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        SLOG_ERROR("WSAStartup失败, {}!", WSAGetLastError())
        WSACleanup();
        return m_connected;
    }
    /* 创建一个 socket 用于 tcp 通信 */
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (m_socket == INVALID_SOCKET)
    {
        SLOG_ERROR("socket创建失败!");
        return m_connected;
    }
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    struct hostent* hp = gethostbyname(hostname.c_str());  // 使用名称
    if (hp == NULL)
    {
        DWORD dwErrCode = GetLastError();
        return m_connected;
    }
    serv_addr.sin_addr.s_addr = *(int*)(*hp->h_addr_list);

    ret = ::connect(m_socket, (sockaddr*)&serv_addr, sizeof(serv_addr));  // 建立连接
    if (ret == SOCKET_ERROR)
    {
        DWORD dwErr = GetLastError();
        return m_connected;
    }

#else
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, port, &hints, &res) != 0)
    {
        perror("getaddrinfo");
        return m_connected;
    }

    m_socket = create_tcp_socket();
    if (connect(m_socket, res->ai_addr, res->ai_addrlen) < 0)
    {
        perror("Connect error");
        freeaddrinfo(res);
        return m_connected;
    }

#endif
    if (m_use_ssl)
    {
        // if (!silly_utils_openssl_init_flag)
        {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_ssl_algorithms();
            // silly_utils_openssl_init_flag = true;
        }

        if (!(m_ssl_ctx = SSL_CTX_new(TLS_client_method())))
        {
            m_err = "SSL_CTX_new error";
            return m_connected;
        }

        if (!(m_ssl = SSL_new(m_ssl_ctx)))
        {
            m_err = "SSL_new error";
            return m_connected;
        }
        ret = SSL_set_fd(m_ssl, m_socket);
        if (ret <= 0)
        {
            m_err = SSL_get_error(m_ssl, ret);
            return m_connected;
        }
        SSL_connect(m_ssl);
    }
    m_connected = true;
    return m_connected;
}
bool silly_socket::read(std::string& msg)
{
    bool status = false;
    if (!m_connected)
        return status;

    do{
        if (m_ssl)
        {
            int num = 1024;
            int ret = SSL_read(m_ssl, &msg, num);
            if (ret != SSL_ERROR_NONE)
            {
                m_err = SSL_get_error(m_ssl, ret);
                break;
            }
        }
        else
        {
            char buff[1024];
#if defined(WIN32)

            int ret = ::recv(m_socket, buff, 1024, 0);
            if (ret > 0)
            {
                status = true;
            }

#else
            char buff[1024];
            int ret = recv(m_socket, buff, 1024, 0);

#endif
            msg.assign(buff, ret);
        }
    } while (0);

    if (m_verbose)
    {
        SLOG_INFO("read:{}", msg);
    }
    return status;
}
bool silly_socket::write(const std::string& msg)
{
    bool status = false;
    if (!m_connected)
        return status;
    if (msg.empty())
        return status;
    if (m_ssl)
    {
        SSL_write(m_ssl, msg.c_str(), msg.size());
    }
    else
    {
#if defined(WIN32)
        int ret = ::send(m_socket, msg.c_str(), msg.size(), 0);
        if (ret > 0)
        {
            status = true;
        }
#else
        send(m_socket, msg.c_str(), msg.size(), 0);
        status = true;
#endif
    }
    return status;
}
void silly_socket::verbose(const bool& v)
{
    m_verbose = v;
}
void silly_socket::release()
{
    std::scoped_lock<std::mutex> lock(m_mutex);
    if (m_connected)
    {
        if (m_ssl)
        {
            int max_times = 10;
            while (!SSL_shutdown(m_ssl) && max_times--)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            SSL_free(m_ssl);
        }

        if (m_ssl_ctx)
        {
            SSL_CTX_free(m_ssl_ctx);
        }

#if defined(WIN32)
        if (SOCKET_ERROR == closesocket(m_socket))
        {
            SLOG_ERROR("关闭socket失败!")
        }
        WSACleanup();
#else
        close(m_socket);
#endif
    }
}
silly_socket::~silly_socket()
{
}
std::string silly_socket::err()
{
    return m_err;
}
silly_socket::silly_socket()
{
}
bool silly_socket::listen(const std::string& hostname, const int& port, const bool& reuse_port)
{
    // TODO: 实现
    return false;
}
