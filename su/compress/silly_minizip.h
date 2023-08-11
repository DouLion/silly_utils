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
using namespace std;

class silly_minizip
{
public:
    static void EnumDirFiles(const string& dirPrefix, const string& dirName, vector<string>& vFiles)
    {
        if (dirPrefix.empty() || dirName.empty())
            return;
        string dirNameTmp = dirName;
        string dirPre = dirPrefix;

        if (dirNameTmp.find_last_of("/") != dirNameTmp.length() - 1)
            dirNameTmp += "/";
        if (dirNameTmp[0] == '/')
            dirNameTmp = dirNameTmp.substr(1);
        if (dirPre.find_last_of("/") != dirPre.length() - 1)
            dirPre += "/";

        string path;

        path = dirPre + dirNameTmp;


        struct stat fileStat;
        DIR* pDir = opendir(path.c_str());
        if (!pDir) return;

        struct dirent* pDirEnt = NULL;
        while ((pDirEnt = readdir(pDir)) != NULL)
        {
            if (strcmp(pDirEnt->d_name, ".") == 0 || strcmp(pDirEnt->d_name, "..") == 0)
                continue;

            string tmpDir = dirPre + dirNameTmp + pDirEnt->d_name;
            if (stat(tmpDir.c_str(), &fileStat) != 0)
                continue;

            string innerDir = dirNameTmp + pDirEnt->d_name;
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

    static int writeInZipFile(zipFile zFile, const string& file)
    {
        fstream f(file.c_str(), std::ios::binary | std::ios::in);
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
                cout << "openfile failed" << endl;
                return -1;
            }
            zip_fileinfo zFileInfo = { 0 };
            int ret = zipOpenNewFileInZip(zFile, src.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, 0, Z_DEFAULT_COMPRESSION);
            if (ret != ZIP_OK)
            {
                cout << "openfile in zip failed" << endl;
                zipClose(zFile, NULL);
                return -1;
            }
            ret = writeInZipFile(zFile, src);
            if (ret != ZIP_OK)
            {
                cout << "write in zip failed" << endl;
                zipClose(zFile, NULL);
                return -1;
            }
            zipClose(zFile, NULL);
            cout << "zip ok" << endl;
        }
        else if (S_ISDIR(fileInfo.st_mode))
        {
            size_t pos = src.find_last_of("/");
            string dirName = src.substr(pos + 1);
            string dirPrefix = src.substr(0, pos);

            zipFile zFile = zipOpen(dst.c_str(), APPEND_STATUS_CREATE);
            if (zFile == NULL)
            {
                cout << "openfile failed" << endl;
                return -1;
            }

            vector<string> vFiles;
            EnumDirFiles(dirPrefix, dirName, vFiles);
            vector<string>::iterator itF = vFiles.begin();
            for (; itF != vFiles.end(); ++itF)
            {
                zip_fileinfo zFileInfo = { 0 };
                int ret = zipOpenNewFileInZip(zFile, itF->c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, 0, Z_DEFAULT_COMPRESSION);
                if (ret != ZIP_OK)
                {
                    cout << "openfile in zip failed" << endl;
                    zipClose(zFile, NULL);
                    return -1;
                }
                ret = writeInZipFile(zFile, *itF);
                if (ret != ZIP_OK)
                {
                    cout << "write in zip failed" << endl;
                    zipClose(zFile, NULL);
                    return -1;
                }
            }

            zipClose(zFile, NULL);
            cout << "zip ok" << endl;
        }
        return 0;
    }
};

#endif //SILLY_UTILS_SILLY_MINIZIP_H
