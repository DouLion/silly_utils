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
typedef SOCKET su_socket_t;
#else

#endif

class silly_socket
{
  public:
    silly_socket();
    ~silly_socket();


    bool connect(const std::string& ip, const int& port);


  private:

};

#endif  // SILLY_UTILS_SILLY_SOCKET_H
