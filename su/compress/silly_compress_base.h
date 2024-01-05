/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/11/14 17:58
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_COMPRESS_BASE_H
#define SILLY_UTILS_SILLY_COMPRESS_BASE_H

#include <iostream>

class silly_compress_base
{
  public:
    enum silly_compress_err
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
        // gzip的错误

        GZipDecompressErr = 301,

        // 7z的错误
        Z7zDecompressErr = 401

    };

  public:
    /// <summary>
    /// 压缩文件或者文件夹,基于不同压缩算法的能力
    /// </summary>
    /// <param name="s_src"></param>
    /// <param name="s_dst"></param>
    /// <returns></returns>
    virtual int compress(const std::string &s_src, const std::string &s_dst) = 0;

    /// <summary>
    ///  解压文件,基于不同压缩算法的能力
    /// </summary>
    /// <param name="s_src"></param>
    /// <param name="s_dst"></param>
    /// <returns></returns>
    virtual int decompress(const std::string &s_src, const std::string &s_dst) = 0;

    /// <summary>
    /// 压缩内存数据,基于不同压缩算法的能力
    /// </summary>
    /// <param name="c_in_val"></param>
    /// <param name="i_in_len"></param>
    /// <param name="c_out_val"></param>
    /// <param name="i_out_len"></param>
    /// <returns></returns>
    virtual int compress(const char *c_in_val, const size_t &i_in_len, char **c_out_val, size_t &i_out_len) = 0;

    /// <summary>
    /// 解压内存数据,基于不同压缩算法的能力
    /// </summary>
    /// <param name="c_in_val"></param>
    /// <param name="i_in_len"></param>
    /// <param name="c_out_val"></param>
    /// <param name="i_out_len"></param>
    /// <returns></returns>
    virtual int decompress(const char *c_in_val, const size_t &i_in_len, char **c_out_val, size_t &i_out_len) = 0;
};

#endif  // SILLY_UTILS_SILLY_COMPRESS_BASE_H
