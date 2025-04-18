/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/1/2 14:04
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_UTILS_SILLY_FORMAT_H
#define SILLY_UTILS_SILLY_FORMAT_H
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <fmt/os.h>
#include <fmt/color.h>

namespace silly_format = fmt;

#define SFMT fmt::format

#ifndef SFP_DEBUG
#ifndef NDEBUG
#define SFP_DEBUG(s, ...)                                                                                            \
    fmt::print(fg(fmt::color::light_sky_blue) | fmt::emphasis::bold, "\n[DEBUG] {}:{}\n\t", SU_FILE_NAME, __LINE__); \
    fmt::print(fg(fmt::color::light_sky_blue) | fmt::emphasis::bold, s, ##__VA_ARGS__);

#else
#define SFP_DEBUG(s, ...)
#endif
#endif

#ifndef SFP_INFO
#define SFP_INFO(s, ...)                                                                    \
    fmt::print(fg(fmt::color::green_yellow), "\n[INFO] {}:{}\n\t", SU_FILE_NAME, __LINE__); \
    fmt::print(fg(fmt::color::green_yellow), s, ##__VA_ARGS__);                             \
    fmt::print("\n");
#endif

#ifndef SFP_WARN
#define SFP_WARN(s, ...)                                                              \
    fmt::print(fg(fmt::color::orange), "\n[WARN] {}:{}\n\t", SU_FILE_NAME, __LINE__); \
    fmt::print(fg(fmt::color::orange), s, ##__VA_ARGS__);                             \
    fmt::print("\n");
#endif

#ifndef SFP_ERROR
#define SFP_ERROR(s, ...)                                                             \
    fmt::print(fg(fmt::color::brown), "\n[ERROR] {}:{}\n\t", SU_FILE_NAME, __LINE__); \
    fmt::print(fg(fmt::color::brown), s, ##__VA_ARGS__);                              \
    fmt::print("\n");
#endif

#ifndef SFP_MARK
#define SFP_MARK fmt::print(fg(fmt::color::pale_green), "\n[MARK] {}:{}\n", SU_FILE_NAME, __LINE__);

#endif

#endif  // SILLY_UTILS_SILLY_FORMAT_H
