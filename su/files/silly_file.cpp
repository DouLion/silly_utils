//
// Created by dly on 2023/8/3.
//

#include "silly_file.h"
#include <fstream>

size_t silly_file::read(const std::string &path, std::string &content, const size_t &offset, const size_t &len)
{
	size_t ret_read_size = 0;
	content.clear();
	std::fstream input;
	input.open(path, std::ios::binary | std::ios::in);
	if (!input.is_open())
	{
		return ret_read_size;
	}
	input.ignore(std::numeric_limits<std::streamsize>::max());
	size_t file_size = input.gcount();
	input.clear(); //  Since ignore will have set eof, 需要移动回文件头
	input.seekg(0, std::ios_base::beg);
	if (offset >= file_size) // 保证读值不为空
	{
		input.close();
		return ret_read_size;
	}
	input.seekg(0, std::ios::beg);
	ret_read_size = SU_MIN(len, file_size) - offset;
	content.resize(ret_read_size);

	input.seekg(offset, std::ios::beg);
	input.read(&content[0], ret_read_size);
	input.close();
	return ret_read_size;
}

size_t silly_file::read(const std::string &path, unsigned char **content, const size_t &offset, const size_t &len)
{
	size_t read_size = 0;
	if ((*content)) // content 不能有内容
	{
		return read_size;
	} 
	std::string s_cont;
	read_size = silly_file::read(path, s_cont, offset, len);
	if (read_size)
	{
		*content = (unsigned char *)malloc(read_size);
		if ((*content))
		{
			memcpy((*content), &s_cont[0], read_size);
		}
		else
		{
			read_size = 0;
		}
	}
	return read_size;
}
