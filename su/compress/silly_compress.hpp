/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2026-03-03
 * @file: silly_compress
 * @description: 压缩功能通用调用
 * @version: v1.0.1 2026-03-03 dou li yang
 */
#ifndef SILLY_COMPRESS_HPP
#define SILLY_COMPRESS_HPP
#include <compress/silly_compress.h>

/// ---------------- 压缩 ----------------
static eCompressErr CompressFile(const suPath& src, const suPath& dst, const eCompressMethod& method = eCompressMethod::ZIP);

static eCompressErr CompressDir(const suPath& src, const suPath& dst, const eCompressMethod& method = eCompressMethod::ZIP);

static eCompressErr CompressBin(const std::string& src, std::string& dst, const eCompressMethod& method = eCompressMethod::ZIP);

static std::string CompressBin(const std::string& src, const eCompressMethod& method = eCompressMethod::ZIP);

/// ---------------- 解压 ----------------

/// 指定解压格式
static eCompressErr DecompressFile(const suPath& src, const suPath& dst, const eCompressMethod& method);

static eCompressErr DecompressDir(const suPath& src, const suPath& dst, const eCompressMethod& method);

static eCompressErr DecompressBin(const std::string& src, std::string& dst, const eCompressMethod& method);

static std::string DecompressBin(const std::string& src, const eCompressMethod& method);

/// 自动判断解压格式
static eCompressErr DecompressFile(const suPath& src, const suPath& dst);

static eCompressErr DecompressDir(const suPath& src, const suPath& dst);

static eCompressErr DecompressBin(const std::string& src, std::string& dst);

static std::string DecompressBin(const std::string& src);

#endif