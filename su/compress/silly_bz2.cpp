//
// Created by dly on 2023/7/10.
//

#include "silly_bz2.h"
#include <bzlib.h>
#include "su_marco.h"

using namespace silly_compress;

#define SILLY_BZ2_SUFFIX ".bz2"
// 1024*1024*512   每次500M
#define SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE (0x20000000)
// /* recommended size */
#define SILLY_BZ2_SUGGEST_COMPRESS_SIZE(in_len) ((unsigned int)(in_len * 1.1 + 600))

// #define SILLY_BZ2_MALLOC(dst, size) dst =  (char*)malloc(size); if(!dst) { return MemAllocErr;}

CPS_ERR BZ2::compress(const std::string &s_src, const std::string &s_dst)
{
    if (!std::filesystem::exists(s_src))
    {
        return CPS_ERR::FileNotExistErr;
    }
    if (std::filesystem::is_directory(s_src))
    {
        return CPS_ERR::Bz2NotSupportDirErr;
    }

    // 读取待压缩数据
    std::ifstream fstream_src(s_src, std::ios::binary | std::ios::ate);
    if (!fstream_src.is_open())
    {
        SU_DEBUG_PRINT("Open file: %s", s_src.c_str());
        return CPS_ERR::Bz2OpenFileErr;
    }

    auto file_len = static_cast<size_t>(fstream_src.tellg());
    fstream_src.seekg(0, std::ios::beg);

    if (file_len == 0)
    {
        fstream_src.close();
        return CPS_ERR::Bz2OpenFileErr;  // 原始代码中没有分配0长度文件的处理，但压缩空文件是常见需求
    }

    auto file_content = (char *)malloc(file_len);
    if (!file_content)
    {
        fstream_src.close();
        throw std::bad_alloc{};
    }

    fstream_src.read(file_content, file_len);
    fstream_src.close();

    if (fstream_src.fail())
    {
        SU_MEM_FREE(file_content);
        throw std::runtime_error("Reading from source file failed.");
    }

    // 构建输出文件
    std::ofstream fstream_dst(s_dst, std::ios::binary);
    if (!fstream_dst.is_open())
    {
        SU_DEBUG_PRINT("Open file: %s", s_dst.c_str());
        return CPS_ERR::Bz2OpenFileErr;
    }

    // 调用bz2进行压缩
    unsigned int dst_len = SILLY_BZ2_SUGGEST_COMPRESS_SIZE(file_len);
    auto compressed = (char *)malloc(dst_len);
    if (!compressed)
    {
        fstream_dst.close();
        throw std::bad_alloc{};
    }

    int rc = BZ2_bzBuffToBuffCompress(compressed, &dst_len, file_content, file_len, 1, 0, 30);
    if (rc != BZ_OK)
    {
        SU_MEM_FREE(compressed);
        throw std::runtime_error("Compression failed.");
    }

    fstream_dst.write(compressed, dst_len);
    fstream_dst.close();

    SU_MEM_FREE(compressed);
    SU_MEM_FREE(file_content);
    return CPS_ERR::Ok;
}

