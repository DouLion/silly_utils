//
// Created by dly on 2023/7/10.
//

#include "silly_bz2.h"
#include <bzlib.h>

#define SILLY_BZ2_SUFFIX ".bz2"
// 1024*1024*512   每次500M
#define SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE (0x20000000)
// /* recommended size */
#define SILLY_BZ2_SUGGEST_COMPRESS_SIZE(in_len) ((unsigned int)(in_len * 1.1 + 600))

eCompressErr suBz2::compress(const suPath &s_src, const suPath &s_dst)
{
    if (!s_src.exists())
    {
        SU_ERROR_PRINT("Compress: Source file does not exist: %s", s_src.u8string().c_str());
        return eCompressErr::FileNotExistErr;
    }
    if (suPath::is_dir(s_src))
    {
        SU_ERROR_PRINT("Compress: bz2 does not support directory compression: %s", s_src.u8string().c_str());
        return eCompressErr::Bz2NotSupportDirErr;
    }

    // 读取待压缩数据
    std::ifstream fstream_src(s_src.u8string(), std::ios::binary | std::ios::ate);
    if (!fstream_src.is_open())
    {
        SU_ERROR_PRINT("Compress: Failed to open source file: %s", s_src.u8string().c_str());
        return eCompressErr::Bz2OpenFileErr;
    }

    size_t file_len = static_cast<size_t>(fstream_src.tellg());
    fstream_src.seekg(0, std::ios::beg);

    std::vector<char> file_content(file_len);  // 使用 std::vector 管理内存
    if (file_len > 0)
    {  // 只有文件不为空才尝试读取
        fstream_src.read(file_content.data(), file_len);
        if (fstream_src.fail() && !fstream_src.eof())  // 如果读取失败且不是文件末尾，说明有错误
        {
            SU_ERROR_PRINT("Compress: Reading from source file failed: %s", s_src.u8string().c_str());
            return eCompressErr::Bz2OpenFileErr;  // 或者更具体的错误码
        }
    }
    fstream_src.close();  // 提前关闭文件流

    // 构建输出文件
    std::ofstream fstream_dst(s_dst.u8string(), std::ios::binary);
    if (!fstream_dst.is_open())
    {
        SU_ERROR_PRINT("Compress: Failed to open destination file: %s", s_dst.u8string().c_str());
        return eCompressErr::Bz2OpenFileErr;
    }

    // 调用bz2进行压缩
    unsigned int dst_len = SILLY_BZ2_SUGGEST_COMPRESS_SIZE(file_len);
    std::vector<char> compressed_buffer(dst_len);  // 使用 std::vector 管理内存

    int rc = BZ2_bzBuffToBuffCompress(compressed_buffer.data(), &dst_len, file_content.data(), file_len, 1, 0, 30);
    if (rc != BZ_OK)
    {
        SU_ERROR_PRINT("Compress: BZ2_bzBuffToBuffCompress failed with error code: %d for file: %s", rc, s_src.u8string().c_str());
        // BZ_MEM_ERROR 可能是内存不足
        if (rc == BZ_MEM_ERROR)
            return eCompressErr::MemAllocErr;
        return eCompressErr::Bz2CompressErr;
    }

    fstream_dst.write(compressed_buffer.data(), dst_len);
    if (fstream_dst.fail())
    {
        SU_ERROR_PRINT("Compress: Writing to destination file failed: %s", s_dst.u8string().c_str());
        return eCompressErr::Bz2OpenFileErr;  // Or a more specific error
    }
    fstream_dst.close();

    return eCompressErr::Ok;
}

