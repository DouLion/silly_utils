//
// Created by dly on 2023/7/25.
//

#include <cstdlib>
#include <cstring>
#include "png_utils.h"
#include "su_marco.h"
using namespace silly_image;

png_data png_utils::create_empty(const size_t& rows, const size_t& cols, const png_uint_32& color_type, const png_uint_32& depth)
{
	png_data ret_data;
	switch (color_type)
	{
	case PNG_COLOR_TYPE_GRAY:
		ret_data.pixel_size = sizeof(png_byte) * 1;
		break;
	case PNG_COLOR_TYPE_RGB:
		ret_data.pixel_size = sizeof(png_byte) * 3;
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		ret_data.pixel_size = sizeof(png_byte) * 2;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		ret_data.pixel_size = sizeof(png_byte) * 4;
		break;
	default:
		break;
	}
	if (!ret_data.pixel_size)
	{
		return ret_data;
	}
	ret_data.color_type = color_type;
	ret_data.bit_depth = depth;
	if (rows && cols)
	{
		ret_data.height = rows;
		ret_data.width = cols;
		ret_data.data = (png_bytep*)malloc(rows * sizeof(png_bytep));
		for (int r = 0; r < rows && ret_data.data; ++r)
		{
			ret_data.data[r] = (png_byte*)malloc(cols * ret_data.pixel_size);
			memset(ret_data.data[r], 0, cols * ret_data.pixel_size);
		}
	}
	return ret_data;
}

png_data png_utils::read(const char* path)
{
	png_data ret_data;

	png_structp png_ptr;
	png_infop info_ptr;
	int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth_1, color_type_1, interlace_type_1;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (nullptr == png_ptr)
	{
		return ret_data;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (nullptr == info_ptr)
	{
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		return ret_data;
	}
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		return ret_data;
	}
	FILE* fp = nullptr;
	fp = fopen(path, "rb");
	if (nullptr == fp)
	{
		fclose(fp);
		return ret_data;
	}

	// 设置输入流
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 0);
	// 读取png文件头
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth_1, &color_type_1, &interlace_type_1, nullptr, nullptr);

	ret_data.width = png_get_image_width(png_ptr, info_ptr);
	ret_data.height = png_get_image_height(png_ptr, info_ptr);
	ret_data.color_type = png_get_color_type(png_ptr, info_ptr);
	ret_data.bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	// ret_data.png_channels = png_get_channels(png_ptr, info_ptr);

	// 读取PNG图像像素数据
	ret_data.data = (png_bytep*)malloc(height * sizeof(png_bytep));
	for (int r = 0; r < height; ++r) {
		ret_data.data[r] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
	}
	png_read_image(png_ptr, ret_data.data);
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

	fclose(fp);
	return ret_data;
}

bool png_utils::write(const char* path, const png_data& data)
{

	if (!data.height || !data.width || !data.data)
	{
		SU_DEBUG_PRINT("%s_%d: invalid png data.\n", __FILENAME__, __LINE__);
		return false;
	}

	FILE* output_fp;
	output_fp = fopen(path, "wb");

	png_structp png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (nullptr == png_write_ptr)
	{
		SU_DEBUG_PRINT("%s_%d: png_create_write_struct failed.\n", __FILENAME__, __LINE__);
		return false;
	}
	png_infop png_w_info = png_create_info_struct(png_write_ptr);
	if (nullptr == png_write_info)
	{
		SU_DEBUG_PRINT("%s_%d: png_create_info_struct failed.\n", __FILENAME__, __LINE__);
		return false;
	}
	if (setjmp(png_jmpbuf(png_write_ptr)))
	{
		png_destroy_read_struct(&png_write_ptr, &png_w_info, nullptr);
		fclose(output_fp);
		return false;

	}
	png_init_io(png_write_ptr, output_fp);
	png_set_IHDR(png_write_ptr, png_w_info, data.width, data.height, data.bit_depth, data.color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_write_ptr, png_w_info);
	png_write_image(png_write_ptr, data.data);
	png_write_end(png_write_ptr, nullptr);

	png_destroy_write_struct(&png_write_ptr, &png_w_info);
	fclose(output_fp);
	return true;

}

#include <vector>

static void PngWriteCallback(png_structp  png_ptr, png_bytep data, png_size_t length) {
	std::vector<unsigned char>* p = (std::vector<unsigned char>*)png_get_io_ptr(png_ptr);
	p->insert(p->end(), data, data + length);
}
bool silly_image::png_utils::encode_to_memory(const png_data& data, char** buf, size_t& len)
{
	png_structp p = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!p)
	{
		return false;
	}
	png_infop info_ptr = png_create_info_struct(p);
	if (!info_ptr)
	{
		return false;
	}
	if (setjmp(png_jmpbuf(p)))
	{
		return false;
	}
	png_set_IHDR(p, info_ptr, data.width, data.height, data.bit_depth,
		data.color_type,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	//png_set_compression_level(p, 1);
	png_set_rows(p, info_ptr, data.data);
	std::vector<unsigned char> out;
	png_set_write_fn(p, &out, PngWriteCallback, NULL);
	png_write_png(p, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	if (out.empty())
	{
		return false;
	}
	len = out.size();
	*buf = (char*)malloc(len);
	if (*buf)
	{
		memcpy(*buf, &out[0], len);
		return true;
	}
	return false;
}

std::string silly_image::png_utils::encode_to_memory(const png_data& data)
{
	std::string result;
	png_structp p = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!p)
	{
		return result;
	}
	png_infop info_ptr = png_create_info_struct(p);
	if (!info_ptr)
	{
		return result;
	}
	if (setjmp(png_jmpbuf(p)))
	{
		return result;
	}
	png_set_IHDR(p, info_ptr, data.width, data.height, data.bit_depth,
		data.color_type,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);
	//png_set_compression_level(p, 1);
	png_set_rows(p, info_ptr, data.data);
	std::vector<unsigned char> out;
	png_set_write_fn(p, &out, PngWriteCallback, NULL);
	png_write_png(p, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	if (out.empty())
	{
		return result;
	}
	size_t len = out.size();
	result.resize(len);
	memcpy(&result[0], &out[0], len);
	return result;
}

void png_data::release()
{
	for (int r = 0; r < height; ++r)
	{
		free(data[r]);
		data[r] = nullptr;
	}
	free(data);
	data = nullptr;
}

png_data png_data::operator=(const png_data& other)
{
	this->data = other.data;
	this->width = other.width;
	this->height = other.height;
	this->bit_depth = other.bit_depth;
	this->color_type = other.color_type;
	this->pixel_size = other.pixel_size;
	return *this;
}

void png_data::set_pixel(const size_t& r, const size_t& c, const png_pixel& sp)
{
	if (!(r < height && c < width))
	{
		SU_DEBUG_PRINT("%s_%d: invalid %d < %d and %d < %d.\n", __FILENAME__, __LINE__, r, height, c, width);
		return;
	}
	size_t col_pos = c * pixel_size;
	switch (color_type)
	{
	case PNG_COLOR_TYPE_GRAY:
		data[r][col_pos] = sp.gray;
		break;
	case PNG_COLOR_TYPE_RGB:
		data[r][col_pos] = sp.red;
		data[r][col_pos + 1] = sp.green;
		data[r][col_pos + 2] = sp.blue;
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		data[r][col_pos] = sp.gray;
		data[r][col_pos + 1] = sp.alpha;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		data[r][col_pos] = sp.red;
		data[r][col_pos + 1] = sp.green;
		data[r][col_pos + 2] = sp.blue;
		data[r][col_pos + 3] = sp.alpha;
		break;
	default:
		break;
	}
}


