/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 11:28
 * @version: 2.0.0 (修复版)
 * @software: silly_utils
 * @description: GZIP 压缩/解压缩工具实现
 */

#include "silly_gzip.h"
#include <zlib.h>
#include <fstream>
#include <vector>
#include <cstdio>

// 默认压缩缓冲区大小 (1MB)
static const unsigned int DEFAULT_BUFFER_SIZE = 1024 * 1024;

// 估算压缩后大小 (压缩后可能略大于原始大小，预留 10% + 固定开销)
inline static unsigned int estimateCompressedSize(unsigned int inLen) {
    return inLen + (inLen / 10) + 1024;
}

// 估算解压后大小 (对于已知大小的情况，直接传入；否则使用估计值)
// 注意：GZIP 格式不直接在头部存储解压大小，需要动态扩容

int silly_gzip::compressFile(const suPath& srcPath, const suPath& dstPath) {
    // 检查源文件
    if (!suPath(srcPath).is_file()) {
        return -1;  // 源文件不存在
    }

    // 读取源文件内容
    std::ifstream ifs(srcPath, std::ios::binary);
    if (!ifs.is_open()) {
        return -2;  // 无法打开源文件
    }

    ifs.seekg(0, std::ios::end);
    std::streamsize fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if (fileSize <= 0) {
        return -3;  // 空文件或错误
    }

    std::vector<char> inputData(static_cast<size_t>(fileSize));
    if (!ifs.read(inputData.data(), fileSize)) {
        return -4;  // 读取失败
    }
    ifs.close();

    // 压缩
    std::string compressedData;
    if (!compressBin(std::string(inputData.begin(), inputData.end()), compressedData)) {
        return -5;  // 压缩失败
    }

    // 写入压缩文件
    std::ofstream ofs(dstPath, std::ios::binary);
    if (!ofs.is_open()) {
        return -6;  // 无法打开目标文件
    }
    ofs.write(compressedData.data(), compressedData.size());
    ofs.close();

    return 0;  // 成功
}

int silly_gzip::decompressFile(const suPath& srcPath, const suPath& dstPath) {
    // 检查源文件
    if (!suPath(srcPath).is_file()) {
        return -1;  // 源文件不存在
    }

    // 读取压缩文件内容
    std::ifstream ifs(srcPath, std::ios::binary);
    if (!ifs.is_open()) {
        return -2;  // 无法打开源文件
    }

    ifs.seekg(0, std::ios::end);
    std::streamsize compressedSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if (compressedSize <= 0) {
        return -3;  // 空文件或错误
    }

    std::vector<char> compressedData(static_cast<size_t>(compressedSize));
    if (!ifs.read(compressedData.data(), compressedSize)) {
        return -4;  // 读取失败
    }
    ifs.close();

    // 解压
    std::string decompressedData;
    if (!decompressBin(compressedData, decompressedData)) {
        return -5;  // 解压失败
    }

    // 写入解压文件
    std::ofstream ofs(dstPath, std::ios::binary);
    if (!ofs.is_open()) {
        return -6;  // 无法打开目标文件
    }
    ofs.write(decompressedData.data(), decompressedData.size());
    ofs.close();

    return 0;  // 成功
}

long silly_gzip::compressBin(const char* inData, unsigned int inLen, char* outData, unsigned int& outLen) {
    if (!inData || inLen == 0 || !outData || outLen == 0) {
        return -1;  // 无效参数
    }

    return compressData(inData, inLen, outData, outLen, true);  // GZIP 格式
}

long silly_gzip::decompressBin(const char* inData, unsigned int inLen, char* outData, unsigned int& outLen) {
    if (!inData || inLen == 0 || !outData || outLen == 0) {
        return -1;  // 无效参数
    }

    return decompressData(inData, inLen, outData, outLen);
}

bool silly_gzip::compressBin(const std::string& inData, std::string& outData) {
    if (inData.empty()) {
        outData.clear();
        return true;  // 空数据视为成功
    }

    // 预估压缩后大小
    unsigned int estimatedSize = estimateCompressedSize(static_cast<unsigned int>(inData.size()));

    // 分配输出缓冲区
    std::vector<char> buffer(estimatedSize);
    unsigned int actualSize = estimatedSize;

    long result = compressBin(inData.data(), static_cast<unsigned int>(inData.size()), buffer.data(), actualSize);

    if (result > 0) {
        outData.assign(buffer.data(), actualSize);
        return true;
    }

    // 如果预估大小不够，尝试扩容重试 (理论上 GZIP 压缩后不会比原始数据大太多)
    if (result == Z_BUF_ERROR) {
        buffer.resize(inData.size() * 2 + 1024);
        actualSize = static_cast<unsigned int>(buffer.size());
        result = compressBin(inData.data(), static_cast<unsigned int>(inData.size()), buffer.data(), actualSize);
        if (result > 0) {
            outData.assign(buffer.data(), actualSize);
            return true;
        }
    }

    outData.clear();
    return false;
}

bool silly_gzip::decompressBin(const std::string& inData, std::string& outData) {
    if (inData.empty()) {
        outData.clear();
        return true;  // 空数据视为成功
    }

    // 对于 GZIP，无法预知解压后大小，需要动态扩容
    static const size_t INITIAL_SIZE = 64 * 1024;  // 64KB 初始大小
    static const size_t MAX_SIZE = 1024 * 1024 * 1024;  // 1GB 上限

    std::vector<char> buffer(INITIAL_SIZE);
    unsigned int available = static_cast<unsigned int>(buffer.size());

    long result = decompressBin(inData.data(), static_cast<unsigned int>(inData.size()), buffer.data(), available);

    if (result > 0) {
        outData.assign(buffer.data(), result);
        return true;
    }

    // 如果缓冲区不够大，动态扩容重试
    if (result == Z_BUF_ERROR) {
        size_t currentSize = buffer.size();
        while (currentSize < MAX_SIZE) {
            currentSize *= 2;
            buffer.resize(currentSize);
            available = static_cast<unsigned int>(currentSize);

            result = decompressBin(inData.data(), static_cast<unsigned int>(inData.size()), buffer.data(), available);
            if (result > 0) {
                outData.assign(buffer.data(), result);
                return true;
            }

            if (result != Z_BUF_ERROR) {
                break;  // 其他错误，不再重试
            }
        }
    }

    outData.clear();
    return false;
}