eCompressErr suBz2::decompress(const suPath &s_src, const suPath &s_dst)
{
    if (!s_src.exists())
    {
        SU_ERROR_PRINT("Decompress: Source file does not exist: %s", s_src.u8string().c_str());
        return eCompressErr::FileNotExistErr;
    }
    if (suPath::is_dir(s_src))
    {
        SU_ERROR_PRINT("Decompress: bz2 does not support directory decompression: %s", s_src.u8string().c_str());
        return eCompressErr::Bz2NotSupportDirErr;
    }

    // 读取待解压缩数据
    std::ifstream fstream_src(s_src.u8string(), std::ios::binary | std::ios::ate);
    if (!fstream_src.is_open())
    {
        SU_ERROR_PRINT("Decompress: Failed to open source file: %s", s_src.u8string().c_str());
        return eCompressErr::Bz2OpenFileErr;
    }

    size_t file_len = static_cast<size_t>(fstream_src.tellg());
    fstream_src.seekg(0, std::ios::beg);

    if (file_len == 0)  // 有效的bzip2文件即使压缩空数据也会有一个头信息，所以零长度的压缩文件是无效的
    {
        SU_ERROR_PRINT("Decompress: Input compressed file is empty: %s", s_src.u8string().c_str());
        return eCompressErr::InValidInputErr;
    }

    std::vector<char> file_content(file_len);
    fstream_src.read(file_content.data(), file_len);
    if (fstream_src.fail() && !fstream_src.eof())
    {
        SU_ERROR_PRINT("Decompress: Reading from source file failed: %s", s_src.u8string().c_str());
        return eCompressErr::Bz2OpenFileErr;
    }
    fstream_src.close();

    // 构建输出文件
    std::ofstream fstream_dst(s_dst.u8string(), std::ios::binary);
    if (!fstream_dst.is_open())
    {
        SU_ERROR_PRINT("Decompress: Failed to open destination file: %s", s_dst.u8string().c_str());
        return eCompressErr::Bz2OpenFileErr;
    }

    // 调用bz2进行解压
    // 初始猜测解压后大小，如果不足会在循环中扩大
    unsigned int dst_len = SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE;
    std::vector<char> dst_decompress(dst_len);
    int rc = BZ_OUTBUFF_FULL;  // 强制进入循环以处理初次解压

    while (rc == BZ_OUTBUFF_FULL)
    {
        // 如果当前缓冲区大小不足，则尝试扩大
        if (dst_decompress.size() < dst_len)
        {
            dst_decompress.resize(dst_len);
        }

        // 注意：BZ2_bzBuffToBuffDecompress 的 destLen 是 in/out 参数
        // 传入的是缓冲区总大小，传出的是实际解压数据大小
        unsigned int current_buffer_capacity = dst_decompress.size();
        unsigned int actual_decompressed_size = current_buffer_capacity;  // 每次尝试的最大解压量

        rc = BZ2_bzBuffToBuffDecompress(dst_decompress.data(), &actual_decompressed_size, file_content.data(), file_len, 0, 0);

        if (rc == BZ_OUTBUFF_FULL)
        {
            // 如果缓冲区仍然不足，bz2库会将actual_decompressed_size设置为需要的最小大小
            // 因此，我们至少需要扩展到这个大小，或者按照默认步长再次扩大
            dst_len = SU_MAX(current_buffer_capacity + SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE, (size_t)actual_decompressed_size);
            if (dst_len > 0x7FFFFFFF)
            {  // 防止无限扩张或过大
                SU_ERROR_PRINT("Decompress: Decompression buffer size too large, aborting: %s", s_src.u8string().c_str());
                return eCompressErr::MemAllocErr;
            }
            dst_decompress.resize(dst_len);  // 再次分配更大的内存
        }
        else if (rc != BZ_OK)
        {
            SU_ERROR_PRINT("Decompress: BZ2_bzBuffToBuffDecompress failed with error code: %d for file: %s", rc, s_src.u8string().c_str());
            if (rc == BZ_MEM_ERROR)
                return eCompressErr::MemAllocErr;
            return eCompressErr::Bz2DecompressErr;
        }
        else
        {
            // 解压成功，实际解压数据大小在actual_decompressed_size中
            dst_len = actual_decompressed_size;  // 更新为实际解压出来的数据大小
        }
    }

    fstream_dst.write(dst_decompress.data(), dst_len);
    if (fstream_dst.fail())
    {
        SU_ERROR_PRINT("Decompress: Writing to destination file failed: %s", s_dst.u8string().c_str());
        return eCompressErr::Bz2OpenFileErr;  // Or a more specific error
    }
    fstream_dst.close();

    return eCompressErr::Ok;
}

