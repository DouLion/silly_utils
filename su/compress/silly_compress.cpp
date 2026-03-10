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
#include "silly_compress.hpp"
#include <compress/silly_minizip.h>
#include <compress/silly_7z.h>
#include <compress/silly_bz2.h>
#include <compress/silly_gzip.h>
#include <compress/silly_rar.h>
#include <compress/silly_zip.h>

eCompressErr CompressFile(const suPath& src, const suPath& dst, const eCompressMethod& method)
{
    return eCompressErr::NotImplement;
}
eCompressErr CompressDir(const suPath& src, const suPath& dst, const eCompressMethod& method)
{
    return eCompressErr::NotImplement;
}
eCompressErr CompressBin(const std::string& src, std::string& dst, const eCompressMethod& method)
{
    eCompressErr ret = eCompressErr::NotImplement;
    if (src.empty())
    {
        dst.clear();
        return eCompressErr::Ok;
    }
    if (eCompressMethod::ZIP == method)
    {
        char* out = nullptr;
        size_t out_len = 0;
        ret = suMiniZIP::compress(src.data(), src.size(), &out, out_len);
        if (eCompressErr::Ok == ret)
        {
            dst.resize(out_len);
            std::memcpy(dst.data(), out, out_len);
        }
        SU_MEM_FREE(out)
    }
    else if (eCompressMethod::BZ2 == method)
    {
        suBz2 bz2;
        char* out = nullptr;
        size_t out_len = 0;
        ret = bz2.compress(src.data(), src.size(), &out, out_len);
        if (eCompressErr::Ok == ret)
        {
            dst.resize(out_len);
            std::memcpy(dst.data(), out, out_len);
        }
        SU_MEM_FREE(out)
    }
    else if (eCompressMethod::RAR == method)
    {
    }
    else if (eCompressMethod::LZ4 == method)
    {
        ret = eCompressErr::NotImplement;
    }
    else if (eCompressMethod::Brotli == method)
    {
        ret = eCompressErr::NotImplement;
    }
    else
    {
        ret = eCompressErr::InvalidMethod;
    }
    return ret;
}

std::string CompressBin(const std::string& src, const eCompressMethod& method)
{
    std::string ret;
    CompressBin(src, ret, method);
    return ret;
}

eCompressErr DecompressFile(const suPath& src, const suPath& dst, const eCompressMethod& method)
{
    return eCompressErr::NotImplement;
}
eCompressErr DecompressDir(const suPath& src, const suPath& dst, const eCompressMethod& method)
{
    return eCompressErr::NotImplement;
}
eCompressErr DecompressBin(const std::string& src, std::string& dst, const eCompressMethod& method)
{
    return eCompressErr::NotImplement;
}
std::string DecompressBin(const std::string& src, const eCompressMethod& method)
{
    return "";
}
eCompressErr AutoDecompressFile(const suPath& src, const suPath& dst)
{
    return eCompressErr::NotImplement;
}
eCompressErr AutoDecompressDir(const suPath& src, const suPath& dst)
{
    return eCompressErr::NotImplement;
}
eCompressErr AutoDecompressBin(const std::string& src, std::string& dst)
{
    return eCompressErr::NotImplement;
}
std::string AutoDecompressBin(const std::string& src)
{
    return "";
}