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
#include <filesystem>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "minizip/zip.h"
#include "minizip/unzip.h"
#include <boost/algorithm/string.hpp>


enum ZIP_ERROR {
    SILLY_ZIP_OK = 0,                 // 操作成功
    SILLY_ZIP_ERROR_OPEN = 1,         // 打开ZIP文件失败
    SILLY_ZIP_ERROR_EMPTY = 2,        // ZIP文件为空
    SILLY_ZIP_ERROR_GET_INFO = 3,     // 获取文件信息失败
    SILLY_ZIP_ERROR_CREATE_DIR = 4,   // 创建目录失败
    SILLY_ZIP_ERROR_CREATE_FILE = 5,  // 创建文件失败
    SILLY_ZIP_ERROR_WRITE_FILE = 6,   // 写入文件失败
    SILLY_ZIP_ERROR_WRITE_ZIP = 7,    // 写入ZIP文件失败
    SILLY_ZIP_ERROR_WRITE_NEWZIP = 8     // 打开新写入的ZIP失败

};


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
