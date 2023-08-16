/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:49
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PYRAMID_TOOLS_H
#define SILLY_UTILS_SILLY_PYRAMID_TOOLS_H

#include "tzx/pyramid/silly_pyramid_info.h"
#include "tzx/pyramid/silly_pyramid_index.h"
#include "tzx/pyramid/silly_pyramid_data.h"


#include <mutex>
#include <thread>


/// <summary>
/// TzxImage ��ʽ���ļ���
/// </summary>
class silly_pyramid
{
public:
	silly_pyramid(void) = default;

	bool open(const std::string& root, const open_mode& mode = open_mode::READ);
	void close();

	/// <summary>
	/// ��ȡָ�������������
	/// </summary>
	/// <param name="layer">�����ڲ�� 0 - 24</param>
	/// <param name="row">�������к� 0 - 2^layer-1</param>
	/// <param name="col">�������к� 0 - 2^layer-1</param>
	/// <param name="size">�����ݴ�С</param>
	/// <returns>��������</returns>
	char* read_data(const uint32_t& layer, const uint64_t& row, const uint64_t& col, size_t& size);

	/// <summary>
	/// ��ָ����д������
	/// </summary>
	/// <param name="layer"></param>
	/// <param name="row"></param>
	/// <param name="col"></param>
	/// <param name="size"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	bool write(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const size_t& size, const char* data);

private:
	open_mode						m_mode;
	std::string						m_root;
	silly_pyramid_info				m_info;
	silly_pyramid_index				m_index;
	silly_pyramid_data				m_data;
};

class silly_pyramid_tools
{

};

#endif //SILLY_UTILS_SILLY_PYRAMID_TOOLS_H
