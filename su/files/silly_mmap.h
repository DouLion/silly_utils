/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 18:06
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_MMAP_H
#define SILLY_UTILS_SILLY_MMAP_H
#include <iostream>



typedef char mmap_cur;

/// <summary>  
/// 内存文件映射功能.
/// 同一个资源文件，不能同时读和写，否则指向文件的指针会乱掉，导致程序异常奔溃。
/// 参照: https://blog.csdn.net/baidu_38172402/article/details/106673606
/// https://zhuanlan.zhihu.com/p/477641987
/// </summary>

class silly_mmap
{
	
public:
	enum open_mode
	{
		READONLY, READWRITE, COPYONWRITE
	};
	silly_mmap() = default;
	silly_mmap(const std::string);
	~silly_mmap();

	bool mopen(const std::string& file, const int mode = open_mode::READONLY);

	mmap_cur* at(const size_t& offset = 0);

	bool read(mmap_cur* dst, const size_t& size, const size_t& offset = 0);

	bool write(mmap_cur* src, const size_t& size, const size_t& offset = 0);

	void mclose();


private:
	std::string m_file;
	size_t m_size{ 0 };
	mmap_cur* m_mmap{ nullptr };

};

#endif //SILLY_UTILS_SILLY_MMAP_H
