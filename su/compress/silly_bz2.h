/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 11:10
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_BZ2_H
#define SILLY_UTILS_SILLY_BZ2_H
#include <iostream>
#include <compress/silly_compress.h>

/*
 * bzip2比传统的gzip或者ZIP的压缩效率更高，但是压缩速度较慢。
 * 从这点来说，非常类似于较新的压缩算法。与RAR或者ZIP等其它不同的是，
 * bzip2只是数据压缩工具，而不是归档工具，在这一点与gzip类似。!!!!!!!!这一行比较重要!!!!!!!!!!!!!
 * 程序本身不包含用于多个文件、加密或者文档切分的工具，相反按照UNIX的传统需要使用如tar或者GnuPG这样的外部工具。
 * */

namespace silly_compress
{
class BZ2
{
  public:
    /// <summary>
    /// 压缩文件,不支持文件夹,bz2不具有归档能力
    /// </summary>
    /// <param name="s_src">待压缩文件路径</param>
    /// <param name="s_dst">压缩输出路径</param>
    /// <returns></returns>
    static CPS_ERR compress(const std::string &s_src, const std::string &s_dst);

    /// <summary>
    ///  解压文件,不支持文件夹,bz2不具有归档能力
    /// </summary>
    /// <param name="s_src">待解压文件路径</param>
    /// <param name="s_dst">解压输出路径</param>
    /// <returns></returns>
    static CPS_ERR decompress(const std::string &s_src, const std::string &s_dst);

    /// <summary>
    /// 压缩内存数据
    /// </summary>
    /// <param name="c_in_val">待压缩内存数据</param>
    /// <param name="i_in_len">待压缩内存数据大小</param>
    /// <param name="c_out_val">压缩数据输出</param>
    /// <param name="i_out_len">压缩数据输出大小</param>
    /// <returns></returns>
    static CPS_ERR compress(const char *c_in_val, const size_t &i_in_len, char **c_out_val, size_t &i_out_len);

    /// <summary>
    /// FIXIT: 解压内存数据, 字符串解压有问题,末尾数据不对
    /// </summary>
    /// <param name="c_in_val">待解压内存数据</param>
    /// <param name="i_in_len">待解压内存数据大小</param>
    /// <param name="c_out_val">解压数据输出</param>
    /// <param name="i_out_len">解压数据输出大小</param>
    /// <returns></returns>
    static CPS_ERR decompress(const char *c_in_val, const size_t &i_in_len, char **c_out_val, size_t &i_out_len);
};
}

typedef silly_compress::BZ2 silly_bz2;


#endif  // SILLY_UTILS_SILLY_BZ2_H
