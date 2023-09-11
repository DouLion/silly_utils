/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 18:06
 * @version: 1.0.1
 * @software: silly_utils
 * @description: �ļ����������ܲ���
 */
#pragma once

#ifndef SILLY_UTILS_TEST_FILE_HPP
#define SILLY_UTILS_TEST_FILE_HPP

#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>
#include "files/silly_file.h"
#include "files/silly_mmap.h"

BOOST_AUTO_TEST_SUITE(TestFiles)

BOOST_AUTO_TEST_CASE(MMAP_READ)		// mmap�ļ���ȡ
{
	std::cout << "\r\n\r\n****************" << "MMAP_READ" << "****************" << std::endl;
	std::filesystem::path p(DEFAULT_DATA_DIR);
	p.append("compress").append("R_RADR_I_S2400_20230721115441_O_DOR_YLD4-D_CAP_FMT.bin.bz2");
	silly_mmap test;
	if (test.open_m(p.string()))
	{
		std::cout << "success" << std::endl;
	}
	else
	{
		std::cout << "failed" << std::endl;
	}

};


BOOST_AUTO_TEST_SUITE_END()

#endif //SILLY_UTILS_TEST_FILE_HPP