// ====================================================================================================
// 内存到内存压缩/解压缩
// ====================================================================================================

eCompressErr suBz2::compress(const char *inBin, const size_t &inLen, char **outBin, size_t &outLen)
{
    if (!inBin || inLen == 0)
    {
        SU_ERROR_PRINT("压缩(内存): Empty input data.");
        return eCompressErr::InValidInputErr;
    }
    if (*outBin)  // 期望 outBin 指向 nullptr，由本函数分配内存
    {
        SU_ERROR_PRINT("压缩(内存): Output buffer already allocated. Clean it first.");
        return eCompressErr::InValidOutputErr;
    }

    unsigned int dst_suggest_len = SILLY_BZ2_SUGGEST_COMPRESS_SIZE(inLen);

    // 第一次尝试分配，可能不够
    *outBin = (char *)std::malloc(dst_suggest_len);
    if (!*outBin)
    {
        SU_ERROR_PRINT("压缩(内存): Failed to allocate initial output buffer.");
        return eCompressErr::MemAllocErr;
    }

    unsigned int current_dst_len = dst_suggest_len;
    int rc = BZ2_bzBuffToBuffCompress(*outBin, &current_dst_len, (char *)inBin, inLen, 1, 0, 30);

    if (rc == BZ_OUTBUFF_FULL)
    {
        // bzip2库会告诉我们需要的最小大小
        // 如果 dst_suggest_len 此时已经是实际需要的大小，可能还不够，
        // 应该基于 bz2 告知的需要大小重新分配。
        // 但 BZ2_bzBuffToBuffCompress 在返回 BZ_OUTBUFF_FULL 时，会将 destLen 设置为所需大小。
        // 因此，current_dst_len 此时就是所需大小。
        unsigned int required_len = current_dst_len;
        SU_MEM_FREE(*outBin);                         // 释放旧的
        *outBin = (char *)std::malloc(required_len);  // 重新分配
        if (!*outBin)
        {
            SU_ERROR_PRINT("压缩(内存): Failed to reallocate output buffer.");
            return eCompressErr::MemAllocErr;
        }
        current_dst_len = required_len;                                                            // 更新为新的缓冲区大小
        rc = BZ2_bzBuffToBuffCompress(*outBin, &current_dst_len, (char *)inBin, inLen, 1, 0, 30);  // 再次尝试压缩
    }

    if (rc != BZ_OK)
    {
        SU_ERROR_PRINT("压缩(内存): BZ2_bzBuffToBuffCompress failed with error code: %d", rc);
        SU_MEM_FREE(*outBin);  // 释放已分配的内存
        if (rc == BZ_MEM_ERROR)
            return eCompressErr::MemAllocErr;
        return eCompressErr::Bz2CompressErr;
    }

    outLen = current_dst_len;  // 实际压缩后的数据长度
    return eCompressErr::Ok;
}

