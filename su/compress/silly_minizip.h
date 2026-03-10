/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 10:08
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_MINIZIP_H
#define SILLY_MINIZIP_H
#include <compress/silly_compress.h>
#include <log/silly_log.h>
#include <files/silly_file.h>
class suMiniZIP
{
  public:
    /// <summary>
    /// 将文件或目录压缩为ZIP文件
    /// </summary>
    /// <param name="src">被压缩文件或目录地址</param>
    /// <param name="dst">生成解压文件路径</param>
    /// <returns></returns>
    /// 注: 测试压缩3G以上ios文件有问题
    static eCompressErr compress(const suPath& src, const suPath& dst = "", const bool& append = false);

    /// <summary>
    /// 解压zip文件,解压单独文件和目录文件
    /// </summary>
    /// <param name="src">待解压文件路径</param>
    /// <param name="dst">解压输出路径</param>
    /// <returns></returns>
    static eCompressErr decompress(const suPath& src, const suPath& dst = "");

    /// <summary>
    /// 压缩内存数据
    /// </summary>
    /// <param name="inBin">待压缩内存数据</param>
    /// <param name="inLen">待压缩内存数据大小</param>
    /// <param name="outBin">压缩数据输出</param>
    /// <param name="outLne">压缩数据输出大小</param>
    /// <returns></returns>
    /// 注: 如果inBin是一个字符串类型的数据要考虑字符串结尾的'\0',需对字符串类型的inBin进行strlen(inBin) + 1
    static eCompressErr compress(const char* inBin, const size_t& inLen, char** outBin, size_t& outLne);

    /// <summary>
    /// 解压内存数据
    /// </summary>
    /// <param name="inBin">待解压内存数据</param>
    /// <param name="inLen">待解压内存数据大小</param>
    /// <param name="outBin">解压数据输出</param>
    /// <param name="outLne">解压数据输出大小</param>
    /// <returns></returns>
    static eCompressErr decompress(const char* inBin, const size_t& inLen, char** outBin, size_t& outLne);
};


#endif  // SILLY_MINIZIP_H
