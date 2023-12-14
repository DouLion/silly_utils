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

#ifndef SILLY_UTILS_SILLY_MINIZIP_H
#define SILLY_UTILS_SILLY_MINIZIP_H

#include <iostream>
#include <compress/silly_compress_base.h>

// TODO : 王英杰  头文件中能不暴露出来的include 文件 不要暴露出来
#include <vector>
#include "minizip/zip.h"
#include "minizip/unzip.h"

// TODO : 王英杰  枚举放到silly_compress_base 的错误枚举类中, 保持格式一致
enum ZIP_ERROR {
    SILLY_ZIP_ok = 0,                     // 操作成功
    SILLY_ZIP_err_open = 1,               // 打开ZIP文件失败
    SILLY_ZIP_err_empty = 2,              // ZIP文件为空
    SILLY_ZIP_err_get_info = 3,           // 获取文件信息失败
    SILLY_ZIP_err_create_dir = 4,         // 创建目录失败
    SILLY_ZIP_err_create_file = 5,        // 创建文件失败
    SILLY_ZIP_err_write_file = 6,         // 写入文件失败
    SILLY_ZIP_err_write_zip = 7,          // 写入ZIP文件失败
    SILLY_ZIP_err_write_newzip = 8,       // 打开新写入的ZIP失败
    SILLY_ZIP_err_open_file = 9           // 打开读取文件失败

};

// TODO : 王英杰 继承silly_compress_base  实现四个虚函数, 参照silly_bz2.h
class silly_minizip
{
public:

    
    /// <summary>
    /// 将文件或目录压缩为ZIP文件
    /// </summary>
    /// <param name="src">被压缩文件的地址</param>
    /// <param name="dst">生成压缩包地址</param>
    /// <returns></returns>
    static int compressZip(std::string src, std::string dst);


    /// <summary>
    /// 解压zip文件,解压单独文件和国际目录文件
    /// </summary>
    /// <param name="zipFileName">zip文件路径</param>
    /// <param name="outputDirectory">生成解压文件路径</param>
    /// <returns></returns>
    static int decompressZip(const std::string& zipFileName, const std::string& outputDir = "");

private:

    /// <summary>
    /// 将一个文件内容写入到一个 ZIP 文件中
    /// </summary>
    /// <param name="zFile"></param>
    /// <param name="file">需要读取的文件</param>
    /// <returns></returns>
    static int writeInZipFile(zipFile zFile, const std::string& file);

    /// <summary>
    /// 列举指定目录及其子目录中的所有文件和子目录
    /// </summary>
    /// <param name="dirPrefix">需压缩目录所在目录的绝对路径</param>
    /// <param name="dirName">需压缩目录名(例如:dirName=comp)</param>
    /// <param name="vFiles">dirName目录下所有的文件和子目录相对路径(相对需要所目录的路径:comp/file2/1.txt")</param>
    static void EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles);


};

#endif //SILLY_UTILS_SILLY_MINIZIP_H
