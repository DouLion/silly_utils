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
/// 同一个资源文件，不能同时读和写? 这一点存疑,不同系统上表现可能不同,需要想办法测试，否则指向文件的指针会乱掉，导致程序异常奔溃。
/// 参照: https://blog.csdn.net/baidu_38172402/article/details/106673606
/// https://zhuanlan.zhihu.com/p/477641987
/// https://learn.microsoft.com/zh-CN/windows/win32/api/winbase/nf-winbase-createfilemappinga
/// https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
/// </summary>

class silly_mmap
{
	
public:
	enum open_mode
	{
		NONE = 0, READONLY = 1, READWRITE = 2
	};
	silly_mmap() = default;
	silly_mmap(const std::string);
	~silly_mmap();

	/// <summary>
	/// 打开内存文件映射, windows 写文件有问题,需要处理
	/// </summary>
	/// <param name="file"></param>
	/// <param name="mode"></param>
	/// <returns></returns>
	bool mopen(const std::string& file, const int mode = open_mode::READONLY);

	/// <summary>
	/// 根据偏移量索引到内存位置
	/// </summary>
	/// <param name="offset"></param>
	/// <returns></returns>
	mmap_cur* at(const size_t& offset = 0);

	/// <summary>
	/// 读取内容
	/// </summary>
	/// <param name="dst">数据存位置</param>
	/// <param name="size">读取大小</param>
	/// <param name="offset">偏移位置</param>
	/// <returns></returns>
	bool read(mmap_cur* dst, const size_t& size, const size_t& offset = 0);

	/// <summary>
	/// TODO: 写入数据到内存文件映射,并且持久化到本地文件,这个还没有完全实现
	/// </summary>
	/// <param name="src"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool write(mmap_cur* src, const size_t& size, const size_t& offset = 0);

	/// <summary>
	/// 关闭,析构函数已经调用此函数,要注意
	/// </summary>
	void mclose();

    size_t size()
    {
        return m_size;
    }

private:
	std::string m_file;
	size_t m_size{ 0 };
	mmap_cur* m_mmap{ nullptr };

};

#endif //SILLY_UTILS_SILLY_MMAP_H
