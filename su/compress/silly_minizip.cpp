//
// Created by dell on 2023/8/10.
//

#include "silly_minizip.h"



int silly_minizip::compressZip(std::string src, std::string dst)
{

    if (src.find_last_of("/") == src.length() - 1)  //去掉最后的/
        src = src.substr(0, src.length() - 1);

    struct stat fileInfo;
    stat(src.c_str(), &fileInfo);
    if (S_ISREG(fileInfo.st_mode))  // 文件
    {
        size_t pos = src.find_last_of("/");
        std::string srcFireName = src.substr(pos + 1); // 目录名(需要压缩的目录)
        zipFile zFile = zipOpen(dst.c_str(), APPEND_STATUS_CREATE);
        if (zFile == NULL)
        {
            std::cout << "openfile failed" << std::endl;
            return -1;
        }
        zip_fileinfo zFileInfo = { 0 };
        int ret = zipOpenNewFileInZip(zFile, srcFireName.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, 0, Z_DEFAULT_COMPRESSION);
        if (ret != ZIP_OK)
        {
            std::cout << "isreg openfile in zip failed" << std::endl;
            zipClose(zFile, NULL);
            return -1;
        }
        ret = writeInZipFile(zFile, src);
        if (ret != ZIP_OK)
        {
            std::cout << "isreg write in zip failed" << std::endl;
            zipClose(zFile, NULL);
            return -1;
        }
        zipClose(zFile, NULL);
        std::cout << "zip file ok" << std::endl;
    }
    else if (S_ISDIR(fileInfo.st_mode))  // 目录
    {
        size_t pos = src.find_last_of("/");
        std::string dirName = src.substr(pos + 1); // 目录名(需要压缩的目录)
        std::string dirPrefix = src.substr(0, pos); // 需要压缩目录所在目录的绝对路径
        zipFile zFile = zipOpen(dst.c_str(), APPEND_STATUS_CREATE);
        if (zFile == NULL)
        {
            std::cout << "openfile failed" << std::endl;
            return -1;
        }
        std::vector<std::string> vFiles;
        EnumDirFiles(dirPrefix, dirName, vFiles);
        std::vector<std::string>::iterator itF = vFiles.begin();
        for (; itF != vFiles.end(); ++itF)
        {
            zip_fileinfo zFileInfo = { 0 };
            int ret = zipOpenNewFileInZip(zFile, itF->c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, 0, Z_DEFAULT_COMPRESSION);
            if (ret != ZIP_OK)
            {
                std::cout << "isdir openfile in zip failed" << std::endl;
                zipClose(zFile, NULL);
                return -1;
            }
            std::string doc_file = (dirPrefix + "/" + *itF);
            if (std::filesystem::is_regular_file(doc_file))   // 判断是否为文件
            {
                ret = writeInZipFile(zFile, doc_file);
            }
            //ret = writeInZipFile(zFile, *itF);
            if (ret != ZIP_OK)
            {
                std::cout << "isdir write in zip failed" << std::endl;
                zipClose(zFile, NULL);
                return -1;
            }
        }

        zipClose(zFile, NULL);
        std::cout << "zip dir ok" << std::endl;
    }
    return 0;
}



bool silly_minizip::decompressZip(const std::string& zipFileName, const std::string& outputFireDir)
{
    std::string outputDirectory = outputFireDir;
    if (outputDirectory.empty())  // 如果解压路径为空,默认为压缩包所在目录
    {
        std::filesystem::path defOutputDir = std::filesystem::path(zipFileName).parent_path();
        outputDirectory = defOutputDir.string();
    }
    // 打开ZIP文件
    unzFile zipFile = unzOpen(zipFileName.c_str());
    if (zipFile == nullptr)
    {
        std::cout << "Failed to open ZIP file: " << zipFileName << std::endl;
        return false;
    }
    // 创建输出目录
    std::filesystem::path outputDir(outputDirectory);
    if (!std::filesystem::exists(outputDir)) 
    {
        std::filesystem::create_directory(outputDir);
    }
    // 进入ZIP文件中的第一个文件
    if (unzGoToFirstFile(zipFile) != UNZ_OK)
    {
        std::cout << "ZIP file is empty : " << zipFileName << std::endl;
        unzClose(zipFile);
        return false;
    }
    // 循环解压缩所有文件
    do
    {
        char filename[512];
        unz_file_info file_info;

        // 获取当前文件信息
        if (unzGetCurrentFileInfo(zipFile, &file_info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            std::cout << "Failed to get current file info in ZIP: " << zipFileName << std::endl;
            unzClose(zipFile);
            return false;
        }
        std::string filePath = outputDirectory + "/" + filename;
        // 创建目录或文件
        if (filename[file_info.size_filename - 1] == '/')  // 目录
        {
            std::filesystem::create_directories(filePath);
        }
        else   // 文件
        {
            // 创建父目录
            std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
            // 写入文件
            std::ofstream outFile(filePath, std::ios::binary); 
            if (!outFile.is_open())
            {
                std::cout << "Failed to create file: " << filePath << std::endl;
                unzClose(zipFile);
                return false;
            }
            char buffer[10000];
            int readSize;
            unzOpenCurrentFile(zipFile);
            while ((readSize = unzReadCurrentFile(zipFile, buffer, sizeof(buffer))) > 0)
            {
                outFile.write(buffer, readSize); // 逐行写入
            }
            outFile.close();
        }

    } while (unzGoToNextFile(zipFile) == UNZ_OK);

    // 关闭ZIP
    unzClose(zipFile);

    std::cout << "Decompression successful: " << zipFileName << std::endl;
    return true;
}
