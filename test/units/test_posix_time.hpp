/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: test_posix_time.h
 * @description: test_posix_time 类声明
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_UTILS_TEST_POSIX_TIME_H
#define SILLY_UTILS_TEST_POSIX_TIME_H
#ifdef CATCH2_UNIT_TEST
#include <catch2/catch_test_macros.hpp>
#include <datetime/silly_posix_time.h>
#include <log/silly_log.h>
#ifdef SILLY_UTILS_BOOST_ENABLE
#endif

TEST_CASE("POSIX_TIME")
{
    SECTION("NOW_AND_STRING")
    {
        silly_posix_time pt_curr;
        pt_curr.now();
        SLOG_DEBUG(pt_curr.to_string())
        SLOG_DEBUG(pt_curr.to_string(DATE_FORMAT_2))
        SLOG_DEBUG(pt_curr.to_string(DATE_FORMAT_3))
        SLOG_DEBUG(pt_curr.to_string(DATE_FORMAT_4))
        SLOG_DEBUG(pt_curr.to_string(DATE_FORMAT_5))
        SLOG_DEBUG(pt_curr.to_string(DATE_FORMAT_6))
        SLOG_DEBUG(pt_curr.to_string(DATE_FORMAT_7))
        SLOG_DEBUG(pt_curr.to_string(DATE_FORMAT_8))
    }

    SECTION("FROM_STR")
    {
        {
            silly_posix_time pt_tmp;
            CHECK(pt_tmp.from_string("2025-10-01 03:04:05.061") == true);
            SLOG_DEBUG(pt_tmp.to_string())
        }
        {
            silly_posix_time pt_tmp;
            CHECK(pt_tmp.from_string("2025-10-01 03:04:05") == true);
            SLOG_DEBUG(pt_tmp.to_string())
        }
        {
            silly_posix_time pt_tmp;
            CHECK(pt_tmp.from_string("2025-10-01 03:04") == true);
            SLOG_DEBUG(pt_tmp.to_string())
        }
        {
            silly_posix_time pt_tmp;
            CHECK(pt_tmp.from_string("2025-10-01 03:04:05.061", DATE_FORMAT_4) == false);
            SLOG_DEBUG(pt_tmp.to_string())
        }
        {
            silly_posix_time pt_tmp;
            CHECK(pt_tmp.from_string("20251001030405", DATE_FORMAT_4) == true);
            SLOG_DEBUG(pt_tmp.to_string())
        }
    }

    SECTION("ADD")
    {

        silly_posix_time pt1;
        pt1.from_string("2025-10-01 03:04:05");
        SLOG_DEBUG("当前时间{}", pt1.to_string())
        silly_posix_time pt2 = pt1 + silly_time_duration(1,0, 0);
        SLOG_DEBUG("+ 1小时{}", pt2.to_string())
        pt2 += silly_time_duration(1, 1, 0);
        SLOG_DEBUG("+ 1小时{}", pt2.to_string())
        pt2 -= silly_time_duration(1, 1, 0);
        SLOG_DEBUG("+ 1小时{}", pt2.to_string())

        pt1 = pt2 - silly_time_duration(1, 1, 0);
        SLOG_DEBUG("- 1小时{}", pt1.to_string())
        pt2.from_string("2025-10-07 09:58:42");

        SLOG_DEBUG("时间{} 减去 时间{}", pt1.to_string(), pt2.to_string())
        auto td = pt1 -pt2;
        SLOG_DEBUG("总秒数{}", td.total_seconds())
        SLOG_DEBUG("总分钟数{}", td.total_minutes())
        SLOG_DEBUG("总小时数{}", td.total_hours())
        SLOG_DEBUG("总天数{}", td.total_days())
        SLOG_DEBUG("{} 天 {} 小时 {} 分钟 {} 秒 ", td.total_days(), td.hours(), td.minutes(), td.seconds())


        SLOG_DEBUG("时间{} 减去 时间{}", pt2.to_string(), pt1.to_string())
        td = pt2 - pt1;
        SLOG_DEBUG("总秒数{}", td.total_seconds())
        SLOG_DEBUG("总分钟数{}", td.total_minutes())
        SLOG_DEBUG("总小时数{}", td.total_hours())
        SLOG_DEBUG("总天数{}", td.total_days())
        SLOG_DEBUG("{} 天 {} 小时 {} 分钟 {} 秒 ", td.total_days(), td.hours(), td.minutes(), td.seconds())
    }
}
#endif
#endif  // SILLY_UTILS_TEST_POSIX_TIME_H
