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

/// �������ļ�������Ϣƫ��������Ϣ����
#define PYRAMID_DESC_OFFSET		0
#define PYRAMID_DESC_LENGTH		4
/// �������ļ����汾��Ϣƫ��������Ϣ����
#define PYRAMID_MVER_OFFSET		4
#define PYRAMID_MVER_LENGTH		4
/// �������ļ��ΰ汾��Ϣƫ��������Ϣ����
#define PYRAMID_PVER_OFFSET		8
#define PYRAMID_PVER_LENGTH		4

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
/// ��ȡʱʹ��mmap, �������޸�ʱʹ����ͨ�ļ���
/// </summary>
public:
	silly_pyramid_base() = default;
	/// <summary>
	/// 
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="mode">1 �� 2 д</param>
	/// <returns></returns>
	bool open(const char* file, const open_mode& mode);

	/// <summary>
	/// �ر��ļ�
	/// </summary>
	/// <returns></returns>
	bool close();


	/// <summary>
	/// ��ȡָ����С�����ݿ�
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool read(size_t seek_offset, char* data, const size_t& size, const size_t& offset = 0);

	/// <summary>
	/// д��ָ����С�����ݿ�
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool write(size_t seek_offset, char* data, const size_t& size, const size_t& offset = 0);


	/// <summary>
	/// fseek
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="flag"></param>
	void seek(const size_t& pos = 0, const int& flag = SEEK_SET);

protected:

	/// <summary>
	/// ��ͨ�ļ�����
	/// </summary>
	/// <param name="file"></param>
	/// <param name="mode"></param>
	/// <returns></returns>
	bool stream_open(const char* file, const char* mode);

	/// <summary>
	/// �ڴ��ļ�ӳ���
	/// </summary>
	/// <param name="file"></param>
	/// <returns></returns>
	bool mmap_open(const char* file);

	/// <summary>
	/// �ļ�����ȡ
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool stream_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset);

	/// <summary>
	/// �ڴ��ļ�ӳ���ȡ
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool mmap_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset);


	/// <summary>
	/// �ļ���д��
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool stream_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset);

	/// <summary>
	/// TODO: �ڴ��ļ�ӳ��д��, ���Ŀǰ������,�ݲ�ʵ��
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	bool mmap_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset);

	/// <summary>
	/// �ر��ļ���
	/// </summary>
	void stream_close();

	/// <summary>
	/// �ر��ڴ��ļ�ӳ��
	/// </summary>
	void mmap_close();
	err_code read_info();

	void write_info();

protected:
	// �ļ�����
	std::string									m_pfile;

	// �Ƿ��Ѿ���
	bool										m_opened{false};
	// �Ƿ�Ϊ��ͨ�ļ���
	bool										m_normal;
	// mmap�ļ�
	silly_mmap								    m_mmap;
	// �ļ���
	FILE*										m_stream;
	// ���汾��	���������ڴ�������ΪС���� ��д, ���ĵ��������г���
	unsigned int								m_major_ver{2};
	// �ΰ汾��
	unsigned int								m_primary_ver{0};

	// ͷ��Ϣ
	char									    m_desc[4];
	// ���̶߳�дʱ�õ���
	std::mutex									m_mutex;
	// ��������
	open_mode									m_mode;
};

#endif //SILLY_UTILS_SILLY_IMAGE_BASE_H
