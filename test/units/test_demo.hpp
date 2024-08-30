/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 18:35
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TEST_CASE")
{
    CHECK(true);
    CHECK(false);
    SECTION("SECTION")
    {
        CHECK(true);
        CHECK(false);
    }
}

#endif //SILLY_UTILS_TEST_DEMO_HPP
