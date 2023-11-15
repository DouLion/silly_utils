//
// Created by dell on 2023/8/10.
//

#include "silly_minizip.h"


int silly_minizip::compressZip(std::string src, std::string dst)
{

    if (src.find_last_of("/") == src.length() - 1)  //去掉最后的/
        src = src.substr(0, src.length() - 1);

#ifdef __linux__
    struct stat64 fileInfo;
    stat64(src.c_str(), &fileInfo);
#else
    struct _stat64 fileInfo;
    _stat64(src.c_str(), &fileInfo);
#endif
    if (S_ISREG(fileInfo.st_mode))  // 文件
    {
        size_t pos = src.find_last_of("/");
        std::string srcFireName = src.substr(pos + 1); // 目录名(需要压缩的目录)
        zipFile zFile = zipOpen(dst.c_str(), APPEND_STATUS_CREATE);
        if (zFile == NULL)
        {
            return SILLY_ZIP_err_write_newzip;  //打开要写入的zip失败
        }
        zip_fileinfo zFileInfo = { 0 };
        int ret = zipOpenNewFileInZip(zFile, srcFireName.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
        if (ret != ZIP_OK)
        {
            zipClose(zFile, NULL);
            return SILLY_ZIP_err_open_file;  // 打开读取文件失败
        }
        ret = writeInZipFile(zFile, src);
        if (ret != ZIP_OK)
        {
            zipClose(zFile, NULL);
            return SILLY_ZIP_err_write_newzip;  // 打开新写入的ZIP失败
        }
        zipClose(zFile, NULL);
    }
    else if (S_ISDIR(fileInfo.st_mode))  // 目录
    {
        size_t pos = src.find_last_of("/");
        std::string dirName = src.substr(pos + 1); // 目录名(需要压缩的目录)
        std::string dirPrefix = src.substr(0, pos); // 需要压缩目录所在目录的绝对路径
        zipFile zFile = zipOpen(dst.c_str(), APPEND_STATUS_CREATE);
        if (zFile == NULL)
        {
            return SILLY_ZIP_err_write_newzip;  //打开要写入的zip失败
        }
        std::vector<std::string> vFiles;
        EnumDirFiles(dirPrefix, dirName, vFiles);
        std::vector<std::string>::iterator itF = vFiles.begin();
        for (; itF != vFiles.end(); ++itF)
        {
            zip_fileinfo zFileInfo = { 0 };
            int ret = zipOpenNewFileInZip(zFile, itF->c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
            if (ret != ZIP_OK)
            {
                zipClose(zFile, NULL);
                return SILLY_ZIP_err_open_file;  // 打开读取文件失败
            }
            std::string doc_file = (dirPrefix + "/" + *itF);
            if (std::filesystem::is_regular_file(doc_file))   // 判断是否为文件
            {
                ret = writeInZipFile(zFile, doc_file);
            }
            if (ret != ZIP_OK)
            {
                zipClose(zFile, NULL);
                return SILLY_ZIP_err_write_newzip;  // 打开新写入的ZIP失败
            }
        }

        zipClose(zFile, NULL);
    }
    return SILLY_ZIP_ok;
}



int silly_minizip::decompressZip(const std::string& zipFileName, const std::string& outputFireDir)
{
    int status = SILLY_ZIP_ok;
    std::string outputDirectory = outputFireDir;
    if (outputDirectory.empty())// 如果解压路径为空,默认为压缩包所在目录
    {
        std::filesystem::path defOutputDir = std::filesystem::path(zipFileName).parent_path();
        outputDirectory = defOutputDir.string();
    }
    // 打开ZIP文件
    unzFile zipFile = unzOpen(zipFileName.c_str());
    if (zipFile == nullptr)
    {
        return SILLY_ZIP_err_open;   // 打开ZIP文件失败
    }
    // 创建输出目录
    std::filesystem::path outputDir(outputDirectory);
    if (!std::filesystem::exists(outputDir))
    {
        if (!std::filesystem::create_directory(outputDir))
        {
            unzClose(zipFile);
            return SILLY_ZIP_err_create_dir;        // 创建目录失败
        }
    }
    // 进入ZIP文件中的第一个文件
    if (unzGoToFirstFile(zipFile) != UNZ_OK)
    {
        unzClose(zipFile);
        return SILLY_ZIP_err_empty; // ZIP文件为空
    }
    // 循环解压缩所有文件
    do
    {
        char filename[512];
        unz_file_info file_info;
        // 获取当前文件信息
        if (unzGetCurrentFileInfo(zipFile, &file_info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            status = SILLY_ZIP_err_get_info;    // 获取文件信息失败
            break;
        }

        std::string filePath = outputDirectory + "/" + filename;
        // TODO: 这里检测一下有没有其他写法, 创建目录或文件
        if (filename[file_info.size_filename - 1] == '/')    // 目录
        {
            std::filesystem::create_directories(filePath);
        }
        else       // 文件
        {
            // 创建父目录
            std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
            // 写入文件
            std::ofstream outFile(filePath, std::ios::binary);
            if (!outFile.is_open())
            {
                status = SILLY_ZIP_err_create_file; // 创建文件失败
                break;
            }
            else
            {
                char buffer[10000];
                int readSize;
                unzOpenCurrentFile(zipFile);

                while ((readSize = unzReadCurrentFile(zipFile, buffer, sizeof(buffer))) > 0)
                {
                    outFile.write(buffer, readSize);
                }
                outFile.close();
            }
        }

    } while (unzGoToNextFile(zipFile) == UNZ_OK);

    unzClose(zipFile);

    return status;
}




int silly_minizip::writeInZipFile(zipFile zFile, const std::string& file)
{
    std::fstream f(file.c_str(), std::ios::binary | std::ios::in);
    if (!f.is_open())
    {
        std::cout << "Failed to open file: " << file << std::endl;
    }
    f.seekg(0, std::ios::end);
    size_t size = f.tellg();  //获取文件大小
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


void silly_minizip::EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles)
{
    std::filesystem::path rel(dirPrefix);
    std::filesystem::path root = rel;
    root.append(dirName);
    for (auto fiter : std::filesystem::recursive_directory_iterator(root))
    {
        if (fiter.is_directory()) // 目录
        {
            if (std::filesystem::is_empty(fiter.path()))  // 检测目录是否为空
            {
                std::string target = std::filesystem::relative(fiter.path(), rel).string();
                boost::replace_all(target, "\\", "/");
                vFiles.push_back(target + "/");  // 添加尾部斜线以表示ZIP中的空目录
            }
        }
        else  // 文件
        {
            std::string target = std::filesystem::relative(fiter.path(), rel).string();
            boost::replace_all(target, "\\", "/");
            vFiles.push_back(target);
        }
    }
}














