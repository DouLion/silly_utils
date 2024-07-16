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
silly_curl::silly_curl()
{
   /* // 初始化curl库，检查返回值以确保成功
    if(curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        // 处理初始化失败的情况，比如抛出异常或者记录错误日志
        throw std::runtime_error("Failed to initialize CURL");
    }
    m_curl = curl_easy_init();
    if(!m_curl) {
        // 初始化失败时清理全局状态并处理错误
        throw std::runtime_error("Failed to initialize easy handle");
    }*/
}
silly_curl::~silly_curl()
{
    /*if(m_curl)
    {
        curl_easy_cleanup(m_curl); // 移除不必要的类型转换，因为m_curl已经是CURL*
    }*/
}
