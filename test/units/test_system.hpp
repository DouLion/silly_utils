/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/9 11:43
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#if I_NEED_TEST
#include <catch2/catch_test_macros.hpp>
#include "system/silly_uuid.h"

TEST_CASE("TestSystem")
{
SECTION("CREATE_UUID")      // BZ2压缩文件
{
	std::cout << "\r\n\r\n****************" << "CREATE_UUID" << "****************" << std::endl;
	std::cout << silly_uuid::random_uuid() << std::endl;
}

}


#endif //SILLY_UTILS_TEST_SYSTEM_HPP
