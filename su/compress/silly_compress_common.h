/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-04
 * @file: silly_compress_common.cpp
 * @description: silly_compress_common 头文件
 * @version: v1.0.1 2025-08-04 dou li yang
 */

#ifndef SILLY_COMPRESS_COMMON_H
#define SILLY_COMPRESS_COMMON_H

enum class eCompressMethod
{
    BZ2 = 1,
    GZIP = 2,
    ZIP = 3,
    MINIZ = 4,
    RAR = 5,
    Zip7Z = 6,
};

enum class eCompressErr
{
    Ok = 0,
    EmptyInputErr = 1,
    MallocErr = 2,
    FileNotExistErr = 3,
    MemAllocErr = 4,
    InValidInputErr = 5,
    InValidOutputErr = 6,
    // bz2的错误
    Bz2DecompressErr = 101,
    Bz2NotSupportDirErr = 102,
    Bz2CompressErr = 103,
    Bz2OpenFileErr = 104,

    // minizip的错误
    MiniZDecompressErr = 201,
    MiniZCreatZipErr = 202,
    MiniZOpenFileErr = 203,
    MiniZCreatDirErr = 204,
    MiniZFileEmptyErr = 205,
    MiniZGetInforErr = 206,

    MiniZCompressStrErr = 207,
    MiniZUncompressStrErr = 208,
    MiniZUnknowErr = 299,
    // gzip的错误

    GZipDecompressErr = 301,

    // 7z的错误
    Z7zDecompressErr = 401,

    // rar的错误
    RAROpenErr = 501,
    RARCreatDirErr = 502,
    RARSuportFormatErr = 503,
    RARWriteErr = 504,
    RARReadErr = 505

};

#endif
