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

/// <summary>
/// 将一个文件内容写入到一个 ZIP 文件中
/// </summary>
/// <param name="zFile"></param>
/// <param name="file">需要读取的文件</param>
/// <returns></returns>
int writeInZipFile(zipFile zFile, const std::string& file)
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

// TODO: 王英杰, 压缩文件和压缩目录分开 
// 解压同样, 解压考虑解压到当前目录 和 解压到指定目录应该是什么样子的
// 错误信息命名方式参考Z_OK附近的定义

// *c_out_val = new char[dest_len]; 统一使用malloc
// *c_out_val 初始化前判断释放为空

int minizip_compress_file();
int minizip_compress_dir();




/// 将文件或目录压缩为ZIP文件
int silly_minizip::compress(const std::string& s_src, const std::string& s_dst)
{
	if (!std::filesystem::exists(s_src))
	{
		return FileNotExistErr;
	}
	std::string src = s_src;
	
	// MARK 替换为 '/' == src[src.length() - 1]
	if (src.find_last_of("/") == src.length() - 1)  //如果是目录去掉目录最后的/
	{
		src = src.substr(0, src.length() - 1);
	}
	// MARK : __linux__ 是否在linux下定义
#ifdef __linux__
	struct stat64 fileInfo;
	stat64(src.c_str(), &fileInfo);
#else
	struct _stat64 fileInfo;
	_stat64(src.c_str(), &fileInfo);
#endif

	if (S_ISREG(fileInfo.st_mode))  // 压缩文件
	{
		size_t pos = src.find_last_of("/");
		std::string srcFireName = src.substr(pos + 1); // 目录名(需要压缩的目录)
		zipFile zFile = zipOpen(s_dst.c_str(), APPEND_STATUS_CREATE);
		if (zFile == NULL)
		{
			return MiniZCreatZipErr;  //创建写入的zip失败
		}
		zip_fileinfo zFileInfo = { 0 };
		int ret = zipOpenNewFileInZip(zFile, srcFireName.c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
		if (ret != ZIP_OK)
		{
			zipClose(zFile, NULL);
			return MiniZOpenFileErr;  // 打开待压缩文件失败
		}
		ret = writeInZipFile(zFile, src);
		if (ret != ZIP_OK)
		{
			zipClose(zFile, NULL);
			return MiniZCreatZipErr;  // 打开新写入的ZIP失败
		}
		zipClose(zFile, NULL);
	}
	else if (S_ISDIR(fileInfo.st_mode))  // 压缩目录
	{
		size_t pos = src.find_last_of("/");
		std::string dirName = src.substr(pos + 1);      // 目录名(需要压缩的目录)
		std::string dirPrefix = src.substr(0, pos);     // 需要压缩目录所在目录的绝对路径
		zipFile zFile = zipOpen(s_dst.c_str(), APPEND_STATUS_CREATE);
		if (zFile == NULL)
		{
			return MiniZCreatZipErr;  //打开要写入的zip失败
		}
		std::vector<std::string> vecFiles;
		EnumDirFiles(dirPrefix, dirName, vecFiles);  // 列举指定目录及其子目录中的所有文件和子目录
		std::vector<std::string>::iterator itF = vecFiles.begin();
		for (; itF != vecFiles.end(); ++itF)
		{
			zip_fileinfo zFileInfo = { 0 };
			int ret = zipOpenNewFileInZip(zFile, itF->c_str(), &zFileInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
			if (ret != ZIP_OK)
			{
				zipClose(zFile, NULL);
				return MiniZOpenFileErr;  // 打开待压缩文件失败
			}
			std::string doc_file = (dirPrefix + "/" + *itF);
			if (std::filesystem::is_regular_file(doc_file))   // 判断是否为文件
			{
				ret = writeInZipFile(zFile, doc_file);
			}
			if (ret != ZIP_OK)
			{
				zipClose(zFile, NULL);
				return MiniZCreatZipErr;  // 打开新写入的ZIP失败
			}
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
	if (outputDirectory.empty())         // 如果解压路径为空,创建默认解压目录,默认为待解压文件所在目录
	{
		std::filesystem::path defOutputDir = std::filesystem::path(s_src).parent_path();
		outputDirectory = defOutputDir.string();
	}
	// 打开ZIP文件
	unzFile zipFile = unzOpen(s_src.c_str());
	if (zipFile == nullptr)
	{
		return MiniZOpenFileErr;   // 打开ZIP文件失败
	}
	// 如果数据目录为空,创建输出目录
	std::filesystem::path outputDir(outputDirectory);
	if (!std::filesystem::exists(outputDir))
	{
		if (!std::filesystem::create_directory(outputDir))
		{
			unzClose(zipFile);
			return MiniZCreatDirErr;        // 创建目录失败
		}
	}
	// 进入ZIP文件中的第一个文件
	if (unzGoToFirstFile(zipFile) != UNZ_OK)
	{
		unzClose(zipFile);
		return MiniZFileEmptyErr;           // ZIP文件为空
	}
	do    // 循环解压缩所有文件
	{
		char filename[512];
		unz_file_info file_info;
		// 获取当前文件信息
		if (unzGetCurrentFileInfo(zipFile, &file_info, filename, sizeof(filename), nullptr, 0, nullptr, 0) != UNZ_OK)
		{
			return MiniZGetInforErr;    // 获取文件信息失败
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
				return MiniZCreatDirErr; // 创建文件失败
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
	return Ok;
}


int silly_minizip::compress(const char* c_in_val, const size_t& i_in_len, char** c_out_val, size_t& i_out_len)
{
	if (c_in_val == nullptr || i_in_len == 0)
	{
		return InValidInputErr;
	}
	// 这里对i_in_len + 1是要考虑字符串结尾的'\0',在strlen(char*)计算char*的长度是不会计算结尾的'\0',在这里要+1
	// MARK 如果不是字符串????
	size_t len = i_in_len + 1;

	uLong dest_len = compressBound(len);    // 解压后的数据大小
	*c_out_val = new char[dest_len];
	// 压缩
	//int err = compress2(reinterpret_cast<Bytef*>(*c_out_val), &dest_len, reinterpret_cast<const Bytef*>(c_in_val), uLong(len), Z_DEFAULT_COMPRESSION);
	int err = compress2((Bytef*)*c_out_val, &dest_len, (Bytef*)c_in_val, uLong(len), Z_DEFAULT_COMPRESSION);
	if (err != Z_OK)
	{
		delete[] * c_out_val;
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
		return InValidInputErr;
	}

	*c_out_val = nullptr;
	i_out_len = 0;

	uLongf dest_len = compressBound(i_in_len);      // 解压后的数据大小
	*c_out_val = new char[dest_len];
	uLong in_len = uLong(i_in_len);
	// 解压
	//int err = uncompress2(reinterpret_cast<Bytef*>(*c_out_val), &dest_len, reinterpret_cast<const Bytef*>(c_in_val), &in_len);
	int err = uncompress2((Bytef*)*c_out_val, &dest_len, (const Bytef*)c_in_val, &in_len);
	if (err != Z_OK)
	{
		delete[] * c_out_val;
		return MiniZUncompressStrErr;
	}

	i_out_len = dest_len;       // 解压后的数据大小
	return Ok;
}









