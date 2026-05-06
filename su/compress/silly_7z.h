/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10
 * @version: 2.0.0 (补充实现版)
 * @software: silly_utils
 * @description: 7Z 压缩/解压缩工具
 *               - 文件操作：compressFile, decompressFile (使用 suPath)
 *               - 内存数据操作：compressBin, decompressBin (char* / std::string 重载)
 *               - 基于 libarchive 实现 (需启用 SU_THIRD_SUPPORT_LIBARCHIVE)
 *               - 注意：7Z 格式压缩/解压对大文件支持良好
 */
#ifndef SILLY_7Z_H
#define SILLY_7Z_H

#include <string>
#include <vector>
#include "../files/silly_path.h"
#include "silly_compress.h"

class silly_7z
{
public:
    /// ==================== 文件操作 ====================

    /// <summary>
    /// 压缩文件或目录为 7Z 文件
    /// </summary>
    /// <param name="srcPath">源文件或目录路径</param>
    /// <param name="dstPath">目标 7Z 文件路径 (可选，默认与源文件同名 .7z)</param>
    /// <param name="append">是否追加模式 (7Z 通常不支持简单追加，建议使用 false)</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr compressFile(const suPath& srcPath, const suPath& dstPath = suPath(""), bool append = false);

    /// <summary>
    /// 解压 7Z 文件
    /// </summary>
    /// <param name="srcPath">源 7Z 文件路径</param>
    /// <param name="dstPath">解压目标路径 (文件则解压到同目录，目录则解压到该目录)</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr decompressFile(const suPath& srcPath, const suPath& dstPath = suPath(""));

    /// ==================== 内存数据块操作 ====================
    /// 注意：7Z 格式通常用于文件归档，纯内存数据压缩建议使用 GZIP 或 BZ2

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
    /// <param name="inData">输入数据 (7Z 格式)</param>
    /// <param name="outData">输出数据 (自动分配内存)</param>
    /// <returns>成功返回 eCompressErr::Ok，失败返回错误码</returns>
    static eCompressErr decompressBin(const std::string& inData, std::string& outData);

    /// <summary>
    /// 压缩内存数据块 (std::string 输入，std::vector<char> 输出 - 自动分配内存)
    /// </summary>
    static eCompressErr compressBin(const std::string& inData, std::vector<char>& outData);

    /// <summary>
    /// 解压内存数据块 (std::vector<char> 输入，std::string 输出 - 自动分配内存)
    /// </summary>
    static eCompressErr decompressBin(const std::vector<char>& inData, std::string& outData);

    /// <summary>
    /// 获取 7Z 库版本信息
    /// </summary>
    static std::string getVersionInfo();

private:
    // 内部实现细节隐藏
    static eCompressErr compressData(const char* inData, size_t inLen, char** outData, size_t& outLen);
    static eCompressErr decompressData(const char* inData, size_t inLen, char** outData, size_t& outLen);
};

#endif  // SILLY_7Z_H
