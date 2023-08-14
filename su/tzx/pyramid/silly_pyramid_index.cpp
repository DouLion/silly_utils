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
	for (int i = 0; i <= MAX_LEVEL; ++i)
	{
		m_layer_infos[i] = layer_info();
	}
}

err_code silly_pyramid_index::read_block_pos(uint32_t layer, uint64_t row, uint64_t col, uint32_t& datasize, uint64_t& datapos)
{
	//if (row > lyrEndRow[layer] || row < lyrBeginRow[layer] || col > lyrEndCol[layer] || col < lyrBeginCol[layer]) {
	//	datasize = 0;
	//	datapos = 0;
	//	return err_code::OK;
	//}

	//uint64_t pos = lyrBeginPos[layer];
	//if (0 == pos) {
	//	datasize = 0;
	//	datapos = 0;
	//	return err_code::OK;
	//}
	//pos += ((lyrEndCol[layer] - lyrBeginCol[layer] + 1) * (row - lyrBeginRow[layer]) + (col - lyrBeginCol[layer])) * 12;
	////pos += (col - lyrBeginCol[layer]) * 12;

	//datasize = datapos = 0;
	//datapos = ((uint64_t*)(m_mmap + pos))[0];
	//datasize = ((uint32_t*)(m_mmap + pos + DATA_POS_SIZE))[0];
	return err_code::OK;
}

bool silly_pyramid_index::init_key_pos()
{
	//if (pyramid_version::Version_2 == m_version)
	//{
	//	uint64_t pos = INDEX_INFO_SIZE;
	//	//uint64_t begpos = 1024;
	//	for (uint8_t l = 0; l < MAX_LEVEL; ++l)
	//	{
	//		uint64_t begrow = ((uint64_t*)(m_mmap + pos))[0];
	//		uint64_t begcol = ((uint64_t*)(m_mmap + pos))[1];
	//		uint64_t endrow = ((uint64_t*)(m_mmap + pos))[2];
	//		uint64_t endcol = ((uint64_t*)(m_mmap + pos))[3];
	//		uint64_t begpos = ((uint64_t*)(m_mmap + pos))[4];
	//		pos += 5 * sizeof(uint64_t);
	//		lyrBeginRow[l] = begrow;
	//		lyrBeginCol[l] = begcol;
	//		lyrEndRow[l] = endrow;
	//		lyrEndCol[l] = endcol;
	//		lyrBeginPos[l] = begpos;
	//		//begpos += (endrow - begrow + 1) * (endcol - begcol + 1) * (DATA_SIZE_SIZE + DATA_POS_SIZE);
	//	}

	//	return true;
	//}
	//else {
	//	uint8_t beglyr, endlyr = 0;
	//	uint64_t pos = INDEX_INFO_SIZE;
	//	memcpy(&beglyr, m_mmap + pos, LAYER_SIZE);
	//	pos += LAYER_SIZE;
	//	memcpy(&endlyr, m_mmap + pos, LAYER_SIZE);
	//	pos += LAYER_SIZE;

	//	int sss = sizeof(uint32_t);
	//	for (uint8_t i = beglyr; i <= endlyr; ++i)
	//	{
	//		lyrBeginRow[i] = ((uint32_t*)(m_mmap + pos))[0];
	//		lyrBeginCol[i] = ((uint32_t*)(m_mmap + pos))[1];
	//		lyrEndRow[i] = ((uint32_t*)(m_mmap + pos))[2];
	//		lyrEndCol[i] = ((uint32_t*)(m_mmap + pos))[3];
	//		pos += COLROW_SIZE * 4;
	//	}

	//	for (uint8_t i = beglyr; i <= endlyr; ++i)
	//	{
	//		lyrBeginPos[i] = pos;
	//		pos += (lyrEndRow[i] - lyrBeginRow[i] + 1) * (lyrEndCol[i] - lyrBeginCol[i] + 1) * (DATA_SIZE_SIZE + DATA_POS_SIZE);
	//	}
	//	return  true;
	//}

	return false;

}
uint64_t silly_pyramid_index::get_layer_start_pos(const uint32_t& layer)
{
	return 0;
}
void silly_pyramid_index::set_layer_info(const uint32_t& layer, const layer_info& linfo)
{
	m_layer_infos[layer] = linfo;
	uint64_t pos = INDEX_DATA_BEGIN_POS;
	// 重新构建索引
	for (auto [l, info]: m_layer_infos)
	{
		m_layer_bpos[l] = pos;
		uint64_t rows = info.end_row - info.start_row + 1;
		uint64_t cols = info.end_col - info.start_col + 1;
		pos += (rows * cols * (DATA_POS_SIZE + DATA_SIZE_SIZE));

	}
}
bool silly_pyramid_index::write_block(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const block_index& idata)
{
	std::scoped_lock lock(m_mutex);
	seek(idata.offset);
	write((char*)(&idata.pos), sizeof(idata.pos), 0);
	seek(sizeof(idata.pos), SEEK_CUR);
	write((char*)(&idata.size), sizeof(idata.size), 0);

	return true;
}

void silly_pyramid_index::write_layer_info()
{
	std::scoped_lock lock(m_mutex);
	seek(PYRAMID_DESC_LENGTH + PYRAMID_MVER_LENGTH + PYRAMID_PVER_LENGTH);
	for (auto [l, info] : m_layer_infos)
	{
		write((char*)(&info.start_row), sizeof(info.start_row), 0);
		write((char*)(&info.start_col), sizeof(info.start_col), 0);
		write((char*)(&info.end_row), sizeof(info.end_row), 0);
		write((char*)(&info.end_col), sizeof(info.end_col), 0);
		write((char*)(&m_layer_bpos[l]), sizeof(m_layer_bpos[l]), 0);
	}
}

bool silly_pyramid_index::close()
{
	write_layer_info();
	return silly_pyramid_base::close();
}
