//
// Created by dell on 2023/8/14.
//

#include "silly_pyramid_base.h"


void silly_pyramid_base::mmap_close()
{
}


bool silly_pyramid_base::open(const char* file, const open_mode& mode)
{
	m_mode = mode;
	if (open_mode::READ == mode)
	{
		stream_open(file, "rb"); // 后面改成mmap形式
		read_info();
	}
	else if (open_mode::APP_WRITE == mode)
	{
		stream_open(file, "wb+");
	}
	else if (open_mode::TRUNC_WRITE == mode )
	{
		stream_open(file, "wb");
	}

	return m_opened;
}

bool silly_pyramid_base::close()
{
	m_opened = false;
	return false;
}

bool silly_pyramid_base::read(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
	if (m_normal)
	{
		return stream_read(seek_offset, data, size, offset);
	}
	else
	{
		return mmap_read(seek_offset, data, size, offset);
	}
	return false;
}

bool silly_pyramid_base::write(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
	if (m_normal)
	{
		return stream_write(seek_offset, data, size, offset);
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
	m_stream = fopen(file, mode);
	if (m_stream)
	{
		m_opened = true;
	}
	return m_opened;
}

bool silly_pyramid_base::mmap_open(const char* file)
{
	return m_opened;
}

bool silly_pyramid_base::stream_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
	std::scoped_lock lock(m_mutex);
	if (m_stream && m_opened)
	{
		seek(seek_offset);
		return fread(data + offset, size, 1, m_stream);
	}
	return false;
}

bool silly_pyramid_base::mmap_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
	return false;
}

bool silly_pyramid_base::stream_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
	if (m_stream && m_opened)	
	{
		seek(seek_offset);
		fwrite(data + offset, size, 1, m_stream);
	}
	return false;
}

bool silly_pyramid_base::mmap_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset)
{
	return false;
}

void silly_pyramid_base::stream_close()
{
	if (m_mode != READ)	// 写打开时需要将信息保存回去
	{
		write_info(); 
	}
	if (m_stream && m_opened)
	{
		fclose(m_stream);
	}
}


err_code silly_pyramid_base::read_info()
{
	
	if (!read(PYRAMID_DESC_OFFSET, m_desc, PYRAMID_DESC_LENGTH))
	{
		return err_code::UNKNOWN;
	}

	if (!read(PYRAMID_MVER_OFFSET, (char*)(&m_major_ver), PYRAMID_MVER_LENGTH))
	{
		return err_code::UNKNOWN;
	}
	if (!read(PYRAMID_PVER_OFFSET, (char*)(&m_primary_ver), PYRAMID_PVER_LENGTH))
	{
		return err_code::UNKNOWN;
	}

	return err_code::OK;
}

void silly_pyramid_base::write_info()
{
	write(0, m_desc, PYRAMID_DESC_LENGTH, 0);
	write(PYRAMID_MVER_OFFSET, (char*)(&m_major_ver), PYRAMID_MVER_LENGTH, 0);
	write(PYRAMID_PVER_OFFSET, (char*)(&m_primary_ver), PYRAMID_PVER_LENGTH, 0);

}
