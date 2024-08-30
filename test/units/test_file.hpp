/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 18:06
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 文件关联处理功能测试
 */
#pragma once

#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>
#include "files/silly_file.h"
#include "files/silly_mmap.h"

TEST_CASE("TestFiles")
{
    SECTION("MMAP_READ")  // mmap文件读取
    {
        std::cout << "\r\n\r\n****************" << "MMAP_READ" << "****************" << std::endl;
        std::filesystem::path p(DEFAULT_SU_DATA_DIR);
        p.append("compress").append("R_RADR_I_S2400_20230721115441_O_DOR_YLD4-D_CAP_FMT.bin.bz2");

        std::string rar = "D:/1_wangyingjie/readfile/split/HN2.rar";

        silly_mmap test;
        if (test.open_m(rar))
        {
            std::cout << "success" << std::endl;
        }
        else
        {
            std::cout << "failed" << std::endl;
        }
    };
}

#endif  // SILLY_UTILS_TEST_FILE_HPP
