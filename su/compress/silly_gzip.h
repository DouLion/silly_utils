/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 11:28
 * @version: 2.0.0 (修复版)
 * @software: silly_utils
 * @description: GZIP 压缩/解压缩工具
 *               - 文件操作：compressFile, decompressFile (使用 suPath)
 *               - 内存数据操作：compressBin, decompressBin (char* / std::string 重载)
 */
#ifndef SILLY_GZIP_H
#define SILLY_GZIP_H

#include <string>
#include <vector>
#include "../files/silly_path.h"  // 使用 suPath 处理路径

class silly_gzip
{
public:
    /// ==================== 文件操作 ====================

    /// <summary>
    /// 压缩文件为 GZIP 格式
    /// </summary>
    /// <param name="srcPath">源文件路径 (suPath)</param>
    /// <param name="dstPath">目标文件路径 (输出 .gz 文件)</param>
    /// <returns>成功返回 0，失败返回错误码</returns>
    int compressFile(const suPath& srcPath, const suPath& dstPath);

    /// <summary>
    /// 解压 GZIP 文件
    /// </summary>
    /// <param name="srcPath">源文件路径 (.gz 文件)</param>
    /// <param name="dstPath">目标文件路径 (解压后的原始文件)</param>
    /// <returns>成功返回 0，失败返回错误码</returns>
    int decompressFile(const suPath& srcPath, const suPath& dstPath);

    /// ==================== 内存数据块操作 (char*) ====================

    /// <summary>
    /// 压缩内存数据块 (char* 输入/输出)
    /// </summary>
    /// <param name="inData">输入数据指针</param>
    /// <param name="inLen">输入数据长度</param>
    /// <param name="outData">输出数据指针 (调用者需确保已分配足够内存，或通过 compressBin 自动分配)</param>
    /// <param name="outLen">输入/输出：输出缓冲区大小 / 实际压缩后长度</param>
    /// <returns>成功返回压缩后数据长度，失败返回负值</returns>
    long compressBin(const char* inData, unsigned int inLen, char* outData, unsigned int& outLen);

    /// <summary>
    /// 解压内存数据块 (char* 输入/输出)
    /// </summary>
    /// <param name="inData">输入数据指针 (GZIP 格式)</param>
    /// <param name="inLen">输入数据长度</param>
    /// <param name="outData">输出数据指针 (调用者需确保已分配足够内存)</param>
    /// <param name="outLen">输入/输出：输出缓冲区大小 / 实际解压后长度</param>
    /// <returns>成功返回解压后数据长度，失败返回负值</returns>
    long decompressBin(const char* inData, unsigned int inLen, char* outData, unsigned int& outLen);

    /// ==================== 内存数据块操作 (std::string) ====================

    /// <summary>
    /// 压缩内存数据块 (std::string 输入，std::string 输出 - 自动分配内存)
    /// </summary>
    /// <param name="inData">输入数据</param>
    /// <param name="outData">输出数据 (自动分配内存)</param>
    /// <returns>成功返回 true，失败返回 false</returns>
    bool compressBin(const std::string& inData, std::string& outData);

    /// <summary>
    /// 解压内存数据块 (std::string 输入，std::string 输出 - 自动分配内存)
    /// </summary>
    /// <param name="inData">输入数据 (GZIP 格式)</param>
    /// <param name="outData">输出数据 (自动分配内存)</param>
    /// <returns>成功返回 true，失败返回 false</returns>
    bool decompressBin(const std::string& inData, std::string& outData);

    /// <summary>
    /// 压缩内存数据块 (std::string 输入，std::vector<char> 输出 - 自动分配内存)
    /// </summary>
    /// <param name="inData">输入数据</param>
    /// <param name="outData">输出数据 (自动分配内存)</param>
    /// <returns>成功返回 true，失败返回 false</returns>
    bool compressBin(const std::string& inData, std::vector<char>& outData);

    /// <summary>
    /// 解压内存数据块 (std::vector<char> 输入，std::string 输出 - 自动分配内存)
    /// </summary>
    /// <param name="inData">输入数据 (GZIP 格式)</param>
    /// <param name="outData">输出数据 (自动分配内存)</param>
    /// <returns>成功返回 true，失败返回 false</returns>
    bool decompressBin(const std::vector<char>& inData, std::string& outData);

private:
    /// <summary>
    /// 内部压缩核心函数 (zlib 封装)
    /// </summary>
    /// <param name="inData">输入数据</param>
    /// <param name="inLen">输入长度</param>
    /// <param name="outData">输出数据指针</param>
    /// <param name="outLen">输出长度</param>
    /// <param name="isGzipFormat">是否为 GZIP 格式 (true) 还是 zlib 格式 (false)</param>
    /// <returns>成功返回压缩后长度，失败返回负值</returns>
    long compressData(const char* inData, unsigned int inLen, char* outData, unsigned int& outLen, bool isGzipFormat = true);

    /// <summary>
    /// 内部解压核心函数 (zlib 封装)
    /// </summary>
    /// <param name="inData">输入数据 (GZIP 格式)</param>
    /// <param name="inLen">输入长度</param>
    /// <param name="outData">输出数据指针</param>
    /// <param name="outLen">输出长度</param>
    /// <returns>成功返回解压后长度，失败返回负值</returns>
    long decompressData(const char* inData, unsigned int inLen, char* outData, unsigned int& outLen);
};

#endif  // SILLY_GZIP_H