bool silly_gzip::compressBin(const std::string& inData, std::vector<char>& outData) {
    if (inData.empty()) {
        outData.clear();
        return true;
    }

    unsigned int estimatedSize = estimateCompressedSize(static_cast<unsigned int>(inData.size()));
    outData.resize(estimatedSize);

    unsigned int actualSize = estimatedSize;
    long result = compressBin(inData.data(), static_cast<unsigned int>(inData.size()), outData.data(), actualSize);

    if (result > 0) {
        outData.resize(actualSize);
        return true;
    }

    // 扩容重试
    if (result == Z_BUF_ERROR) {
        outData.resize(inData.size() * 2 + 1024);
        actualSize = static_cast<unsigned int>(outData.size());
        result = compressBin(inData.data(), static_cast<unsigned int>(inData.size()), outData.data(), actualSize);
        if (result > 0) {
            outData.resize(actualSize);
            return true;
        }
    }

    outData.clear();
    return false;
}

bool silly_gzip::decompressBin(const std::vector<char>& inData, std::string& outData) {
    if (inData.empty()) {
        outData.clear();
        return true;
    }

    // 动态扩容逻辑同 string 版本
    static const size_t INITIAL_SIZE = 64 * 1024;
    static const size_t MAX_SIZE = 1024 * 1024 * 1024;

    std::vector<char> buffer(INITIAL_SIZE);
    unsigned int available = static_cast<unsigned int>(buffer.size());

    long result = decompressBin(inData.data(), static_cast<unsigned int>(inData.size()), buffer.data(), available);

    if (result > 0) {
        outData.assign(buffer.data(), result);
        return true;
    }

    if (result == Z_BUF_ERROR) {
        size_t currentSize = buffer.size();
        while (currentSize < MAX_SIZE) {
            currentSize *= 2;
            buffer.resize(currentSize);
            available = static_cast<unsigned int>(currentSize);

            result = decompressBin(inData.data(), static_cast<unsigned int>(inData.size()), buffer.data(), available);
            if (result > 0) {
                outData.assign(buffer.data(), result);
                return true;
            }

            if (result != Z_BUF_ERROR) {
                break;
            }
        }
    }

    outData.clear();
    return false;
}

// ==================== 内部实现 ====================

long silly_gzip::compressData(const char* inData, unsigned int inLen, char* outData, unsigned int& outLen, bool isGzipFormat) {
    if (!inData || inLen == 0 || !outData || outLen == 0) {
        return -1;
    }

    z_stream c_stream;
    c_stream.zalloc = Z_NULL;
    c_stream.zfree = Z_NULL;
    c_stream.opaque = Z_NULL;

    // 15 = 默认窗口大小, +16 = GZIP 格式
    int windowBits = isGzipFormat ? (15 | 16) : 15;

    if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        return -2;
    }

    c_stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(inData));  // 修复：直接使用指针，不要 *
    c_stream.avail_in = inLen;
    c_stream.next_out = reinterpret_cast<Bytef*>(outData);  // 修复：直接使用指针
    c_stream.avail_out = outLen;

    // 压缩循环
    while (c_stream.avail_in > 0 && c_stream.total_out < outLen) {
        if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) {
            deflateEnd(&c_stream);
            return -3;
        }
    }

    if (c_stream.avail_in > 0) {
        // 输出缓冲区太小，返回 Z_BUF_ERROR 指示需要扩容
        deflateEnd(&c_stream);
        outLen = static_cast<unsigned int>(c_stream.total_out);
        return Z_BUF_ERROR;
    }

    // 完成压缩
    int err;
    do {
        err = deflate(&c_stream, Z_FINISH);
        if (err == Z_STREAM_END) {
            break;
        }
        if (err != Z_OK) {
            deflateEnd(&c_stream);
            return -4;
        }
    } while (c_stream.avail_out > 0);

    if (deflateEnd(&c_stream) != Z_OK) {
        return -5;
    }

    outLen = static_cast<unsigned int>(c_stream.total_out);
    return outLen;
}

long silly_gzip::decompressData(const char* inData, unsigned int inLen, char* outData, unsigned int& outLen) {
    if (!inData || inLen == 0 || !outData || outLen == 0) {
        return -1;
    }

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    // MAX_WBITS + 16 = GZIP 格式
    int err = inflateInit2(&strm, MAX_WBITS | 16);
    if (err != Z_OK) {
        return -2;
    }

    strm.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(inData));  // 直接使用指针
    strm.avail_in = inLen;
    strm.next_out = reinterpret_cast<Bytef*>(outData);  // 直接使用指针
    strm.avail_out = outLen;

    // 解压循环
    err = inflate(&strm, Z_FINISH);
    if (err == Z_STREAM_END) {
        inflateEnd(&strm);
        outLen = static_cast<unsigned int>(strm.total_out);
        return outLen;
    }

    if (err == Z_BUF_ERROR) {
        // 输出缓冲区太小
        inflateEnd(&strm);
        outLen = static_cast<unsigned int>(strm.total_out);
        return Z_BUF_ERROR;
    }

    inflateEnd(&strm);
    return -3;  // 解压失败
}
