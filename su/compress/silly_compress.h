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

static eCompressErr compress_file(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method = eCompressMethod::GZIP);

static eCompressErr compress_dir(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method = eCompressMethod::GZIP);

static eCompressErr compress_bin(const std::string& src, std::string& dst, const eCompressMethod& method = eCompressMethod::GZIP);

static std::string compress_bin(const std::string& src, const eCompressMethod& method = eCompressMethod::GZIP);

static eCompressErr decompress_file(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method);

static eCompressErr decompress_dir(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method);

static eCompressErr decompress_bin(const std::string& src, std::string& dst, const eCompressMethod& method);

static std::string decompress_bin(const std::string& src, const eCompressMethod& method);

static eCompressErr decompress_file(const std::filesystem::path& src, const std::filesystem::path& dst);

static eCompressErr decompress_dir(const std::filesystem::path& src, const std::filesystem::path& dst);

static eCompressErr decompress_bin(const std::string& src, std::string& dst);

static std::string decompress_bin(const std::string& src);

}  // namespace su

#endif  // SILLY_COMPRESS_H
