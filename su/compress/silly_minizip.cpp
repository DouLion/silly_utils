//
// Created by dell on 2023/8/10.
//

#include "silly_minizip.h"
#include <filesystem>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>


#include <string>
#include <vector>
#include <fstream>

#include <boost/algorithm/string.hpp>

#include "minizip/zip.h"
#include "minizip/unzip.h"
#include "su_marco.h"


/// <summary>
/// 将一个文件内容写入到一个 ZIP 文件中
/// </summary>
/// <param name="zFile"></param>
/// <param name="file">需要读取的文件</param>
/// <returns></returns>
int writeInZipFile(const zipFile& zFile, const std::string& file)
{
	int ret = -1;
	size_t max_size = SILLY_UTILS_4G_BYTE;   // 最大处理文件4G
	size_t block_size = SILLY_UTILS_1G_BYTE;  // 大于4G后拆分读取每次1G
	std::fstream srcFile(file.c_str(), std::ios::binary | std::ios::in);
	if (!srcFile.is_open())
	{
		std::cout << "Failed to open file: " << file << std::endl;
	}
	srcFile.seekg(0, std::ios::end);
	std::streampos size = srcFile.tellg();  //获取文件大小
	srcFile.seekg(0, std::ios::beg);
	if (size <= 0)
	{
		ret = zipWriteInFileInZip(zFile, NULL, 0);
	}
	else if (size > 0 && size < max_size)  // (0 ~ 4G) 左右都不包含
	{
		size_t mall = (size_t)size;
		char* buf = (char*)malloc(mall);
		srcFile.read(buf, size);
		ret = zipWriteInFileInZip(zFile, buf, size);
		SU_MEM_FREE(buf);

	}
	else if (size >= max_size)  // 文件大于等于4g, 拆分成1g的写入zip中
	{
		size_t remainingSize = size;
		while (remainingSize > 0)
		{
			size_t chunkSize = std::min(remainingSize, block_size);
			char* buffer = (char*)malloc(chunkSize);
			srcFile.read(buffer, chunkSize);
			ret = zipWriteInFileInZip(zFile, buffer, chunkSize);
			SU_MEM_FREE(buffer);
			if (ret != ZIP_OK)
			{
				SU_ERROR_PRINT("Failed to write chunk to zip file. ");
				return ret;
			}
			remainingSize -= chunkSize;
		}
	}
	return ret;
}

/// <summary>
/// 列举指定目录及其子目录中的所有文件和子目录
/// </summary>
/// <param name="dirPrefix">需压缩目录所在目录的绝对路径</param>
/// <param name="dirName">需压缩目录名(例如:dirName=comp)</param>
/// <param name="vFiles">dirName目录下所有的文件和子目录相对路径(相对需要所目录的路径:comp/file2/1.txt")</param>
void EnumDirFiles(const std::string& dirPrefix, const std::string& dirName, std::vector<std::string>& vFiles)
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
				std::filesystem::path relative_path = std::filesystem::relative(fiter.path(), rel);
				relative_path.append(""); // 是结尾有一个'/',表示relative_path是一个目录
				vFiles.push_back(relative_path.string());  // 添加尾部斜线以表示ZIP中的空目录
			}
		}
		else  // 文件
		{
			std::filesystem::path relative_path = std::filesystem::relative(fiter.path(), rel);
			vFiles.push_back(relative_path.string());  // 添加尾部斜线以表示ZIP中的空目录
		}
	}
}


// 压缩文件
int minizip_compress_file(const zipFile& zFile, const std::string& src)
{
	std::string srcFileName = std::filesystem::path(src).filename().string();
	zip_fileinfo zFileInfo = { 0 };
	//int ret = zipOpenNewFileInZip64(zFile, srcFileName.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_BEST_COMPRESSION ,1);
	int ret = zipOpenNewFileInZip3_64(zFile, srcFileName.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 1);
	if (ret != ZIP_OK)
	{
		SU_ERROR_PRINT("Failed to open file in zip: %s", src.c_str());
		return ret;
	}
	ret = writeInZipFile(zFile, src);   
	if (ret != ZIP_OK)
	{
		SU_ERROR_PRINT("ailed to write file to zip: %s", src.c_str());
	}
	return ret;
}

