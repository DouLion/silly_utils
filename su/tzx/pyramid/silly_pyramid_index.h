/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:48
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PYRAMID_INDEX_H
#define SILLY_UTILS_SILLY_PYRAMID_INDEX_H

#define TZX_IMAGE_DATA_INDEX_NAME		"TzxImage.index"

#include "tzx/pyramid/silly_pyramid_base.h"
#include <map>

#define TZX_IMAGE_DATA_POS_SIZE					8		// ��¼data����λ�õ����ݴ�С
#define TZX_IMAGE_DATA_SIZE_SIZE				4		// ��¼data���ݴ�С�����ݵĴ�С
#define TZX_IMAGE_INDEX_INFO_SIZE				8
#define TZX_IMAGE_LAYER_SIZE					1
#define TZX_IMAGE_COLROW_SIZE					4

#define TZX_IMAGE_MAX_LEVEL						24
#define TZX_IMAGE_INDEX_DATA_BEGIN_POS			1024

struct block_index
{
	uint64_t offset;
	uint64_t pos;
	uint32_t size;
};

struct layer_info
{
	uint64_t start_row{0};
	uint64_t end_row{0};
	uint64_t start_col{0};
	uint64_t end_col{0};

	layer_info& operator=(const layer_info& other)
	{
		this->start_row = other.start_row;
		this->end_row = other.end_row;
		this->start_col = other.start_col;
		this->end_col = other.end_col;
		return *this;
	}
};


class silly_pyramid_index : public silly_pyramid_base
{
public:
	silly_pyramid_index();

	/// <summary>
	/// ��ʼ������Ĺؼ���Ϣ, �ӿ����ļ���
	/// </summary>
	/// <returns></returns>
	bool init_layer_info();

	bool open(const char* file, const open_mode& mode, const bool& usemmap);

	err_code read_block_pos(uint32_t layer, uint64_t row, uint64_t col, uint32_t& datasize, uint64_t& datapos);
	

	/// <summary>
	/// ��ȡ�����ļ�,��Ӧ��������ʼλ��
	/// </summary>
	/// <param name="layer"></param>
	/// <returns></returns>
	uint64_t get_layer_start_pos(const uint32_t& layer);

	/// <summary>
	/// ����ÿ�����ʼ��Ϣ
	/// </summary>
	/// <param name="linfo"></param>
	void set_layer_info(const uint32_t& layer, const layer_info& linfo);


	/// <summary>
	/// д�����ݿ��е�ָ������������� ����ƫ���������ݿ��С
	/// </summary>
	/// <param name="layer"></param>
	/// <param name="row"></param>
	/// <param name="col"></param>
	/// <param name="idata"></param>
	/// <returns></returns>
	bool write_block(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const block_index& idata);


	void write_layer_info();

	bool close();

public:
	std::map<uint32_t, layer_info> m_layer_infos;
	// �����ļ���,ÿ��洢data��Ϣ����ʼλ��
	std::map<uint32_t, uint64_t> m_layer_bpos;
	
};

#endif //SILLY_UTILS_SILLY_PYRAMID_INDEX_H

