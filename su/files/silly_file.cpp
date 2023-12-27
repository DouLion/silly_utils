//
// Created by dly on 2023/8/3.
//

#include "silly_file.h"
#include <fstream>

bool silly_file::read(const std::string& path, std::string& content, const size_t& offset, const size_t& len)
{
	bool status = false;
	std::fstream input;
	input.open(path, std::ios::binary | std::ios::in);
	if (!input.is_open())
	{
		return status;
	}
	input.ignore(std::numeric_limits<std::streamsize>::max());
	size_t file_size = input.gcount();
	input.clear();   //  Since ignore will have set eof, 需要移动回文件头
	input.seekg(0, std::ios_base::beg);
	if (offset >= file_size) //保证读值不为空
	{
		input.close();
		return status;
	}
	input.seekg(0, std::ios::beg);
	size_t content_size = SU_MIN(len, file_size) - offset;
	content.resize(content_size);

	input.seekg(offset, std::ios::beg);
	input.read(&content[0], content_size);
	input.close();
	status = true;
    return status;
}
