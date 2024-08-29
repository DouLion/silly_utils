/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/7/16 13:40
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_CURL_H
#define SILLY_UTILS_SILLY_CURL_H
#include <su_marco.h>
/*class silly_http_request
{
    std::map<std::string, std::string> headers;
    std::string url;
    std::string data;
    std::map<std::string ,std::string> form_data;
    std::map<std::string, std::string> name_file;
    std::string user;
    std::string pwd;
};


class silly_http_response
{
    int code;
    std::map<std::string, std::string> headers;
    std::string body;
};*/

namespace silly_curl
{
class http
{
  public:
};

class ftp
{
  public:
};

class websocket
{
  public:
};

/// 代理
class proxy
{
  public:
};

class smtp_req
{
  public:
    std::string server;
    std::string sender;
    std::string user;
    std::string pwd;
    std::map<std::string, std::string> nick_receivers;
    std::string subject;
    std::string content;

    bool ssl{false};
    std::map<std::string, std::string> name_files;
};

/// 邮件
class smtp
{
  public:
    bool send(const smtp_req& req);
};

};  // namespace silly_curl

#endif  // SILLY_UTILS_SILLY_CURL_H
