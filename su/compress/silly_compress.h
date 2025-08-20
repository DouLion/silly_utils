/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-04
 * @file: silly_compress.cpp
 * @description: silly_compress 头文件
 * @version: v1.0.1 2025-08-04 dou li yang
 */
#ifndef SILLY_COMPRESS_H
#define SILLY_COMPRESS_H
#include <files/silly_file.h>
#include <compress/silly_compress_common.h>
namespace su
{

/// ---------------- 压缩 ----------------

static eCompressErr CompressFile(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method = eCompressMethod::ZIP);

static eCompressErr CompressDir(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method = eCompressMethod::ZIP);

static eCompressErr CompressBin(const std::string& src, std::string& dst, const eCompressMethod& method = eCompressMethod::ZIP);

static std::string CompressBin(const std::string& src, const eCompressMethod& method = eCompressMethod::ZIP);

/// ---------------- 解压 ----------------

/// 指定解压格式
static eCompressErr DecompressFile(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method);

static eCompressErr DecompressDir(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method);

static eCompressErr DecompressBin(const std::string& src, std::string& dst, const eCompressMethod& method);

static std::string DecompressBin(const std::string& src, const eCompressMethod& method);

/// 自动判断解压格式
static eCompressErr AutoDecompressFile(const std::filesystem::path& src, const std::filesystem::path& dst);

static eCompressErr AutoDecompressDir(const std::filesystem::path& src, const std::filesystem::path& dst);

static eCompressErr AutoDecompressBin(const std::string& src, std::string& dst);

static std::string AutoDecompressBin(const std::string& src);

}  // namespace su

#endif  // SILLY_COMPRESS_H
