/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/1 10:03
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_TEST_COMPRESS_HPP
#define SILLY_UTILS_TEST_COMPRESS_HPP
#include "files/TFF_FileUtils.h"
#include "compress/silly_bz2.h"
#include "compress/silly_minizip.h"
#include "encode/convert.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestCompress)


BOOST_AUTO_TEST_CASE(MINIZIP_DECOMPRESS_DIR)      // minizip压缩文件夹
{
	std::cout << "\r\n\r\n****************" << "MINIZIP_DECOMPRESS_DIR" << "****************" << std::endl;
	std::filesystem::path src_file(DEFAULT_DATA_DIR);
	src_file += "/compress/456";   //123.txt
	std::filesystem::path des_file(DEFAULT_DATA_DIR);
	des_file += "/compress/123.zip";
	int result = silly_minizip::decompressZip(des_file.string(), src_file.string());
	if (result == 0)
	{
		std::cout << "Compression completed successfully." << std::endl;
	}
	else
	{
		std::cout << "Compression failed." << std::endl;
	}

};



BOOST_AUTO_TEST_CASE(MINIZIP_COMPRESS_DIR)      // minizip压缩文件夹
{
	std::cout << "\r\n\r\n****************" << "MINIZIP_COMPRESS_DIR" << "****************" << std::endl;
	std::filesystem::path src_file(DEFAULT_DATA_DIR);
	src_file += "/compress/123.txt";   //123.txt
	std::filesystem::path des_file(DEFAULT_DATA_DIR);
	des_file += "/compress/123.zip";
	int result =0;
	//result = silly_minizip::compressZip(src_file.string().c_str(), des_file.string().c_str());
	if (result == 0) 
	{
		std::cout << "Compression completed successfully." << std::endl;
	}
	else 
	{
		std::cout << "Compression failed." << std::endl;
	}




	//silly_minizip::compressu(silly_conv::UTF82GBK(R"(E:/dly/workspace/文档)"), silly_conv::UTF82GBK(R"(E:/dly/workspace/文档.zip)"));
};



BOOST_AUTO_TEST_CASE(BZ2_COMPRESS_FILE)      // BZ2压缩文件
{
	std::cout << "\r\n\r\n****************" << "BZ2_COMPRESS_FILE" << "****************" << std::endl;
	std::filesystem::path to_compress_file(DEFAULT_DATA_DIR);
	to_compress_file.append("hebei_station.txt");
	//std::string dst = silly_bz2::compress(to_compress_file.string());
	//BOOST_CHECK(!dst.empty());
	//std::cout << "Compress to " << dst << std::endl;
};

BOOST_AUTO_TEST_CASE(BZ2_COMPRESS_DATA)      // BZ2压缩数据
{
	std::cout << "\r\n\r\n****************" << "BZ2_COMPRESS_DATA" << "****************" << std::endl;
	std::filesystem::path to_compress_file(DEFAULT_DATA_DIR);
	to_compress_file.append("hebei_station.txt");
	//char* data = nullptr;
	//int len = 0;
	//FileUtils::ReadAll(to_compress_file.string(), &data, len);
	//if (data && len)
	//{
	//	char* dst = nullptr;
	//	unsigned int dstl = 0;
	//	if (silly_bz2::compress(data, len, &dst, &dstl))
	//	{
	//		std::cout << "Compress data from size " << len << " to size " << (int)dstl << std::endl;
	//	}
	//	else
	//	{
	//		std::cerr << "Compress failed." << std::endl;
	//	}
	//	if (dst) free(dst);
	//}

};

BOOST_AUTO_TEST_CASE(BZ2_DECOMPRESS_FILE)      // BZ2解压缩文件
{
	std::cout << "\r\n\r\n****************" << "BZ2_DECOMPRESS_FILE" << "****************" << std::endl;
	std::filesystem::path upoad_file(DEFAULT_DATA_DIR);
	//upoad_file.append("compress").append("R_RADR_I_S2400_20230721115441_O_DOR_YLD4-D_CAP_FMT.bin.bz2");

	
};

BOOST_AUTO_TEST_CASE(BZ2_DECOMPRESS_DATA)      // BZ2解压缩数据
{
	std::cout << "\r\n\r\n****************" << "BZ2_DECOMPRESS_DATA" << "****************" << std::endl;
};



BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_COMPRESS_HPP