// 压缩文件夹
int minizip_compress_dir(const zipFile& zFile, const std::string& dirPath)
{
	std::filesystem::path dir(dirPath);
	std::string dirName = dir.filename().string();
	std::string dirParent = dir.parent_path().string();
	std::vector<std::string> vecFiles;
	EnumDirFiles(dirParent, dirName, vecFiles);  // 列举指定目录及其子目录中的所有文件和子目录
	for (const auto& filePath : vecFiles)  // 处理所有的文件
	{
		zip_fileinfo zFileInfo = { 0 };
		//int ret = zipOpenNewFileInZip64(zFile, filePath.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_BEST_COMPRESSION,1);
		int ret = zipOpenNewFileInZip3_64(zFile, filePath.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 1);
		if (ret != ZIP_OK)
		{
			std::cout << "Failed to open file in zip: " << filePath << std::endl;
			return ret;
		}
		std::filesystem::path dirPrefix = dir.parent_path();
		dirPrefix.append(filePath);   // 需压缩文件的绝对目录
		if (std::filesystem::is_regular_file(dirPrefix.string()))	// 判断是否为文件
		{
			ret = writeInZipFile(zFile, dirPrefix.string());
		}
		if (ret != ZIP_OK)
		{
			std::cout << "Failed to write file to zip: " << dirPrefix.string() << std::endl;
			return ret;
		}
	}
	return ZIP_OK;
}


/// 将文件或目录压缩为ZIP文件
int silly_minizip::compress(const std::string& s_src, const std::string& s_dst)
{
	if (!std::filesystem::exists(s_src))
	{
		return FileNotExistErr;
	}
	std::string src = s_src;
	// MARK : __linux__ 是否在linux下定义
#ifdef __linux__
	struct stat64 fileInfo;
	stat64(src.c_str(), &fileInfo);
#else
	struct _stat64 fileInfo;
	_stat64(src.c_str(), &fileInfo);
#endif
	std::string outDst = s_dst;
	if (outDst.empty())		// 压缩路径为空
	{
		std::filesystem::path srcFilePath = std::filesystem::path(src).parent_path();
		std::string onlyFileName = std::filesystem::path(src).filename().stem().string();  //不包括后缀
		onlyFileName.append(SILLY_ZIP_SUFFIX);
		srcFilePath.append(onlyFileName);
		outDst = srcFilePath.string();
	}
	if (S_ISREG(fileInfo.st_mode))  // 压缩文件
	{
		zipFile zFile = zipOpen64(outDst.c_str(), APPEND_STATUS_CREATE);
		if (zFile == NULL)
		{
			zipClose(zFile, NULL);
			return MiniZCreatZipErr;  // 创建写入的zip失败
		}
		int ret = minizip_compress_file(zFile, src);  //压缩文件
		if (ret != ZIP_OK)
		{
			zipClose(zFile, NULL);
			return MiniZOpenFileErr;
		}
		zipClose(zFile, NULL);
	}
	else if (S_ISDIR(fileInfo.st_mode))  // 压缩文件夹
	{

		zipFile zFile = zipOpen64(outDst.c_str(), APPEND_STATUS_CREATE);
		if (zFile == NULL)
		{
			return MiniZCreatZipErr;  //  创建写入的zip失败
		}
		int ret = minizip_compress_dir(zFile, src);  // 压缩文件夹
		if (ret != ZIP_OK)
		{
			zipClose(zFile, NULL);
			return ret; 
		}
		zipClose(zFile, NULL);
	}
	return Ok;
}

