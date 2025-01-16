/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-29
 * @file: cairo_test.c
 * @description: cairo_test实现
 * @version: v1.0.1 2024-08-29 dou li yang
 */
#pragma once
#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>
#include "graphics/render/canvas/silly_cairo.h"

TEST_CASE("SILLY_CAIRO")
{
    SECTION("WRITE_WORD")
    {
        silly_cairo sc;
        sc.create(400, 400);
        silly_cairo_text sct;
        sct.text = "测试 test 123 ABC";
        sct.font_size = 20;
        sct.x = sc.width()/2;
        sct.y = sc.height()/2;
        sc.draw_text(sct);
        sc.write("cairo_text.png");
        sc.release();
    }
}

#endif

