/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 10:08
 * @version: 2.0.0 (修复版)
 * @software: silly_utils
 * @description: RAR 压缩/解压缩工具
 *               - 文件操作：compressFile, decompressFile (使用 suPath)
 *               - 内存数据操作：compressBin, decompressBin (char* / std::string 重载)
 *               - 基于 libarchive 实现 (需启用 SU_THIRD_SUPPORT_LIBARCHIVE)
 */
#ifndef SILLY_RAR_H
#define SILLY_RAR_H

#include <string>
#include <vector>
#include "../files/silly_path.h"  // 使用 suPath 处理路径
#include "silly_compress.h"       // 使用 eCompressErr 错误码

class suRAR
{
public:
    /// ==================== 文件操作 ====================

    /// <summary>
    /// 压缩文件或目录为 RAR 文件
    /// </summary>
    /// <param name="srcPath">源文件或目录路径 (suPath)</param>
    /// <param name="dstPath">目标 RAR 文件路径 (可选，默认与源文件同名 .rar)</param>
    /// <param name="append">是否追加模式 (true=追加到现有RAR, false=覆盖)</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr compressFile(const suPath& srcPath, const suPath& dstPath = suPath(""), bool append = false);

    /// <summary>
    /// 解压 RAR 文件
    /// </summary>
    /// <param name="srcPath">源 RAR 文件路径</param>
    /// <param name="dstPath">解压目标路径 (文件则解压到同目录，目录则解压到该目录)</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr decompressFile(const suPath& srcPath, const suPath& dstPath = suPath(""));

    /// ==================== 内存数据块操作 (char*) ====================
    /// 注意：RAR 格式不适合小数据块压缩，此处仅提供接口，实际可能不实用

    /// <summary>
    /// 压缩内存数据块 (char* 输入/输出)
    /// </summary>
    /// <param name="inData">输入数据指针</param>
    /// <param name="inLen">输入数据长度</param>
    /// <param name="outData">输出数据指针 (调用者需提供缓冲区，函数会自动分配)</param>
    /// <param name="outLen">输入：输出缓冲区大小；输出：实际压缩后长度</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr compressBin(const char* inData, size_t inLen, char** outData, size_t& outLen);

    /// <summary>
    /// 解压内存数据块 (char* 输入/输出)
    /// </summary>
    /// <param name="inData">输入数据指针 (RAR 格式)</param>
    /// <param name="inLen">输入数据长度</param>
    /// <param name="outData">输出数据指针 (调用者需提供足够大的缓冲区)</param>
    /// <param name="outLen">输入：输出缓冲区大小；输出：实际解压后长度</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr decompressBin(const char* inData, size_t inLen, char** outData, size_t& outLen);

    /// ==================== 内存数据块操作 (std::string) ====================

    /// <summary>
    /// 压缩内存数据块 (std::string 输入，std::string 输出 - 自动分配内存)
    /// </summary>
    /// <param name="inData">输入数据</param>
    /// <param name="outData">输出数据 (自动分配内存)</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr compressBin(const std::string& inData, std::string& outData);

    /// <summary>
    /// 解压内存数据块 (std::string 输入，std::string 输出 - 自动分配内存)
    /// </summary>
    /// <param name="inData">输入数据 (RAR 格式)</param>
    /// <param name="outData">输出数据 (自动分配内存)</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr decompressBin(const std::string& inData, std::string& outData);

    /// ==================== 内存数据块操作 (std::vector) ====================

    /// <summary>
    /// 压缩内存数据块 (std::string 输入，std::vector<char> 输出 - 自动分配内存)
    /// </summary>
    static eCompressErr compressBin(const std::string& inData, std::vector<char>& outData);

    /// <summary>
    /// 解压内存数据块 (std::vector<char> 输入，std::string 输出 - 自动分配内存)
    /// </summary>
    static eCompressErr decompressBin(const std::vector<char>& inData, std::string& outData);

private:
    /// <summary>
    /// 内部压缩核心函数 (libarchive 封装)
    /// </summary>
    static eCompressErr compressData(const char* inData, size_t inLen, char** outData, size_t& outLen);

    /// <summary>
    /// 内部解压核心函数 (libarchive 封装)
    /// </summary>
    static eCompressErr decompressData(const char* inData, size_t inLen, char** outData, size_t& outLen);
};

#endif  // SILLY_RAR_H
