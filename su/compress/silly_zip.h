/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 11:11
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_ZIP_H
#define SILLY_UTILS_SILLY_ZIP_H
#include <iostream>

class silly_zip
{
public:
	bool compress(const std::string& src, std::string& dst) { return false; };
	bool decompress(const std::string& src, std::string& dst) { return false; };

};

#endif //SILLY_UTILS_SILLY_ZIP_H
