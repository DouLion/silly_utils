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

#if I_NEED_TEST
#include <catch2/catch_test_macros.hpp>
#include "files/silly_file.h"
#include "compress/silly_bz2.h"
#include "compress/silly_minizip.h"
TEST_CASE("TestCompress")
{




SECTION("BIG_FILE_DECOMPRESS")  // 大文件解压缩
{
    std::cout << "\r\n\r\n****************"
              << "BIG_FILE_DECOMPRESS"
              << "****************" << std::endl;

    std::string g4_5 = "D:/1_wangyingjie/readfile/cpmpress/big4_5g.zip";
    std::string g4_5_out = "D:/1_wangyingjie/readfile/cpmpress/txt/";

    std::string g4_5_dir = "D:/1_wangyingjie/readfile/Radar_LOCAL/HN4_cpm";
    std::string g4_5_out_dir = "D:/1_wangyingjie/readfile/Radar_LOCAL/HN4.zip";

   silly_compress::CPS_ERR result1 = silly_minizip::decompress(g4_5_out_dir, g4_5_dir);  // 压缩
    if (result1 == silly_compress::CPS_ERR::Ok)
    {
        std::cout << "Compression completed successfully." << std::endl;
    }
    else
    {
        std::cout << "Compression failed." << std::endl;
    }

    int r = 0;
    int e = 0;
    int f = 0;
};

SECTION("BIG_FILE_COMRESS")  // 大文件压缩
{
    std::cout << "\r\n\r\n****************"
              << "BIG_FILE_COMRESS"
              << "****************" << std::endl;

     std::string g4_5 = "D:/1_wangyingjie/readfile/cpmpress/big4_5g.txt";
     std::string g4_5_out = "D:/1_wangyingjie/readfile/cpmpress/big4_5g.zip";
     silly_compress::CPS_ERR result1 = silly_minizip::compress(g4_5, g4_5_out);  // 压缩
     if (result1 == silly_compress::CPS_ERR::Ok)
     {
         std::cout << "Compression completed successfully." << std::endl;
     }
     else
     {
         std::cout << "Compression failed." << std::endl;
     }



    std::string g4_5_dir = "D:/1_wangyingjie/readfile/Radar_LOCAL/HN4";
    std::string g4_5_out_dir = "D:/1_wangyingjie/readfile/Radar_LOCAL/HN4_1.zip";


    silly_compress::CPS_ERR result = silly_minizip::compress(g4_5_dir, g4_5_out_dir);  // 压缩

    if (result == silly_compress::CPS_ERR::Ok)
    {
        std::cout << "Compression completed successfully." << std::endl;
    }
    else
    {
        std::cout << "Compression failed." << std::endl;
    }

    int r = 0;
    int e = 0;
    int f = 0;
};



SECTION("MINIZIP_INHERIT")  // 修改继承
{
    std::cout << "\r\n\r\n****************"
              << "MINIZIP_INHERIT"
              << "****************" << std::endl;

    // 解压字符串
    const char* input_data = "This is a test char";
    size_t input_length = strlen(input_data) + 10;

    std::string input = "This is a test char";
    size_t len = input.length() + 1;

    char* compressed_data = nullptr;
    size_t compressed_length;

    silly_minizip mzip;
    //silly_compress::CPS_ERR com = mzip.compress(input_data, input_length, &compressed_data, compressed_length);

    //char* decompressed_data = nullptr;
    //size_t decompressed_length;

    //silly_compress::CPS_ERR decom = mzip.decompress(compressed_data, compressed_length, &decompressed_data, decompressed_length);
    //-------------------------------------------------

    // 解压文件
    std::filesystem::path src_file(DEFAULT_SU_DATA_DIR);
    src_file += "/compress/123.txt";  // 压缩文件
    std::filesystem::path des_file(DEFAULT_SU_DATA_DIR);
    des_file += "/compress/123_file.zip";

    std::filesystem::path src_dir(DEFAULT_SU_DATA_DIR);
    src_dir += "/compress/compress_dir";  // 压缩文件夹
    std::filesystem::path des_dir(DEFAULT_SU_DATA_DIR);
    des_dir += "/compress/copm_dir.zip";

    // boost::timer tm1; // 定义后计时开始
    // tm1.restart();  // 从新从这里开始计时
    // std::cout << tm1.elapsed() << std::endl;  // 单位是秒

    // silly_minizip mzip;
    silly_compress::CPS_ERR result = mzip.compress(src_file.string().c_str(), des_file.string().c_str());  // 压缩
    if (result == silly_compress::CPS_ERR::Ok)
    {
        std::cout << "Compression completed successfully." << std::endl;
    }
    else
    {
        std::cout << "Compression failed." << std::endl;
    }
    std::filesystem::path un_dir(DEFAULT_SU_DATA_DIR);
    un_dir += "/compress/un";
    std::string dir = "";
    silly_compress::CPS_ERR de = mzip.decompress(des_dir.string(), un_dir.string());

    int r = 0;
    int e = 0;
    int f = 0;
};

SECTION("MINIZIP_DECOMPRESS_DIR")  // minizip解压缩文件夹
{
    std::cout << "\r\n\r\n****************"
              << "MINIZIP_DECOMPRESS_DIR"
              << "****************" << std::endl;
    std::filesystem::path des_file(DEFAULT_SU_DATA_DIR);
    des_file += "/compress/压缩2.zip";
    std::filesystem::path src_file(DEFAULT_SU_DATA_DIR);
    src_file += "/compress/789";  // 123.txt
    // int result = silly_minizip::decompressZip(des_file.string(), src_file.string());
    // if (result == 0)
    //{
    //	std::cout << "decompression completed successfully." << std::endl;
    // }
    // else
    //{
    //	std::cout << "decompression failed." << std::endl;
    // }
};


SECTION("MINIZIP_DATA")  // 压缩解压缩字符串数据
{
    std::cout << "\r\n\r\n****************"
              << "MINIZIP_DATA"
              << "****************" << std::endl;

    // 解压字符串
    const char* input_data = "This is a test char aaaa ";
    size_t input_length = strlen(input_data);

    std::string input = "This is a test char";
    size_t len = input.length() + 1;

    char* compressed_data = nullptr;
    size_t compressed_length;

    silly_minizip mzip;
    silly_compress::CPS_ERR com = mzip.compress(input_data, input_length, &compressed_data, compressed_length);

    std::cout << "compressed_data:" << compressed_data << std::endl;
    std::cout << "compressed_length:" << compressed_length << std::endl;

    char* decompressed_data = nullptr;
    size_t decompressed_length;

    silly_compress::CPS_ERR decom = mzip.decompress(compressed_data, compressed_length, &decompressed_data, decompressed_length);

    std::cout << "decompressed_data:" << decompressed_data << std::endl;
    std::cout << "decompressed_length:" << decompressed_length << std::endl;
    //-------------------------------------------------

    int r = 0;
    int e = 0;
    int f = 0;
};

SECTION("BZ2_COMPRESS_FILE")  // BZ2压缩文件
{
    std::cout << "\r\n\r\n****************"
              << "BZ2_COMPRESS_FILE"
              << "****************" << std::endl;
    std::filesystem::path to_compress_file(DEFAULT_SU_DATA_DIR);
    to_compress_file.append("hebei_station.txt");
    // std::string dst = silly_bz2::compress(to_compress_file.string());
    // BOOST_CHECK(!dst.empty());
    // std::cout << "Compress to " << dst << std::endl;
};

SECTION("BZ2_COMPRESS_DATA")  // BZ2压缩数据
{
    std::cout << "\r\n\r\n****************"
              << "BZ2_COMPRESS_DATA"
              << "****************" << std::endl;
    std::filesystem::path to_compress_file(DEFAULT_SU_DATA_DIR);
    to_compress_file.append("hebei_station.txt");
    // char* data = nullptr;
    // int len = 0;
    // FileUtils::ReadAll(to_compress_file.string(), &data, len);
    // if (data && len)
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
    // }
};

SECTION("BZ2_DECOMPRESS_FILE")  // BZ2解压缩文件
{
    std::cout << "\r\n\r\n****************"
              << "BZ2_DECOMPRESS_FILE"
              << "****************" << std::endl;
    std::filesystem::path upoad_file(DEFAULT_SU_DATA_DIR);
    // upoad_file.append("compress").append("R_RADR_I_S2400_20230721115441_O_DOR_YLD4-D_CAP_FMT.bin.bz2");
};

SECTION("BZ2_DECOMPRESS_DATA")  // BZ2解压缩数据
{
    std::cout << "\r\n\r\n****************"
              << "BZ2_DECOMPRESS_DATA"
              << "****************" << std::endl;
};

}


#endif  // SILLY_UTILS_TEST_COMPRESS_HPP
