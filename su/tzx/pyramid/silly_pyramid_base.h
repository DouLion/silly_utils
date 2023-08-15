/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:47
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_IMAGE_BASE_H
#define SILLY_UTILS_SILLY_IMAGE_BASE_H

#include "files/silly_mmap.h"
#include <mutex>

/// 金字塔文件描述信息偏移量即信息长度
#define PYRAMID_DESC_OFFSET		0
#define PYRAMID_DESC_LENGTH		4
/// 金字塔文件主版本信息偏移量即信息长度
#define PYRAMID_MVER_OFFSET		4
#define PYRAMID_MVER_LENGTH		2
/// 金字塔文件次版本信息偏移量即信息长度
#define PYRAMID_PVER_OFFSET		6
#define PYRAMID_PVER_LENGTH		2

enum err_code {
	OK = 0,
	UNKNOWN = 1,
	NOT_OPEN = 2
};

enum pyramid_version {
	Version_1 = 1,
	Version_2 = 2,
	Version_3 = 3,
	Version_4 = 4,
	Version_5 = 5,
	Version_6 = 6
};


class silly_pyramid_base
{
/// <summary>
/// 读取时使用mmap, 创建和修改时使用普通文件流
/// </summary>
public:
	silly_pyramid_base() = default;
	/// <summary>
	/// 
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="mode">1 读 2 写</param>
	/// <returns></returns>
	bool open(const char* file, const open_mode& mode);

	/// <summary>
	/// 关闭文件
	/// </summary>
	/// <returns></returns>
	bool close();


	/// <summary>
	/// 读取指定大小的数据块
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool read(char* data, const size_t& size, const size_t& offset);

	/// <summary>
	/// 写入指定大小的数据库
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool write(char* data, const size_t& size, const size_t& offset);


	/// <summary>
	/// fseek
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="flag"></param>
	void seek(const size_t& pos = 0, const int& flag = SEEK_SET);

protected:

	/// <summary>
	/// 普通文件流打开
	/// </summary>
	/// <param name="file"></param>
	/// <param name="mode"></param>
	/// <returns></returns>
	bool stream_open(const char* file, const char* mode);

	/// <summary>
	/// 内存文件映射打开
	/// </summary>
	/// <param name="file"></param>
	/// <returns></returns>
	bool mmap_open(const char* file);

	/// <summary>
	/// 文件流读取
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool stream_read(char* data, const size_t& size, const size_t& offset);

	/// <summary>
	/// 内存文件映射读取
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool mmap_read(char* data, const size_t& size, const size_t& offset);


	/// <summary>
	/// 文件流写入
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool stream_write(char* data, const size_t& size, const size_t& offset);

	/// <summary>
	/// TODO: 内存文件映射写入, 这个目前有问题,暂不实现
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool mmap_write(char* data, const size_t& size, const size_t& offset);

	/// <summary>
	/// 关闭文件流
	/// </summary>
	void stream_close();

	/// <summary>
	/// 关闭内存文件映射
	/// </summary>
	void mmap_close();
	err_code read_info();

	void write_info();

protected:
	// 文件名称
	std::string									m_pfile;
	// 是否为普通文件流
	bool										m_normal{true};
	// mmap文件
	silly_mmap								    m_mmap;
	// 文件流
	FILE*										m_stream{nullptr};
	// 主版本号
	unsigned short								m_major_ver{2};
	// 次版本号
	unsigned short								m_primary_ver{0};

	unsigned int								m_version{2<<2|0};
	// 头信息
	char									    m_desc[4];
	// 多线程读写时用的锁
	std::mutex									m_mutex;
};

#endif //SILLY_UTILS_SILLY_IMAGE_BASE_H
