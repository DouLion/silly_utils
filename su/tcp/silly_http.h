/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 15:53
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_HTTP_H
#define SILLY_UTILS_SILLY_HTTP_H
#include <iostream>
#include <map>
#include <vector>

class silly_http
{
	std::string request_get(const std::string& url, const std::map<std::string, std::string>& headers);
	std::string request_post(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers);

	std::string request_download(const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers);
	std::string request_upload(const std::string& url, const std::string& body, const std::vector<std::string> files, const std::map<std::string, std::string>& headers);
};

#endif //SILLY_UTILS_SILLY_HTTP_H
