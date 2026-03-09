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
#ifndef SILLY_BZ2_H
#define SILLY_BZ2_H
#include <log/silly_log.h>
#include <compress/silly_compress.h>

/*
 * bzip2比传统的gzip或者ZIP的压缩效率更高，但是压缩速度较慢。
 * 从这点来说，非常类似于较新的压缩算法。与RAR或者ZIP等其它不同的是，
 * bzip2只是数据压缩工具，而不是归档工具，在这一点与gzip类似。!!!!!!!!这一行比较重要!!!!!!!!!!!!!
 * 程序本身不包含用于多个文件、加密或者文档切分的工具，相反按照UNIX的传统需要使用如tar或者GnuPG这样的外部工具。
 * */

// TODO: 某些情况bz2解压会有问题
class suBz2
{
  public:
    /// <summary>
    /// 压缩文件,不支持文件夹,bz2不具有归档能力
    /// </summary>
    /// <param name="inFile">待压缩文件路径</param>
    /// <param name="outFile">压缩输出路径</param>
    /// <returns></returns>
    static eCompressErr compress(const suPath &inFile, const suPath &outFile);

    /// <summary>
    ///  解压文件,不支持文件夹,bz2不具有归档能力
    /// </summary>
    /// <param name="inFile">待解压文件路径</param>
    /// <param name="outFile">解压输出路径</param>
    /// <returns></returns>
    static eCompressErr decompress(const suPath &inFile, const suPath &outFile);

    /// <summary>
    /// 压缩内存数据
    /// </summary>
    /// <param name="inBin">待压缩内存数据</param>
    /// <param name="inLen">待压缩内存数据大小</param>
    /// <param name="outBin">压缩数据输出</param>
    /// <param name="outLen">压缩数据输出大小</param>
    /// <returns></returns>
    static eCompressErr compress(const char *inBin, const size_t &inLen, char **outBin, size_t &outLen);
    static eCompressErr compress(const std::string &inBin, std::string &outBin);

    /// <summary>
    /// FIXIT: 解压内存数据, 字符串解压有问题,末尾数据不对
    /// </summary>
    /// <param name="inBin">待解压内存数据</param>
    /// <param name="inLen">待解压内存数据大小</param>
    /// <param name="outBin">解压数据输出</param>
    /// <param name="outLen">解压数据输出大小</param>
    /// <returns></returns>
    static eCompressErr decompress(const char *inBin, const size_t &inLen, char **outBin, size_t &outLen);
    static eCompressErr decompress(const std::string &inBin, std::string &outBin);

    static bool valid(const std::string &bin);
};

#endif  // SILLY_BZ2_H
