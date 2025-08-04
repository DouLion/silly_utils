/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-04
 * @file: silly_compress.c
 * @description: silly_compress实现
 * @version: v1.0.1 2025-08-04 dou li yang
 */
#include "silly_compress.h"
eCompressErr su::compress_file(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method)
{
    
}
eCompressErr su::compress_dir(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method)
{
}
eCompressErr su::compress_bin(const std::string& src, std::string& dst, const eCompressMethod& method)
{
}
std::string su::compress_bin(const std::string& src, const eCompressMethod& method)
{
}
eCompressErr su::decompress_file(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method)
{
}
eCompressErr su::decompress_dir(const std::filesystem::path& src, const std::filesystem::path& dst, const eCompressMethod& method)
{
}
eCompressErr su::decompress_bin(const std::string& src, std::string& dst, const eCompressMethod& method)
{
}
std::string su::decompress_bin(const std::string& src, const eCompressMethod& method)
{
}
eCompressErr su::decompress_file(const std::filesystem::path& src, const std::filesystem::path& dst)
{
}
eCompressErr su::decompress_dir(const std::filesystem::path& src, const std::filesystem::path& dst)
{
}
eCompressErr su::decompress_bin(const std::string& src, std::string& dst)
{
}
std::string su::decompress_bin(const std::string& src)
{
}