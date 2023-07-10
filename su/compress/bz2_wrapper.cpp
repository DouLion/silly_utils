//
// Created by dell on 2023/7/10.
//

#include "bz2_wrapper.h"
#include <bzlib.h>
#include <filesystem>

#define BZ2_SUFFIX    ".bz2"

std::string bz2_wrapper::compress(const std::string& src_file, const std::string& dst_file)
{
	std::string ret_dst_file = dst_file;
	std::filesystem::path src_path(src_file);
	if (!std::filesystem::exists(src_path))
	{
		return ret_dst_file;
	}

	if (ret_dst_file.empty())
	{
		ret_dst_file = src_path.string().append(BZ2_SUFFIX);
	}

	char* file_content = nullptr;
	size_t file_len = 0;
	FILE* rf = nullptr;
	errno_t err = fopen_s(&rf, src_file.c_str(), "rb");
	if (rf && !err)    // 读取文件内容
	{
		char buffer[1024 * 10] = { 0 };
		size_t cur = fread(buffer, 1, sizeof(buffer), rf);
		size_t pos = 0;
		size_t size = cur;
		while (cur > 0)
		{
			if (nullptr == file_content)
			{
				file_content = (char*)malloc(size);
			}
			else
			{
				file_content = (char*)realloc(file_content, size);
			}
			//fileContent.resize(fileContent.size() + cur);
			memcpy(&(file_content)[pos], buffer, cur);
			pos += cur;
			memset(buffer, 0, sizeof(buffer));
			cur = fread(buffer, 1, sizeof(buffer), rf);
			size += cur;
		}
		file_len = size;

		fclose(rf);
	}

	// 调用bz2进行压缩
	unsigned int dst_len = (unsigned int)(file_len * 1.1 + 600);   /* recommended size */

	char* compressed = (char*)malloc(dst_len);
/*	err = BZ2_bzBuffToBuffCompress(
			dest,       //目标buffer（压缩后的数据）
			&destLen,   //目标buffer长度。函数内部压缩完之后，会设置为实际的长度。
			content,     //原始数据
			len,        //原始数据长度
			1,          //blockSize100k 字典大小，范围1-9，实际尺寸是100k*N
			0,          //verbosity 控制台输出压缩信息，范围0-4，数字越小输出越少
			30          //默认值30，库内部排序算法相关
	);*/
	int rc = BZ2_bzBuffToBuffCompress(compressed, &dst_len, file_content, file_len, 1, 0, 30);

	if (rc != BZ_OK)
	{
		printf_s("File: %s, line: %d. Error: compression failed: %d", __FILE__, __LINE__, rc);
		free(compressed);
		free(file_content);
		return "";
	}

	FILE* wf = nullptr;
	fopen_s(&wf, ret_dst_file.c_str(), "wb");
	if (wf)
	{
		fwrite(compressed, 1, dst_len, wf);
		fclose(wf);
	}
	else
	{
		free(file_content);
		free(compressed);
		return "";
	}
	free(file_content);
	free(compressed);
	return ret_dst_file;
}

std::string bz2_wrapper::decompress(const std::string& src_file, const std::string& dst_file)
{
	std::string ret_dst_file = dst_file;
	std::filesystem::path src_path(src_file);
	if (!std::filesystem::exists(src_path))
	{
		return ret_dst_file;
	}
	// TODO: 这里要区分大小写
	if (src_path.extension().string() != BZ2_SUFFIX)// 判断文件扩展名是否正确
	{
		printf_s("File: %s, line: %d. Error: invalid bz2 extension: %s.\n", __FILE__, __LINE__, src_path.string().c_str());
		return ret_dst_file;
	}

	if (ret_dst_file.empty())
	{
		ret_dst_file = src_path.string().substr(0, src_path.string().size() - strlen(BZ2_SUFFIX));
	}

	char* file_content = nullptr;
	int file_len = 0;
	FILE* pf = nullptr;
	errno_t err = fopen_s(&pf, src_file.c_str(), "rb");
	if (pf && !err)    // 读取文件内容
	{
		char buffer[1024 * 10] = { 0 };
		size_t cur = fread(buffer, 1, sizeof(buffer), pf);
		size_t pos = 0;
		size_t size = cur;
		while (cur > 0)
		{
			if (nullptr == file_content)
			{
				file_content = (char*)malloc(size);
			}
			else
			{
				file_content = (char*)realloc(file_content, size);
			}
			//fileContent.resize(fileContent.size() + cur);
			memcpy(&(file_content)[pos], buffer, cur);
			pos += cur;
			memset(buffer, 0, sizeof(buffer));
			cur = fread(buffer, 1, sizeof(buffer), pf);
			size += cur;
		}
		file_len = (int)size;

		fclose(pf);
	}

	// 调用bz2进行解压
	unsigned int dst_len = 1024*1024*1024;
	char* decompress = (char*)malloc(dst_len);      //接收解压缩的数据。解压缩需要的内存大小事先是不知道的。
	int rc = BZ2_bzBuffToBuffDecompress(decompress, &dst_len, file_content, file_len, 0, 0);
	if (BZ_OK != rc)
	{
		printf_s("File: %s, line: %d. Error: decompression failed: %d", __FILE__, __LINE__, rc);
		free(file_content);
		free(decompress);
		return "";
	}
	FILE* wf = nullptr;
	fopen_s(&wf, ret_dst_file.c_str(), "wb");
	if (wf)
	{
		fwrite(decompress, 1, dst_len, wf);
		fclose(wf);
	}
	else
	{
		return "";
	}
	free(file_content);
	free(decompress);


	return ret_dst_file;
}
