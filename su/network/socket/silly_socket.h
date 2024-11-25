/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-11-22
 * @file: silly_socket.h
 * @description: silly_socket 类声明
 * @version: v1.0.1 2024-11-22 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SOCKET_H
#define SILLY_UTILS_SILLY_SOCKET_H
#include <su_marco.h>
#ifdef WIN32
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif
#include <openssl/ssl.h>
#include <openssl/err.h>
class silly_socket
{
  public:
    silly_socket();
    ~silly_socket();

    bool create(const std::string& hostname, const int& port, const bool& ssl = false);

    bool read(std::string& msg);

    bool write(const std::string& msg);

    bool listen(const std::string& hostname, const int& port, const bool& reuse_port = false);

    void verbose(const bool& v);

    std::string err();

    void release();
  private:
    bool m_connected = false;
    socket_t m_socket = 0;
    bool m_verbose = false;
    bool m_use_ssl = false;
    std::string m_err;
    SSL* m_ssl= nullptr;
    SSL_CTX* m_ssl_ctx = nullptr;
    std::mutex m_mutex;
};

#endif  // SILLY_UTILS_SILLY_SOCKET_H
