/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/3/11 11:43
 * @version: 1.0.1
 * @description: redis¿Í»§¶Ë
 */
#ifndef SILLY_UTILS_SILLY_REDIS_CLIENT_H
#define SILLY_UTILS_SILLY_REDIS_CLIENT_H
#include <su_marco.h>

class silly_redis_client
{
  public:
    bool init(const std::string& ip, const int& port, const std::string pwd = "");



  private:
    std::string m_ip;
    std::string m_pwd;
    int m_port;

};

#endif  // SILLY_UTILS_SILLY_REDIS_CLIENT_H
