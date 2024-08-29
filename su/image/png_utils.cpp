//
// Created by dly on 2023/7/25.
//

#include "png_utils.h"

using namespace silly_image;

static void silly_png_write_callback(png_structp png_ptr, png_bytep data, png_size_t length)
{
    std::string *p = (std::string *)png_get_io_ptr(png_ptr);
    p->insert(p->end(), data, data + length);
}

static void silly_png_read_callback(png_structp png_ptr, png_bytep data, png_size_t length)
{
    std::string *pngData = (std::string *)png_get_io_ptr(png_ptr);
    if (length > pngData->size())
    {
        length = pngData->size();
    }
    memcpy(data, pngData->data(), length);
    pngData->erase(0, length);
}

png_data png_utils::create_empty(const size_t &rows, const size_t &cols, const png_uint_32 &color_type, const png_uint_32 &depth)
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
        ret_data.data = (png_bytep *)malloc(rows * sizeof(png_bytep));
        for (int r = 0; r < rows && ret_data.data; ++r)
        {
            ret_data.data[r] = (png_byte *)malloc(cols * ret_data.pixel_size);
            memset(ret_data.data[r], 0, cols * ret_data.pixel_size);
        }
    }
    return ret_data;
}

png_data png_utils::read(const std::string &path)
{
    png_data ret_data;

    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
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
    FILE *fp = nullptr;
    fp = fopen(path.c_str(), "rb");
    if (!fp)
    {
        // fclose(fp);
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
    ret_data.data = (png_bytep *)malloc(height * sizeof(png_bytep));
    for (int r = 0; r < height; ++r)
    {
        ret_data.data[r] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }
    png_read_image(png_ptr, ret_data.data);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    fclose(fp);
    return ret_data;
}

bool png_utils::write(const std::string &path, const png_data &data)
{
    if (!data.height || !data.width || !data.data)
    {
        SLOG_DEBUG("invalid png data.");
        return false;
    }

    FILE *output_fp;
    output_fp = fopen(path.c_str(), "wb");

    png_structp png_write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (nullptr == png_write_ptr)
    {
        SLOG_DEBUG("png_create_write_struct failed.");
        return false;
    }
    png_infop png_w_info = png_create_info_struct(png_write_ptr);
    if (nullptr == png_write_info)
    {
        SLOG_DEBUG("png_create_info_struct failed.");
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

bool png_utils::memory_encode(const png_data &data, std::string &buff)
{
    bool status = false;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        return status;
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        return status;
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return status;
    }
    // std::vector<unsigned char> buff_vec;
    png_set_IHDR(png_ptr, info_ptr, data.width, data.height, data.bit_depth, data.color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_rows(png_ptr, info_ptr, data.data);
    png_set_write_fn(png_ptr, &buff, silly_png_write_callback, NULL);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    /* buff.resize(buff_vec.size());
     std::copy(buff_vec.begin(), buff_vec.end(), buff.begin());*/
    status = buff.size();
    return status;
}

bool png_utils::memory_decode(const std::string &buff, png_data &data)
{
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        std::cerr << "Failed to initialize PNG read struct." << std::endl;
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        std::cerr << "Failed to initialize PNG info struct." << std::endl;
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        std::cerr << "Error during PNG parsing." << std::endl;
        return false;
    }

    // 设置输入数据作为PNG读取源
    png_set_read_fn(png_ptr, const_cast<std::string *>(&buff), silly_png_read_callback);

    // 读取PNG头部信息
    png_read_info(png_ptr, info_ptr);

    // 获取PNG图像的基本信息
    png_uint_32 width, height;
    int bit_depth, color_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
    png_read_update_info(png_ptr, info_ptr);
    data = png_utils::create_empty(height, width, color_type, bit_depth);
    // 读取PNG图像的所有数据
    // png_read_end(png_ptr, info_ptr);
    // 读取PNG图像像素数据
    /*data.data = (png_bytep *)malloc(height * sizeof(png_bytep));
    for (int r = 0; r < height; ++r)
    {
        data.data[r] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }*/
    png_read_image(png_ptr, data.data);

    // 清理资源
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

    return true;
}
bool png_utils::rgb_to_rgba(const png_data &src, png_data &dst, const png_byte &alpha)
{
    if (src.color_type != PNG_COLOR_TYPE_RGB)
    {
        return false;
    }
    dst = create_empty(src.height, src.width, PNG_COLOR_TYPE_RGB_ALPHA, src.bit_depth);
    for (size_t r = 0; r < src.height; ++r)
    {
        png_bytep srcp = src.data[r];
        png_bytep dstp = dst.data[r];
        for (size_t c = 0; c < src.width; ++c)
        {
            memcpy(dstp, srcp, 3);
            dstp[3] = alpha;
            srcp += 3;
            dstp += 4;
        }
    }

    return true;
}
bool png_utils::rgba_to_rgb(const png_data &src, png_data &dst)
{
    if (src.color_type != PNG_COLOR_TYPE_RGB_ALPHA)
    {
        return false;
    }
    dst = create_empty(src.height, src.width, PNG_COLOR_TYPE_RGB, src.bit_depth);
    for (size_t r = 0; r < src.height; ++r)
    {
        png_bytep srcp = src.data[r];
        png_bytep dstp = dst.data[r];
        for (size_t c = 0; c < src.width; ++c)
        {
            memcpy(dstp, srcp, 3);
            srcp += 4;
            dstp += 3;
        }
    }
    return true;
}

void png_data::release()
{
    for (int r = 0; r < height; ++r)
    {
        SU_MEM_FREE(data[r]);
        data[r] = nullptr;
    }
    SU_MEM_FREE(data);
    data = nullptr;
}

png_data png_data::operator=(const png_data &other)
{
    this->data = other.data;
    this->width = other.width;
    this->height = other.height;
    this->bit_depth = other.bit_depth;
    this->color_type = other.color_type;
    this->pixel_size = other.pixel_size;
    return *this;
}

void png_data::set_pixel(const size_t &r, const size_t &c, const silly_color &sp)
{
    if (!(r < height && c < width))
    {
        // SLOG_DEBUG("invalid %zu < %u and %zu < %u.", r, height, c, width);
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