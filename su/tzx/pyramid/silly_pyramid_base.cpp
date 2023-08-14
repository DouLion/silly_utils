//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_base.h"


void silly_pyramid_base::mmap_close()
{
}


bool silly_pyramid_base::open(const char* file, const open_mode& mode)
{
	if (open_mode::READ == mode)
	{
		stream_open(file, "rb"); // 后面改成mmap形式
	}
	else if (open_mode::APP_WRITE == mode)
	{
		stream_open(file, "wb+");
	}
	else if (open_mode::TRUNC_WRITE == mode )
	{
		stream_open(file, "wb");
	}

	return false;
}

bool silly_pyramid_base::close()
{
	return false;
}

bool silly_pyramid_base::read(char* data, const size_t& size, const size_t& offset)
{
	if (m_normal)
	{
		return stream_read(data, size, offset);
	}
	else
	{
		return mmap_read(data, size, offset);
	}
	return false;
}

bool silly_pyramid_base::write(char* data, const size_t& size, const size_t& offset)
{
	if (m_normal)
	{
		return stream_write(data, size, offset);
	}
	return false;
}

void silly_pyramid_base::seek(const size_t& pos, const int& flag)
{
	if (m_stream)
	{
		fseek(m_stream, pos, SEEK_SET);
	}
}

bool silly_pyramid_base::stream_open(const char* file, const char* mode)
{
	if(mode)
	m_stream = fopen(file, mode);
	if (m_stream) return true;
	return false;
}

bool silly_pyramid_base::mmap_open(const char* file)
{
	return false;
}

bool silly_pyramid_base::stream_read(char* data, const size_t& size, const size_t& offset)
{
	return fread(data + offset, size, 1, m_stream);
	
}

bool silly_pyramid_base::mmap_read(char* data, const size_t& size, const size_t& offset)
{
	return false;
}

bool silly_pyramid_base::stream_write(char* data, const size_t& size, const size_t& offset)
{
	if (m_stream)	
	{
		fwrite(data + offset, size, 1, m_stream);
	}
	return false;
}

bool silly_pyramid_base::mmap_write(char* data, const size_t& size, const size_t& offset)
{
	return false;
}

void silly_pyramid_base::stream_close()
{
	if (m_stream)
	{
		write_info();
		fclose(m_stream);
	}
	
}


err_code silly_pyramid_base::read_info()
{
	
	seek();
	if (!read(m_desc, PYRAMID_DESC_LENGTH, PYRAMID_DESC_OFFSET))
	{
		return err_code::UNKNOWN;
	}

	if (!read((char*)(&m_major_ver), PYRAMID_MVER_LENGTH, PYRAMID_MVER_OFFSET))
	{
		return err_code::UNKNOWN;
	}
	if (!read((char*)(&m_primary_ver), PYRAMID_PVER_LENGTH, PYRAMID_PVER_OFFSET))
	{
		return err_code::UNKNOWN;
	}

	m_version = (m_major_ver << 2) | m_primary_ver;

	return err_code::OK;
}

void silly_pyramid_base::write_info()
{
	seek(0);
	write(m_desc, PYRAMID_DESC_LENGTH, 0);
	seek(PYRAMID_DESC_LENGTH, SEEK_SET);
	write((char*)(&m_major_ver), PYRAMID_MVER_LENGTH, 0);
	seek(PYRAMID_DESC_LENGTH + PYRAMID_MVER_LENGTH, SEEK_SET);
	write((char*)(&m_primary_ver), PYRAMID_PVER_LENGTH, 0);

}
