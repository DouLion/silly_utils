//
// Created by dell on 2024/7/22.
//

#ifndef SILLY_COMPRESS_H
#define SILLY_COMPRESS_H

namespace silly_compress
{
enum class CPS_ERR
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
    Z7zDecompressErr = 401

};
}






#endif //SILLY_COMPRESS_H
