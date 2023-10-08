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

#ifndef SILLY_UTILS_TEST_TCP_HPP
#define SILLY_UTILS_TEST_TCP_HPP

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>

#include "tcp/ftp_utils.h"
#include "tcp/silly_ftp.h"
#include "files/silly_file.h"

BOOST_AUTO_TEST_SUITE(TestTCP)


BOOST_AUTO_TEST_CASE(FTP_UPLOATE)      // FTP�ϴ�
{
	std::cout << "\r\n\r\n****************" << "FTP_UPLOATE" << "****************" << std::endl;
	std::filesystem::path upoad_file(DEFAULT_DATA_DIR);
	upoad_file.append("hebei_station.txt");
	BOOST_CHECK(ftp_utils::ftp_upload(upoad_file.string(), "ftp://192.168.0.202:21/240/hebei_station.txt", "radar", "3edc9ijn", 10, false));
};

BOOST_AUTO_TEST_CASE(SILLY_FTP_UPLOATE) // silly_ftp �ϴ�
{
	//std::cout << "\r\n\r\n****************" << "FTP_UPLOATE" << "****************" << std::endl;
	//std::filesystem::path upoad_file(DEFAULT_DATA_DIR);
	//upoad_file.append("hebei_station.txt");

	//silly_ftp* ftp = new silly_ftp();
	//int ret = ftp->Connect("192.168.0.202:21");
	//ret = ftp->Login("radar", "3edc9ijn");
	//// ret = ftp->Get("text.txt", "text.txt", silly_ftp::image);
	//ret = ftp->Put(upoad_file.string().c_str(), "ftp://192.168.0.202:21/240/aa.txt", silly_ftp::ascii);
	//ftp->Quit();
};

BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_TCP_HPP