CPS_ERR BZ2::decompress(const std::string &s_src, const std::string &s_dst)
{
    if (!std::filesystem::exists(s_src))
    {
        return CPS_ERR::FileNotExistErr;
    }
    if (std::filesystem::is_directory(s_src))
    {
        return CPS_ERR::Bz2NotSupportDirErr;
    }

    char *file_content = nullptr;
    size_t file_len;
    std::ifstream fstream_src(s_src, std::ios::binary | std::ios::ate);
    if (!fstream_src.is_open())
    {
        SU_DEBUG_PRINT("Open file: %s", s_src.c_str());

        return CPS_ERR::Bz2OpenFileErr;
    }
    // 构建输出文件
    std::ofstream fstream_dst(s_dst, std::ios::binary);
    if (!fstream_dst.is_open())
    {
        SU_DEBUG_PRINT("Open file: %s", s_dst.c_str());
        return CPS_ERR::Bz2OpenFileErr;
    }

    file_len = static_cast<size_t>(fstream_src.tellg());
    fstream_src.seekg(0, std::ios::beg);

    if (file_len)
    {
        file_content = (char *)malloc(file_len);
        if (!file_content)
        {
            fstream_src.close();
            return CPS_ERR::MemAllocErr;
        }

        fstream_src.read(file_content, file_len);
    }
    fstream_src.close();

    // 调用bz2进行解压
    unsigned int dst_len = SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE;
    char *dst_decompress = (char *)malloc(dst_len);  // 接收解压缩的数据。解压缩需要的内存大小事先是不知道的。
    int rc = BZ2_bzBuffToBuffDecompress(dst_decompress, &dst_len, file_content, file_len, 0, 0);
    while (BZ_OUTBUFF_FULL == rc)  // 预设解压空间较小,需要扩大,其他情况再看如何解决
    {
        dst_len += SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE;
        if (dst_decompress)
        {
            SU_MEM_FREE(dst_decompress);
        }
        dst_decompress = (char *)malloc(dst_len);
        rc = BZ2_bzBuffToBuffDecompress(dst_decompress, &dst_len, file_content, file_len, 0, 0);
    }
    if (BZ_OK != rc)
    {
        SU_ERROR_PRINT("Decompress failed: %d", rc);
        SU_MEM_FREE(file_content);
        SU_MEM_FREE(dst_decompress);
        return CPS_ERR::Bz2DecompressErr;
    }
    fstream_dst.write(dst_decompress, dst_len);
    fstream_dst.close();

    SU_MEM_FREE(file_content);
    SU_MEM_FREE(dst_decompress);

    return CPS_ERR::Ok;
}

CPS_ERR BZ2::compress(const char *c_in_val, const size_t &i_in_len, char **c_out_val, size_t &i_out_len)
{
    if (!c_in_val || !i_in_len)
    {
        SU_ERROR_PRINT("Empty input data.");
        return CPS_ERR::InValidInputErr;
    }
    unsigned int dst_len = SILLY_BZ2_SUGGEST_COMPRESS_SIZE(i_in_len);
    char *c_buff_src = (char *)c_in_val;
    if (!*c_out_val)
    {
        *c_out_val = (char *)malloc(dst_len);
    }
    else
    {
        SU_ERROR_PRINT("Clean output and set null.");
        return CPS_ERR::InValidOutputErr;
    }
    int rc = BZ2_bzBuffToBuffCompress(*c_out_val, &dst_len, c_buff_src, i_in_len, 1, 0, 30);
    if (rc != BZ_OK)
    {
        SU_ERROR_PRINT("Compression failed : %d.", rc);
        return CPS_ERR::Bz2CompressErr;
    }
    i_out_len = dst_len;
    return CPS_ERR::Ok;
}

CPS_ERR BZ2::decompress(const char *c_in_val, const size_t &i_in_len, char **c_out_val, size_t &i_out_len)
{
    if (!c_in_val || !i_in_len)
    {
        SU_ERROR_PRINT("Empty input data.");
        return CPS_ERR::InValidInputErr;
    }

    i_out_len = 0;
    char *c_buff_src = (char *)c_in_val;
    if (*c_out_val)
    {
        SU_ERROR_PRINT("Clean output and set null.");
        return CPS_ERR::InValidOutputErr;
    }
    unsigned int dst_len = SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE;
    char *decompress = (char *)malloc(dst_len);
    if (!decompress)
    {
        return CPS_ERR::MemAllocErr;
    };
    int rc = BZ2_bzBuffToBuffDecompress(decompress, &dst_len, c_buff_src, i_in_len, 0, 0);
    while (BZ_OUTBUFF_FULL == rc)  // 预设解压空间较小,需要扩大,其他情况再看如何解决
    {
        dst_len += SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE;
        char *tmp = (char *)realloc(decompress, dst_len);
        if (!tmp)
        {
            SU_MEM_FREE(decompress);
            return CPS_ERR::MemAllocErr;
        }
        decompress = tmp;
        rc = BZ2_bzBuffToBuffDecompress(decompress, &dst_len, c_buff_src, i_in_len, 0, 0);
    }

    if (BZ_OK != rc)
    {
        SU_ERROR_PRINT("Decompress failed : % d.", rc);
        if (*c_out_val)
        {
            SU_MEM_FREE(*c_out_val);
            *c_out_val = nullptr;
        }
        return CPS_ERR::Bz2DecompressErr;
    }
    *c_out_val = (char *)malloc(dst_len);
    memcpy(*c_out_val, decompress, dst_len);
    SU_MEM_FREE(decompress);
    i_out_len = dst_len;
    return CPS_ERR::Ok;
}
