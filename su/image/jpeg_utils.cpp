//
// Created by dell on 2023/7/26.
//

#include "jpeg_utils.h"
#include <cstring>

struct my_error_mgr
{
    struct jpeg_error_mgr pub;    /* "public" fields */
    jmp_buf setjmp_buffer;        /* for return to caller */
};
typedef struct my_error_mgr* my_error_ptr;
void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr)cinfo->err;

    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}


bool jpeg_data::release()   
{
    if (nullptr == image_data)
    {
        return false;
    }
    else
    {
        free(image_data);
        image_data = nullptr;
    }
    return true;
}


jpeg_data jpeg_utils::creat_empty_jpeg(const size_t& width, const size_t& height, const size_t& components, const J_COLOR_SPACE& color_space, const size_t& quality, const size_t& data_precision)
{
    jpeg_data res_jpeg;

    int row_stride;

    res_jpeg.jpeg_width = width;
    res_jpeg.jpeg_height = height;
    res_jpeg.jpeg_components = components;
    res_jpeg.color_space = color_space;
    res_jpeg.quality = quality;
    res_jpeg.data_precision = data_precision;

    row_stride = width * components;
    //image_buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, height);

    res_jpeg.image_data = (unsigned char*)malloc(sizeof(unsigned char*) * width * height * components);
    memset(res_jpeg.image_data, 255, sizeof(unsigned char*) * width * height * components);

    return res_jpeg;
}


bool jpeg_utils::write_jpeg_data(const char* path, const jpeg_data& jpeg_data)
{
    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;

    JSAMPARRAY image_buffer = NULL;
    JSAMPROW row_p[1];;

    int row, col;

    cinfo.err = jpeg_std_error(&jerr);
    /*初始化JPEG压缩对象*/
    jpeg_create_compress(&cinfo);

    FILE* outfile;
    if ((outfile = fopen(path, "wb")) == NULL)
    {
        ERREXIT(&cinfo, JERR_FILE_WRITE);
        return false;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = jpeg_data.jpeg_width;            // 宽度
    cinfo.image_height = jpeg_data.jpeg_height;          // 高度
    cinfo.input_components = jpeg_data.jpeg_components;    /* # 每个像素的颜色分量 3或1 */
    cinfo.in_color_space = jpeg_data.color_space;       /* 输入图像的色彩空间必须为J_COLOR_SPACE枚举常量之一，通常为JCS_RGB或JCS_GRAYSCALE*/
    cinfo.data_precision = jpeg_data.data_precision; /* 输入图像的数据精度 一般为8*/

    jpeg_set_defaults(&cinfo);

    jpeg_set_quality(&cinfo, jpeg_data.quality, TRUE);
    //使用4 : 4 : 4子采样（默认为4 : 2 : 0）
    cinfo.comp_info[0].h_samp_factor = cinfo.comp_info[0].v_samp_factor = 1;
    jpeg_start_compress(&cinfo, TRUE);

    unsigned int row_stride = cinfo.image_width * cinfo.input_components;

    for (int row = 0; cinfo.next_scanline < cinfo.image_height; row++)
    {
        row_p[0] = &jpeg_data.image_data[row * row_stride];
        jpeg_write_scanlines(&cinfo, row_p, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);

    /* Step 8: release JPEG compression object */
    jpeg_destroy_compress(&cinfo);

    return true;

}


jpeg_data jpeg_utils::read_jpeg(const char* path)
{
    jpeg_data res_jpeg;

    jpeg_decompress_struct cinfo;
    my_error_mgr jerr;

    FILE* infile;                 /* source file */
    JSAMPARRAY buffer = NULL;                 /* Output row buffer */
    int col;
    int row_stride;               /* physical row width in output buffer */

    if ((infile = fopen(path, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", path);
        return res_jpeg;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return res_jpeg;
    }

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;

    res_jpeg.jpeg_width = cinfo.output_width;
    res_jpeg.jpeg_height = cinfo.output_height;
    res_jpeg.jpeg_components = cinfo.output_components;
    res_jpeg.color_space = cinfo.out_color_space;
    res_jpeg.data_precision = cinfo.data_precision;

    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);


    // 创建存储图像数据的数组
    res_jpeg.image_data = (unsigned char*)malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);

    int row = 0;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        // 将当前扫描行的数据存储到数组中
        memcpy(res_jpeg.image_data + row * row_stride, buffer[0], row_stride);
        row++;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return res_jpeg;
}


bool jpeg_data::set_pixel(const size_t& row, const size_t& col, const jpeg_pixel& pixel)
{

    if (row > jpeg_width || col > jpeg_height)
    {
        std::cout << "Coordinates out of bounds " << std::endl;
        return false;
    }
    int row_stride = jpeg_width * jpeg_components;
    int start = (col * row_stride) + (row * jpeg_components);
    if (3 == jpeg_components)
    {
        image_data[start] = pixel.red;
        image_data[start + 1] = pixel.green;
        image_data[start + 2] = pixel.blue;

    }
    else if (1 == jpeg_components)
    {
        image_data[start] = pixel.gray;
    }

    return true;
}



