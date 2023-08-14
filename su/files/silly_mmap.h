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


enum open_mode
{
	READ = 1,	// Ŀǰ��֧�ֶ�,��֧��д�͸�
	WRITE = 2,
	APP_WRITE = 3,
	TRUNC_WRITE = 4

};


typedef char mmap_cur;

/// <summary>  
/// �ڴ��ļ�ӳ�书��.
/// ͬһ����Դ�ļ�������ͬʱ����д������ָ���ļ���ָ����ҵ������³����쳣������
/// ����: https://blog.csdn.net/baidu_38172402/article/details/106673606
/// https://zhuanlan.zhihu.com/p/477641987
/// </summary>

class silly_mmap
{

public:
	silly_mmap() = default;
	silly_mmap(const std::string);
	~silly_mmap();

	bool open(const std::string& file, const int mode = open_mode::READ);

	mmap_cur* at(const size_t& offset = 0);

	bool read(mmap_cur* dst, const size_t& size, const size_t& offset = 0);

	bool write(mmap_cur* src, const size_t& size, const size_t& offset = 0);


private:
	std::string m_file;
	size_t m_size{ 0 };
	mmap_cur* m_mmap{ nullptr };

};

#endif //SILLY_UTILS_SILLY_MMAP_H
