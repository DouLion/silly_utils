//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_index.h"

silly_pyramid_index::silly_pyramid_index()
{
	m_desc[0] = 'I';
	m_desc[1] = 'H';
	m_desc[2] = 'D';
	m_desc[3] = 'R';
	for (int i = 0; i <= TZX_IMAGE_MAX_LEVEL; ++i)
	{
		m_layer_infos[i] = layer_info();
	}
}

bool silly_pyramid_index::open(const char* file, const silly_mmap::open_mode& mode, const bool& usemmap)
{
	if (!silly_pyramid_base::open(file, mode, usemmap))
	{
		return false;
	}
	if (mode == silly_mmap::open_mode::READONLY)
	{
		return init_layer_info();
	}
	return true;
}

err_code silly_pyramid_index::read_block_pos(uint32_t layer, uint64_t row, uint64_t col, uint32_t& datasize, uint64_t& datapos)
{
	if (row > m_layer_infos[layer].end_row || row < m_layer_infos[layer].start_row || col > m_layer_infos[layer].end_col || col < m_layer_infos[layer].start_col) {
		datasize = 0;
		datapos = 0;
		return err_code::OK;
	}

	uint64_t pos = m_layer_bpos[layer];
	if (0 == pos) {
		datasize = 0;
		datapos = 0;
		return err_code::OK;
	}
	pos += ((m_layer_infos[layer].end_col - m_layer_infos[layer].start_col + 1) * (row - m_layer_infos[layer].start_row) + (col - m_layer_infos[layer].start_col)) * (TZX_IMAGE_DATA_POS_SIZE + TZX_IMAGE_DATA_SIZE_SIZE);

	datasize = datapos = 0;
	char buff[TZX_IMAGE_DATA_POS_SIZE + TZX_IMAGE_DATA_SIZE_SIZE] = { 0 };
	read(pos, buff, TZX_IMAGE_DATA_POS_SIZE + TZX_IMAGE_DATA_SIZE_SIZE);
	// read(pos + TZX_IMAGE_DATA_POS_SIZE, (char*)(&datasize), TZX_IMAGE_DATA_SIZE_SIZE);
	datapos = ((uint64_t*)buff)[0];
	datasize = ((uint32_t*)buff)[2];
	return err_code::OK;
}

bool silly_pyramid_index::init_layer_info()
{
	if (pyramid_version::Version_2 == m_major_ver)
	{
		uint64_t pos = TZX_IMAGE_INDEX_INFO_SIZE;
		for (uint8_t l = 0; l < TZX_IMAGE_MAX_LEVEL; ++l)
		{
			char buff[40];
			read(pos, buff, 40);
			uint64_t begrow = ((uint64_t*)(buff))[0];
			uint64_t begcol = ((uint64_t*)(buff))[1];
			uint64_t endrow = ((uint64_t*)(buff))[2];
			uint64_t endcol = ((uint64_t*)(buff))[3];
			uint64_t begpos = ((uint64_t*)(buff))[4];
			pos += 5 * sizeof(uint64_t);
			m_layer_infos[l].start_row = begrow;
			m_layer_infos[l].start_col = begcol;
			m_layer_infos[l].end_row = endrow;
			m_layer_infos[l].end_col = endcol;
			m_layer_bpos[l] = begpos;
		}
	}
	else if (pyramid_version::Version_1 == m_major_ver) {
		uint8_t beglyr, endlyr = 0;

		uint64_t pos = TZX_IMAGE_INDEX_INFO_SIZE;
		read(pos, (char*)(&beglyr), TZX_IMAGE_LAYER_SIZE);
		pos += TZX_IMAGE_LAYER_SIZE;
		read(pos, (char*)(&endlyr), TZX_IMAGE_LAYER_SIZE);
		pos += TZX_IMAGE_LAYER_SIZE;

		int sss = sizeof(uint32_t);
		for (uint8_t l = beglyr; l <= endlyr; ++l)
		{
			char buff[16];
			read(pos, buff, 16);
			m_layer_infos[l].start_row = ((uint32_t*)(buff))[0];
			m_layer_infos[l].start_col = ((uint32_t*)(buff))[1];
			m_layer_infos[l].end_row = ((uint32_t*)(buff))[2];
			m_layer_infos[l].end_col = ((uint32_t*)(buff))[3];
			pos += TZX_IMAGE_COLROW_SIZE * 4;
		}

		for (uint8_t l = beglyr; l <= endlyr; ++l)
		{
			m_layer_bpos[l] = pos;
			pos += (m_layer_infos[l].end_row - m_layer_infos[l].start_row + 1) * (m_layer_infos[l].end_col - m_layer_infos[l].start_col + 1) * (TZX_IMAGE_DATA_SIZE_SIZE + TZX_IMAGE_DATA_POS_SIZE);
		}
	}
	else
	{
		return false;
	}

	return true;

}

uint64_t silly_pyramid_index::get_layer_start_pos(const uint32_t& layer)
{
	return 0;
}
void silly_pyramid_index::set_layer_info(const uint32_t& layer, const layer_info& linfo)
{
	m_layer_infos[layer] = linfo;
	uint64_t pos = TZX_IMAGE_INDEX_DATA_BEGIN_POS;
	// 重新构建索引
	for (auto [l, info] : m_layer_infos)
	{
		m_layer_bpos[l] = pos;
		uint64_t rows = info.end_row - info.start_row + 1;
		uint64_t cols = info.end_col - info.start_col + 1;
		pos += (rows * cols * (TZX_IMAGE_DATA_POS_SIZE + TZX_IMAGE_DATA_SIZE_SIZE));

	}
}
bool silly_pyramid_index::write_block(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const block_index& idata)
{
	write(idata.offset, (char*)(&idata.pos), sizeof(idata.pos), 0);
	write(sizeof(idata.pos), (char*)(&idata.size), sizeof(idata.size), 0);
	return true;
}

void silly_pyramid_index::write_layer_info()
{
	if (m_stream)
	{
		size_t pos = PYRAMID_DESC_LENGTH + PYRAMID_MVER_LENGTH + PYRAMID_PVER_LENGTH;
		for (auto [l, info] : m_layer_infos)
		{
			write(pos, (char*)(&info.start_row), sizeof(info.start_row), 0);
			pos += sizeof(info.start_row);
			write(pos, (char*)(&info.start_col), sizeof(info.start_col), 0);
			pos += sizeof(info.start_col);
			write(pos, (char*)(&info.end_row), sizeof(info.end_row), 0);
			pos += sizeof(info.end_row);
			write(pos, (char*)(&info.end_col), sizeof(info.end_col), 0);
			pos += sizeof(info.end_col);
			write(pos, (char*)(&m_layer_bpos[l]), sizeof(m_layer_bpos[l]), 0);
			pos += sizeof(m_layer_bpos[l]);
		}
	}
}

bool silly_pyramid_index::close()
{
	if (m_mode != silly_mmap::open_mode::READONLY)
	{
		write_layer_info();
	}
	
	return silly_pyramid_base::close();
}
