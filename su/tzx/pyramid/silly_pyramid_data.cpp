//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_data.h"

silly_pyramid_data::silly_pyramid_data()
{
	m_desc[0] = 'I';
	m_desc[1] = 'D';
	m_desc[2] = 'A';
	m_desc[3] = 'T';
	
}


std::string silly_pyramid_data::read_block(uint32_t layer, uint64_t row, uint64_t col)
{
	std::string ret_data = "";
	//uint64_t datapos = 0;
	//uint32_t datasize = 0;
	//if (err_code::OK != m_index.read_block_pos(layer, row, col, datasize, datapos))
	//{
	//	return ret_data;
	//}
	//if (datasize == 0 || datapos == 0)
	//{
	//	return ret_data;
	//}

	//ret_data.resize(datasize);
	//memcpy(&ret_data[0], m_mmap + datapos, datasize);
	return ret_data;
}

char* silly_pyramid_data::read_block(uint32_t layer, uint64_t row, uint64_t col, uint32_t& datasize)
{
	char* ret_data = nullptr;
	/*uint64_t datapos = 0;
	if (err_code::OK != m_index.read_block_pos(layer, row, col, datasize, datapos))
	{
		return ret_data;
	}
	if (datasize == 0 || datapos == 0)
	{
		return ret_data;
	}

	ret_data = (char*)malloc(datasize);
	if (ret_data)
	{
		memcpy(ret_data, m_mmap + datapos, datasize);
	}*/
	
	return ret_data;
}

bool silly_pyramid_data::write_block(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const block_data& bdata)
{
	std::scoped_lock lock(m_mutex);
	seek(bdata.offset);
	if (bdata.size && bdata.data)
	{
		return write(bdata.data, bdata.size, 0);
	}

	return true;
	
}