eCompressErr suBz2::decompress(const char *inBin, const size_t &inLen, char **outBin, size_t &outLen)
{
    if (!inBin || inLen == 0)
    {
        SU_ERROR_PRINT("解压(内存): Empty input data.");
        return eCompressErr::InValidInputErr;
    }
    if (*outBin)  // 期望 outBin 指向 nullptr，由本函数分配内存
    {
        SU_ERROR_PRINT("解压(内存): Output buffer already allocated. Clean it first.");
        return eCompressErr::InValidOutputErr;
    }

    outLen = 0;  // 初始化输出长度

    unsigned int current_dst_len = SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE;
    char *decompress_buffer = (char *)std::malloc(current_dst_len);
    if (!decompress_buffer)
    {
        SU_ERROR_PRINT("解压(内存): Failed to allocate initial decompress buffer.");
        return eCompressErr::MemAllocErr;
    }

    int rc = BZ_OUTBUFF_FULL;  // 强制进入循环以处理初次解压

    while (rc == BZ_OUTBUFF_FULL)
    {
        unsigned int actual_decompressed_size_attempt = current_dst_len;  // 每次尝试的最大解压量
        rc = BZ2_bzBuffToBuffDecompress(decompress_buffer, &actual_decompressed_size_attempt, (char *)inBin, inLen, 0, 0);

        if (rc == BZ_OUTBUFF_FULL)
        {
            // bz2库会将 actual_decompressed_size_attempt 设置为需要的最小大小
            unsigned int required_len = actual_decompressed_size_attempt;
            if (required_len <= current_dst_len)
            {
                // 这通常不应该发生，但作为安全措施
                required_len = current_dst_len + SILLY_BZ2_DECOMPRESS_DEFAULT_SIZE;
            }

            char *tmp = (char *)std::realloc(decompress_buffer, required_len);
            if (!tmp)
            {
                SU_ERROR_PRINT("解压(内存): Failed to reallocate decompress buffer.");
                SU_MEM_FREE(decompress_buffer);
                return eCompressErr::MemAllocErr;
            }
            decompress_buffer = tmp;
            current_dst_len = required_len;  // 更新为新的缓冲区大小
        }
        else if (rc != BZ_OK)
        {
            SU_ERROR_PRINT("解压(内存): BZ2_bzBuffToBuffDecompress failed with error code: %d", rc);
            SU_MEM_FREE(decompress_buffer);
            if (rc == BZ_MEM_ERROR)
                return eCompressErr::MemAllocErr;
            return eCompressErr::Bz2DecompressErr;
        }
        else
        {
            // 解压成功，实际解压数据大小在actual_decompressed_size_attempt中
            outLen = actual_decompressed_size_attempt;
        }
    }

    // 将解压后的数据拷贝到最终的输出缓冲区
    *outBin = (char *)std::malloc(outLen);
    if (!*outBin)
    {
        SU_ERROR_PRINT("解压(内存): Failed to allocate final output buffer.");
        SU_MEM_FREE(decompress_buffer);
        return eCompressErr::MemAllocErr;
    }
    std::memcpy(*outBin, decompress_buffer, outLen);
    SU_MEM_FREE(decompress_buffer);

    return eCompressErr::Ok;
}

eCompressErr suBz2::compress(const std::string &inBin, std::string &outBin)
{
    const char *in_data = inBin.data();
    size_t in_len = inBin.size();
    char *out_data = nullptr;  // 由下一层函数分配
    size_t out_len = 0;

    eCompressErr ret = compress(in_data, in_len, &out_data, out_len);
    if (ret == eCompressErr::Ok && out_len > 0)
    {
        outBin.assign(out_data, out_len);  // 使用 assign 避免不必要的拷贝
        SU_MEM_FREE(out_data);
    }
    else if (out_data)
    {  // 如果有分配但失败，也要释放
        SU_MEM_FREE(out_data);
    }
    return ret;
}

eCompressErr suBz2::decompress(const std::string &inBin, std::string &outBin)
{
    const char *in_data = inBin.data();
    size_t in_len = inBin.size();
    char *out_data = nullptr;  // 由下一层函数分配
    size_t out_len = 0;

    eCompressErr ret = decompress(in_data, in_len, &out_data, out_len);
    if (ret == eCompressErr::Ok && out_len > 0)
    {
        outBin.assign(out_data, out_len);  // 使用 assign 避免不必要的拷贝
        SU_MEM_FREE(out_data);
    }
    else if (out_data)
    {  // 如果有分配但失败，也要释放
        SU_MEM_FREE(out_data);
    }
    return ret;
}

bool suBz2::valid(const std::string &bin)
{
    // BZ2 文件头部至少为 "BZh" + 块大小字符 (1-9)，共 4 个字节
    if (bin.length() < 4)
    {
        return false;
    }

    // 检查魔数前三个字符 "BZh"
    if (bin[0] != 'B' || bin[1] != 'Z' || bin[2] != 'h')
    {
        return false;
    }

    // 检查第四个字符，它表示块大小，范围是 '1' 到 '9'
    // std::isdigit 函数也可以用于检查，但直接比较字符更简单明了
    if (bin[3] >= '1' && bin[3] <= '9')
    {
        return true;
    }

    return false;  // 不符合魔数特征
}
