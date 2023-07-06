#pragma once
// #define LIBZIPPP_WITH_ENCRYPTION
#include "encode_iconv.hpp"
#include <libzippp/libzippp.h>

#include <filesystem>
#include <vector>
#include <map>
#include <iostream>


using namespace libzippp;
class TzxZipPPUtils
{
/// <summary>
///		1.目前测试支持 zip, rar, tar.gz, 7z, 单纯的文件后缀名不同,里面的压缩算法应该是一样的.
///		2. FIXIT: 解压文件在windows平台下没有什么问题, centos平台下会另外产生一个乱码的文件夹, 需要修复.
/// 	3. FIXIT: 调用addFile时,需要转UTF8, windows平台测试后的猜测是,跟系统编码有关, 与项目编码无关, 需要在Ubuntu和Centos上继续测试.
/// </summary>
public:
	/// <summary>
	/// 压缩多个(不同位置的)文档到同一个压缩文件
	/// </summary>
	/// <param name="archivePath">压缩文件名称(输出)</param>
	/// <param name="errMsg">错误信息</param>
	/// <param name="filePaths">被压缩的文件列表(输入)</param>
	/// <param name="password">密码, 默认没有</param>
	/// <param name="encryptionMethod">编码方式, 默认没有</param>
	/// <returns>是否成功, false 可以通过errMsg查看具体错误信息.</returns>
	static bool Compress(const std::string& archivePath, std::string& errMsg, const std::vector<std::string>& filePaths, const std::string& password = "", ZipArchive::Encryption encryptionMethod = ZipArchive::Encryption::None)
	{
		errMsg = "";

		ZipArchive zf(archivePath, password, encryptionMethod);
		zf.open(ZipArchive::New);
		if (!zf.isOpen())
		{
			errMsg = "Open archive: " + archivePath + " failed.";
			zf.unlink();
			return false;
		}
		for (auto& file : filePaths)
		{
			std::filesystem::path tp(file);
			if (!std::filesystem::exists(tp))
			{
				errMsg = "Source file: " + file + " not exists.";
				zf.close();
				zf.unlink();
				return false;
			}
			if (zf.hasEntry(tp.filename().string()))
			{
				errMsg = "Repeat file: " + file + ".";
				zf.close();
				zf.unlink();
				return false;
			}
			zf.addFile(GBK2UTF8(tp.filename().string().c_str()), GBK2UTF8(file.c_str()));
		}
		zf.close();
		return true;
	}

	/// <summary>
	/// 将一个文件夹下的所有内容压缩到一个文件中
	/// </summary>
	/// <param name="archivePath">压缩文件名称(输出)</param>
	/// <param name="errMsg">错误信息</param>
	/// <param name="dir">被压缩的文件夹(输入)</param>
	/// <param name="password">密码, 默认没有</param>
	/// <param name="encryptionMethod">编码方式, 默认没有</param>
	/// <returns>是否成功, false 可以通过errMsg查看具体错误信息.</returns>
	static bool Compress(const std::string& archivePath, std::string& errMsg, const std::string& dir, const std::string& password = "", ZipArchive::Encryption encryptionMethod = ZipArchive::Encryption::None)
	{
		errMsg = "";

		ZipArchive zf(archivePath, password, encryptionMethod);
		zf.open(ZipArchive::New);
		if (!zf.isOpen())
		{
			errMsg = "Open archive: " + archivePath + " failed.";
			zf.unlink();
			return false;
		}
		std::map<std::string, std::vector<std::string>> arc2files;
		RecursiveAddFile(dir, arc2files);
		for (auto [entry, files] : arc2files)
		{
			zf.addEntry(entry);
			for (auto& file : files)
			{
				std::filesystem::path fp(file);
				std::string entryName = entry + fp.filename().string();
				zf.addFile(GBK2UTF8(entryName.c_str()), GBK2UTF8(file.c_str()));
			}
		}
		zf.close();
		return true;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="archivePath">压缩文件名称(输入)</param>
	/// <param name="errMsg">错误信息</param>
	/// <param name="createDir">true: 以压缩文件名创建文件夹,将文件解压到该文件夹下; false: 直接将文件解压到 dirPath 下</param>
	/// <param name="dirPath">指定解压位置,默认为当前文件夹(输出)</param>
	/// <param name="password">密码, 默认没有</param>
	/// <param name="encryptionMethod">编码方式, 默认没有</param>
	/// <returns>是否成功, false 可以通过errMsg查看具体错误信息.</returns>	
	static bool DeCompress(const std::string& archivePath, std::string& errMsg, const bool& createDir = true, const std::string& dirPath = "", const std::string& password = "", ZipArchive::Encryption encryptionMethod = ZipArchive::Encryption::None)
	{
		ZipArchive zf(archivePath);
		zf.open(ZipArchive::ReadOnly);

		std::vector<ZipEntry> entries = zf.getEntries();
		std::vector<ZipEntry>::iterator it;
		for (it = entries.begin(); it != entries.end(); ++it) {
			ZipEntry entry = *it;
			std::string name = entry.getName();
			std::cout << name << std::endl;
			int size = entry.getSize();
			std::cout << size << std::endl;

			//the length of binaryData will be size
			void* binaryData = entry.readAsBinary();

			//the length of textData will be size
			std::string textData = entry.readAsText();

			//...
		}

		zf.close();
		return false;
	}
private:

	/// <summary>
	/// 遍历文件夹,并且构建archive的文件路径
	/// </summary>
	/// <param name="path"></param>
	/// <param name="arc2files"></param>
	/// <param name="entry"></param>
	static void RecursiveAddFile(const std::string& path, std::map<std::string, std::vector<std::string>>& arc2files, const std::string& entry = "")
	{
		std::filesystem::path dir(path);
		for (const auto& fileIter : std::filesystem::directory_iterator(dir))
		{
			std::cout << fileIter.path().string() << std::endl;
			if (std::filesystem::is_directory(fileIter.path()))
			{
				std::string new_entry = entry;
				new_entry.append(fileIter.path().filename().string());
				new_entry.append("/");
				RecursiveAddFile(fileIter.path().string(), arc2files, new_entry);
			}
			else
			{
				arc2files[entry].push_back(fileIter.path().string());
			}
		}
	}
};

