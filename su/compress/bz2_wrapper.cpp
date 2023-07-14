//
// Created by dell on 2023/7/10.
//

#include "bz2_wrapper.h"
#include <bzlib.h>
#include <filesystem>
#include <cstring>

#define BZ2_SUFFIX    ".bz2"
// 1024*1024*512
#define BZ2_DECOMPRESS_DEFAULT_SIZE        536870912
// /* recommended size */
#define BZ2_SUGGEST_COMPRESS_SIZE(inlen) ((unsigned int)(inlen * 1.1 + 600))

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
	rf = fopen(src_file.c_str(), "rb");
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
	unsigned int dst_len = BZ2_SUGGEST_COMPRESS_SIZE(file_len);   /* recommended size */

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
		printf("File: %s, line: %d. Error: compression failed: %d", __FILE__, __LINE__, rc);
		free(compressed);
		free(file_content);
		return "";
	}

	FILE* wf = nullptr;
	wf = fopen(ret_dst_file.c_str(), "wb");
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
		printf("File: %s, line: %d. Error: invalid bz2 extension: %s.\n", __FILE__, __LINE__, src_path.string().c_str());
		return ret_dst_file;
	}

	if (ret_dst_file.empty())
	{
		ret_dst_file = src_path.string().substr(0, src_path.string().size() - strlen(BZ2_SUFFIX));
	}

	char* file_content = nullptr;
	int file_len = 0;
	FILE* pf = nullptr;
	pf = fopen(src_file.c_str(), "rb");
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
	unsigned int dst_len = BZ2_DECOMPRESS_DEFAULT_SIZE;
	char* decompress = (char*)malloc(dst_len);      //接收解压缩的数据。解压缩需要的内存大小事先是不知道的。
	int rc = BZ2_bzBuffToBuffDecompress(decompress, &dst_len, file_content, file_len, 0, 0);
	while (BZ_OUTBUFF_FULL == rc)	// 预设解压空间较小,需要扩大,其他情况再看如何解决
	{
		dst_len += BZ2_DECOMPRESS_DEFAULT_SIZE;
		if (decompress)
		{ free(decompress); }
		decompress = (char*)malloc(dst_len);
		rc = BZ2_bzBuffToBuffDecompress(decompress, &dst_len, file_content, file_len, 0, 0);
	}
	if (BZ_OK != rc)
	{
		printf("File: %s, line: %d. Error: decompression failed: %d", __FILE__, __LINE__, rc);
		free(file_content);
		free(decompress);
		return "";
	}
	FILE* wf = nullptr;
	wf = fopen(ret_dst_file.c_str(), "wb");
	if (wf)
	{
		fwrite(decompress, 1, dst_len, wf);
		fclose(wf);
	}
	else
	{
		free(file_content);
		free(decompress);
		return "";
	}
	free(file_content);
	free(decompress);


	return ret_dst_file;
}

bool bz2_wrapper::compress(const char* inval, const unsigned int& inlen, char** outval, unsigned int* outlen)
{
	if (!inval || ! inlen)
	{
		printf("File: %s, line: %d. Error: empty in value.\n", __FILE__, __LINE__);
		return false;
	}
	unsigned int dst_len = BZ2_SUGGEST_COMPRESS_SIZE(inlen);
	char* c_inval = (char*)inval;
	if (!*outval)
	{
		*outval = (char*)malloc(dst_len);
	}
	else
	{
		printf("File: %s, line: %d. Error: clean outval and set null.\n", __FILE__, __LINE__);
		return false;
	}
	int rc = BZ2_bzBuffToBuffCompress(*outval, &dst_len, c_inval, inlen, 1, 0, 30);
	if (rc != BZ_OK)
	{
		printf("File: %s, line: %d. Error: compression failed: %d.\n", __FILE__, __LINE__, rc);

		return false;
	}
	return true;
}

bool bz2_wrapper::decompress(const char* inval, const unsigned int& inlen, char** outval, unsigned int* outlen)
{
	if (!inval || ! inlen)
	{
		printf("File: %s, line: %d. Error: empty in value.\n", __FILE__, __LINE__);
		return false;
	}
	unsigned int dst_len = BZ2_DECOMPRESS_DEFAULT_SIZE;
	*outlen = 0;
	char* c_inval = (char*)inval;
	if (!*outval)
	{
		*outval = (char*)malloc(dst_len);
	}
	else
	{
		printf("File: %s, line: %d. Error: clean outval and set null.\n", __FILE__, __LINE__);
		return false;
	}
	char* decompress = (char*)malloc(dst_len);      //接收解压缩的数据。解压缩需要的内存大小事先是不知道的。
	int rc = BZ2_bzBuffToBuffDecompress(decompress, &dst_len, c_inval, inlen, 0, 0);
	while (BZ_OUTBUFF_FULL == rc)	// 预设解压空间较小,需要扩大,其他情况再看如何解决
	{
		dst_len += BZ2_DECOMPRESS_DEFAULT_SIZE;
		if (decompress)
		{ free(decompress); }
		decompress = (char*)malloc(dst_len);
		rc = BZ2_bzBuffToBuffDecompress(decompress, &dst_len, c_inval, inlen, 0, 0);
	}
	if (BZ_OK != rc)
	{
		printf("File: %s, line: %d. Error: decompression failed: %d.\n", __FILE__, __LINE__, rc);
		if (decompress)
		{ free(decompress); decompress= nullptr;}
		return false;
	}
	*outlen = dst_len;
	return true;
}
