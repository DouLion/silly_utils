/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 11:10
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_BZ2_H
#define SILLY_UTILS_SILLY_BZ2_H
#include <iostream>

/*
 * bzip2比传统的gzip或者ZIP的压缩效率更高，但是压缩速度较慢。
 * 从这点来说，非常类似于较新的压缩算法。与RAR或者ZIP等其它不同的是，
 * bzip2只是数据压缩工具，而不是归档工具，在这一点与gzip类似。!!!!!!!!这一行比较重要!!!!!!!!!!!!!
 * 程序本身不包含用于多个文件、加密或者文档切分的工具，相反按照UNIX的传统需要使用如tar或者GnuPG这样的外部工具。
 * */

class silly_bz2
{
public:
	static std::string compress(const std::string& src_file, const std::string& dst_file = "");

	static std::string decompress(const std::string& src_file, const std::string& dst_file = "");

	static bool compress(const char* inval, const unsigned int& inlen, char** outval, unsigned int* outlen);
	static bool decompress(const char* inval, const unsigned int& inlen, char** outval, unsigned int* outlen);
};

#endif //SILLY_UTILS_SILLY_BZ2_H
