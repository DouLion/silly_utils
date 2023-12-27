/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/3 11:32
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_FILE_H
#define SILLY_UTILS_SILLY_FILE_H
#include <filesystem>
#include <files/TFF_FileUtils.h>
#include <su_marco.h>

class silly_file
{
public:
	/// <summary>
	/// 读取文件内容
	/// </summary>
	/// <param name="path">文件路径</param>
	/// <param name="content">内容接受对象</param>
	/// <param name="offset">偏移位置</param>
	/// <param name="len">预定读取大小</param>
	/// <returns>实际读取大小</returns>
	static size_t read(const std::string &path, std::string &content, const size_t &offset = 0, const size_t &len = SIZE_MAX);
};

#endif // SILLY_UTILS_SILLY_FILE_H
