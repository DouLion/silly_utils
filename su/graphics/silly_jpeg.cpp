/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-02
 * @file: silly_jpeg.cpp
 * @description: silly_jpeg实现
 * @version: v1.0.1 2025-01-02 dou li yang
 */
#include "silly_jpeg.h"
#include <files/silly_file.h>

#if SU_THIRD_SUPPORT_JPEG
#include "jpeglib.h"
#include "jerror.h"
#include <setjmp.h>

struct my_error_mgr
{
    struct jpeg_error_mgr pub; /* "public" fields */
    jmp_buf setjmp_buffer;     /* for return to caller */
};
using my_error_ptr = my_error_mgr*;

void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;

    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

static eColorType jpeg2sillyctype(const J_COLOR_SPACE& type)
{
    switch (type)
    {
        case J_COLOR_SPACE::JCS_GRAYSCALE:
            return eColorType::GRAY;
        case J_COLOR_SPACE::JCS_RGB:
            return eColorType::RGB;
        case J_COLOR_SPACE::JCS_EXT_RGBA:
            return eColorType::RGBA;
        default:
            throw std::runtime_error("不支持的类型");
    }
}

static J_COLOR_SPACE silly2jpegctype(const eColorType& type)
{
    switch (type)
    {
        case eColorType::GRAY:
            return J_COLOR_SPACE::JCS_GRAYSCALE;
        case eColorType::RGB:
            return J_COLOR_SPACE::JCS_RGB;
        case eColorType::RGBA:
            return J_COLOR_SPACE::JCS_EXT_RGBA;
        default:
            throw std::runtime_error("不支持的类型");
    }
    return J_COLOR_SPACE::JCS_UNKNOWN;
}
#endif
void sujpeg::pixel(const size_t& row, const size_t& col, const silly::color& pixel)
{
#if SU_THIRD_SUPPORT_JPEG
    if (row >= m_height || col >= m_width)
    {
        return;
    }

    int start = (col + row * m_width) * m_channels;
    if (3 == m_channels)
    {
        m_bytes[start] = pixel.red;
        m_bytes[start + 1] = pixel.green;
        m_bytes[start + 2] = pixel.blue;
    }
    if (1 == m_channels)
    {
        m_bytes[start] = pixel.gray;
    }

#endif
    return;
}

silly::color sujpeg::pixel(const size_t& row, const size_t& col) const
{
    silly::color pixel = {0, 0, 0};
    if (row >= m_height || col >= m_width)
    {
        std::cout << "Coordinates out of bounds " << std::endl;
        return pixel;
    }
    int start = (col + row * m_width) * m_channels;

    if (m_channels == 3)
    {
        pixel.red = m_bytes[start];
        pixel.green = m_bytes[start + 1];
        pixel.blue = m_bytes[start + 2];
    }
    else if (m_channels == 1)
    {
        pixel.gray = m_bytes[start];
    }

    return pixel;
}

sujpeg& sujpeg::operator=(const sujpeg& rh)
{
    this->m_width = rh.m_width;
    this->m_height = rh.m_height;
    this->m_channels = rh.m_channels;
    this->m_type = rh.m_type;
    this->m_quality = rh.m_quality;
    this->m_depth = rh.m_depth;
    this->m_bytes = rh.m_bytes;

    return *this;
}
bool sujpeg::create(const size_t& width, const size_t& height, const eColorType& type, const uint8_t& depth)
{
    m_type = type;
    switch (m_type)
    {
        case eColorType::GRAY:
            m_channels = 1;
            break;
        case eColorType::RGB:
            m_channels = 3;
            break;
        case eColorType::GRAYA:
            m_channels = 2;
            break;
        case eColorType::RGBA:
            m_channels = 4;
            break;
        default:
            return false;
    }
    m_width = width;
    m_height = height;
    if (!(m_width > 0 && m_height > 0))
    {
        return false;
    }
    m_bytes.resize(m_width * m_height * m_channels, 0);
    // memset(m_bytes.data(), 0, sizeof(uint8_t) * m_width * m_height * m_channels);
    return true;
}
bool sujpeg::read(const std::filesystem::path& file)
{
    bool status = false;
    std::string content;
    if (sufile::read(file, content) == 0)
    {
        return status;
    }

    return decode(content);
}
bool sujpeg::write(const std::filesystem::path& file) const
{
    std::string content = encode();
    if (content.empty())
    {
        return false;
    }

    return sufile::write(file, content) > 0;
}
bool sujpeg::decode(const std::string& bin)
{
    bool status = false;
#if SU_THIRD_SUPPORT_JPEG

    if (!valid(bin))
    {
        return status;
    }

    // 创建 JPEG 解压缩结构体
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // 将 JPEG 数据设置为输入
    jpeg_mem_src(&cinfo, (unsigned char*)bin.data(), bin.size());

    // 开始解压缩
    jpeg_read_header(&cinfo, TRUE);
    m_width = cinfo.image_width;
    m_height = cinfo.image_height;
    m_channels = cinfo.num_components;
    m_type = jpeg2sillyctype(cinfo.out_color_space);

    // 计算文件大小并分配内存
    m_bytes.resize(m_width * m_height * m_channels, 0);

    // 开始解压缩并读取数据
    jpeg_start_decompress(&cinfo);
    unsigned char* row_pointer = m_bytes.data();

    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
        row_pointer += cinfo.output_width * cinfo.num_components;
    }

    // 完成解压缩
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    status = true;

#endif
    return status;
}
std::string sujpeg::encode() const
{
    std::string ret;
#if SU_THIRD_SUPPORT_JPEG
    if (!(!m_bytes.empty() && m_width > 0 && m_height > 0))
    {
        return ret;
    }
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    // 分配内存缓冲区
    unsigned long mem_size = 0;
    unsigned char* buffer = nullptr;
    jpeg_mem_dest(&cinfo, &buffer, &mem_size);

    cinfo.image_width = m_width;
    cinfo.image_height = m_height;
    cinfo.input_components = m_channels;
    cinfo.in_color_space = silly2jpegctype(m_type);
    cinfo.data_precision = m_depth;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, m_quality, TRUE);
    // 使用4 : 4 : 4子采样（默认为4 : 2 : 0）
    cinfo.comp_info[0].h_samp_factor = cinfo.comp_info[0].v_samp_factor = 1;

    jpeg_start_compress(&cinfo, TRUE);

    unsigned int row_stride = cinfo.image_width * cinfo.input_components;

    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = (uint8_t*)&m_bytes[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    ret.assign(reinterpret_cast<char*>(buffer), mem_size);

#endif
    return ret;
}
