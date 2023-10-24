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

class silly_minizip
{
public:
    static void EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles)
    {
        if (dirPrefix.empty() || dirName.empty())
            return;
        std::string dirNameTmp = dirName;
        std::string dirPre = dirPrefix;

        if (dirNameTmp.find_last_of("/") != dirNameTmp.length() - 1)
            dirNameTmp += "/";
        if (dirNameTmp[0] == '/')
            dirNameTmp = dirNameTmp.substr(1);
        if (dirPre.find_last_of("/") != dirPre.length() - 1)
            dirPre += "/";

        std::string path;

        path = dirPre + dirNameTmp;


        struct stat fileStat;
        DIR* pDir = opendir(path.c_str());
        if (!pDir) return;

        struct dirent* pDirEnt = NULL;
        while ((pDirEnt = readdir(pDir)) != NULL)
        {
            if (strcmp(pDirEnt->d_name, ".") == 0 || strcmp(pDirEnt->d_name, "..") == 0)
                continue;

            std::string tmpDir = dirPre + dirNameTmp + pDirEnt->d_name;
            if (stat(tmpDir.c_str(), &fileStat) != 0)
                continue;

            std::string innerDir = dirNameTmp + pDirEnt->d_name;
            if (fileStat.st_mode & S_IFDIR == S_IFDIR)
            {
                EnumDirFiles(dirPrefix, innerDir, vFiles);
                continue;
            }

            vFiles.push_back(innerDir);
        }

        if (pDir)
            closedir(pDir);
    }

    static int writeInZipFile(zipFile zFile, const std::string& file)
    {
        std::fstream f(file.c_str(), std::ios::binary | std::ios::in);
        f.seekg(0, std::ios::end);
        long size = f.tellg();
        f.seekg(0, std::ios::beg);
        if (size <= 0)
        {
            return zipWriteInFileInZip(zFile, NULL, 0);
        }
        char* buf = new char[size];
        f.read(buf, size);
        int ret = zipWriteInFileInZip(zFile, buf, size);
        delete[] buf;
        return ret;
    }

    static void EnumDirFiles2(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles)
    {
        std::filesystem::path rel(dirPrefix);
        std::filesystem::path root = rel;
        root.append(dirName);
        for (auto fiter : std::filesystem::recursive_directory_iterator(root))
        {
            if (fiter.is_directory())
            {
                continue;
            }
            std::string target = std::filesystem::relative(fiter.path(), rel).string();
            boost::replace_all(target, "\\", "/");
            vFiles.push_back(target);
        }
    }


    static int compressu(std::string src, std::string dst)
    {
        
        if (src.find_last_of("/") == src.length() - 1)
            src = src.substr(0, src.length() - 1);

        struct stat fileInfo;
        stat(src.c_str(), &fileInfo);
        if (S_ISREG(fileInfo.st_mode))
        {
            zipFile zFile = zipOpen(dst.c_str(), APPEND_STATUS_CREATE);
            if (zFile == NULL)
            {
                std::cout << "openfile failed" << std::endl;
                return -1;
            }
            zip_fileinfo zFileInfo = { 0 };
            int ret = zipOpenNewFileInZip(zFile, src.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, 0, Z_DEFAULT_COMPRESSION);
            if (ret != ZIP_OK)
            {
                std::cout << "openfile in zip failed" << std::endl;
                zipClose(zFile, NULL);
                return -1;
            }
            ret = writeInZipFile(zFile, src);
            if (ret != ZIP_OK)
            {
                std::cout << "write in zip failed" << std::endl;
                zipClose(zFile, NULL);
                return -1;
            }
            zipClose(zFile, NULL);
            std::cout << "zip file ok" << std::endl;
        }
        else if (S_ISDIR(fileInfo.st_mode))
        {
            size_t pos = src.find_last_of("/");
            std::string dirName = src.substr(pos + 1);
            std::string dirPrefix = src.substr(0, pos);

            zipFile zFile = zipOpen(dst.c_str(), APPEND_STATUS_CREATE);
            if (zFile == NULL)
            {
                std::cout << "openfile failed" << std::endl;
                return -1;
            }

            std::vector<std::string> vFiles;
           
            EnumDirFiles2(dirPrefix, dirName, vFiles);
           /* std::cout << dirPrefix << std::endl;
            std::cout << dirName << std::endl;
            EnumDirFiles(dirPrefix, dirName, vFiles);*/
            std::vector<std::string>::iterator itF = vFiles.begin();
            for (; itF != vFiles.end(); ++itF)
            {
                zip_fileinfo zFileInfo = { 0 };
                std::cout << itF->c_str() << std::endl;
                int ret = zipOpenNewFileInZip(zFile, itF->c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, 0, Z_DEFAULT_COMPRESSION);
                if (ret != ZIP_OK)
                {
                    std::cout << "openfile in zip failed" << std::endl;
                    zipClose(zFile, NULL);
                    return -1;
                }
                ret = writeInZipFile(zFile, *itF);
                if (ret != ZIP_OK)
                {
                    std::cout << "write in zip failed" << std::endl;
                    zipClose(zFile, NULL);
                    return -1;
                }
            }

            zipClose(zFile, NULL);
            std::cout << "zip dir ok" << std::endl;
        }
        return 0;
    }
};

#endif //SILLY_UTILS_SILLY_MINIZIP_H
