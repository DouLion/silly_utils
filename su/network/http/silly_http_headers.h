/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-09
 * @file: silly_http_headers.h
 * @description: silly_http_headers 类声明
 * @version: v1.0.1 2024-10-09 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_HTTP_HEADERS_H
#define SILLY_UTILS_SILLY_HTTP_HEADERS_H
#include <su_marco.h>
/*using Headers =
    std::unordered_multimap<std::string, std::string, detail::case_ignore::hash,
                            detail::case_ignore::equal_to>;*/
class silly_http_headers
{
  public:
    silly_http_headers();
    ~silly_http_headers();

  private:
};

#endif  // SILLY_UTILS_SILLY_HTTP_HEADERS_H