/// 解压zip文件,解压单独文件和目录文件
int silly_minizip::decompress(const std::string& s_src, const std::string& s_dst)
{
	if (!std::filesystem::exists(s_src))  // 解压文件不存在
	{
		return FileNotExistErr;
	}
	std::string outputDirectory = s_dst;
	if (outputDirectory.empty())         // 如果解压路径为空,创建一个和压缩包名称相同的目录,解压到该目录下
	{
		std::filesystem::path srcPath(s_src);
		std::string srcOnlyName = srcPath.filename().stem().string();
		std::filesystem::path defOutputDir = srcPath.parent_path().append(srcOnlyName);
		outputDirectory = defOutputDir.string();
	}
	// 如果数据目录为空,创建输出目录
	std::filesystem::path outputDir(outputDirectory);
	if (!std::filesystem::exists(outputDir))
	{
		if (!std::filesystem::create_directory(outputDir))
		{
			return MiniZCreatDirErr;        // 创建目录失败
		}
	}
	unzFile zipFile = unzOpen64(s_src.c_str());   	// 打开ZIP文件
	if (zipFile == nullptr)
	{
		return MiniZOpenFileErr;   // 打开ZIP文件失败
	}
	// 进入ZIP文件中的第一个文件
	if (unzGoToFirstFile(zipFile) != UNZ_OK)
	{
		unzClose(zipFile);
		return MiniZFileEmptyErr;           // ZIP文件为空
	}
	do    // 循环解压缩所有文件
	{
		size_t nameLen = 0;					// 该压缩分支文件名长度
		size_t uncompressed_size = 0;		// 该压缩分支占字节数
		unz_file_info64 file_info;			// 获取当前文件信息
		if (unzGetCurrentFileInfo64(zipFile, &file_info, nullptr, 0, nullptr, 0, nullptr, 0) == UNZ_OK)
		{
			nameLen = file_info.size_filename;
			uncompressed_size = file_info.uncompressed_size;
		}
		char* filename = (char*)malloc(nameLen + 1);
		if (unzGetCurrentFileInfo64(zipFile, &file_info, filename, nameLen + 1, nullptr, 0, nullptr, 0) != UNZ_OK)
		{
			SU_MEM_FREE(filename);
			return MiniZGetInforErr;					// 获取文件信息失败
		}
		std::filesystem::path one_file = outputDir;
		one_file.append(filename);

		if (std::filesystem::is_directory(one_file))	// 压缩分支为目录
		{
			if (!std::filesystem::exists(one_file))
			{
				if (!std::filesystem::create_directories(one_file.string()))
				{
					SU_MEM_FREE(filename);
					return MiniZCreatDirErr;			// 创建目录失败
				}
			}
		}
		else       // 压缩分支为文件
		{
			// 创建父目录
			std::filesystem::create_directories(std::filesystem::path(one_file).parent_path());
			// 写入文件
			std::ofstream outFile(one_file, std::ios::binary);
			if (!outFile.is_open())
			{
				SU_MEM_FREE(filename);
				return MiniZCreatDirErr; // 创建文件失败
			}
			else
			{
				char* buffer = (char*)malloc(uncompressed_size);
				int readSize;
				unzOpenCurrentFile(zipFile);

				while ((readSize = unzReadCurrentFile(zipFile, buffer, uncompressed_size)) > 0)
				{
					outFile.write(buffer, readSize);
				}
				outFile.close();
				SU_MEM_FREE(buffer);
			}
		}
		SU_MEM_FREE(filename);

	} while (unzGoToNextFile(zipFile) == UNZ_OK);
	unzClose(zipFile);
	return Ok;
}


int silly_minizip::compress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
{
	if (c_in_val == nullptr || i_in_len == 0)
	{
		SU_ERROR_PRINT("Empty input data.");
		return InValidInputErr;
	}

	uLong dest_len = compressBound(i_in_len);    // 解压后的数据大小
	if (!*c_out_val)
	{
		*c_out_val = (char*)malloc(dest_len);
	}
	else
	{
		SU_ERROR_PRINT("Clean output and set null.");
		return InValidOutputErr;
	}
	i_out_len = 0;
	// 压缩
	int err = compress2((Bytef*)*c_out_val, &dest_len, (Bytef*)c_in_val, uLong(i_in_len), Z_DEFAULT_COMPRESSION);
	if (err != Z_OK)
	{
		SU_MEM_FREE(*c_out_val);
		return MiniZCompressStrErr;
	}

	i_out_len = dest_len;       // 解压后的数据大小
	return Ok;
}

int silly_minizip::decompress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
{
	// 检查输入参数
	if (c_in_val == nullptr || i_in_len == 0)
	{
		SU_ERROR_PRINT("Empty input data.");
		return InValidInputErr;
	}
	if (*c_out_val)
	{
		SU_ERROR_PRINT("Clean output and set null.");
		return InValidOutputErr;
	}

	uLongf dest_len = compressBound(i_in_len);      // 解压后的数据大小
	*c_out_val = (char*)malloc(dest_len);

	i_out_len = 0;
	uLong in_len = uLong(i_in_len);
	// 解压
	int err = uncompress2((Bytef*)*c_out_val, &dest_len, (const Bytef*)c_in_val, &in_len);
	if (err != Z_OK)
	{
		SU_MEM_FREE(*c_out_val);
		return MiniZUncompressStrErr;
	}
	i_out_len = dest_len;       // 解压后的数据大小

	return Ok;
}









