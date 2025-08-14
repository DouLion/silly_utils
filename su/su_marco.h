/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/11 16:03
 * @version: 1.0.1
 * @software: silly_utils
 * @description:  基本头文件 以及 一些自定义宏,加上SU_前缀, 避免与第三方库冲突
 */
// 标准c++头文件
// 输入输出库
#pragma once

// =============================================================================
// C++ 标准库
// =============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>  // C++17
#include <memory>
#include <functional>
#include <exception>
#include <stdexcept>
#include <initializer_list>

// 容器
#include <vector>
#include <list>
#include <deque>
#include <forward_list>
#include <array>  // 固定大小数组
#include <stack>
#include <queue>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

// 算法与数值
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <limits>
#include <ratio>  // 有理数库
#include <cfenv>  // 浮点环境

// 工具 (C++17)
#include <any>       // C++17
#include <optional>  // C++17
#include <variant>   // C++17
#include <tuple>
#include <utility>
#include <type_traits>
#include <chrono>

// 输入/输出与文件系统
#include <fstream>
#include <filesystem>  // C++17
#include <iomanip>
#include <ios>

// 多线程
// 这部分在mingw下会有问题,还没能够处理
#include <thread>
#include <mutex>
#include <shared_mutex>  // C++14/17
#include <future>
#include <condition_variable>
#include <atomic>

// 其他
#include <regex>
#include <clocale>
#include <locale>
#include <cassert>
#include <cstddef>

// =============================================================================
// C 标准库 (C++ 风格头文件)
// =============================================================================

#include <cstdio>   // stdio.h
#include <cstdlib>  // stdlib.h
#include <cstring>  // string.h (优先于 string.h)
#include <cmath>    // math.h (优先于 math.h)
#include <cctype>   // ctype.h
#include <climits>  // limits.h
#include <ctime>    // time.h
#include <csignal>  // signal.h
#include <cerrno>   // errno.h

// =============================================================================
// 操作系统特定头文件与定义
// =============================================================================

#if defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
// Windows 特定头文件 (注意顺序!)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#pragma comment(lib, "WSOCK32.lib")
#pragma comment(lib, "ws2_32.lib")
#include <psapi.h>
#include <io.h>  // _open, _close 等

// Windows 类型别名
using silly_handle = HANDLE;
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(-1))
#endif

#else
// POSIX / Linux 特定头文件
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>  // 替代 Windows 的 _findfirst 等
#include <sys/stat.h>
#include <fcntl.h>

// 为非 Windows 系统定义二进制模式标志和句柄类型
#ifndef O_BINARY
#define O_BINARY 0  // 在 Unix-like 系统上通常不需要或为0
#endif

// 非 Windows 句柄类型
using silly_handle = int;
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (-1)
#endif
#endif  // _WIN32 || __MINGW32__ || __MINGW64__

// =============================================================================
// 其他平台无关但需要的头文件
// =============================================================================

// C 风格整数类型 (虽然 <cstdint> 更好，但有时需要)
// #include <stdint.h>        // 与 <cstdint> 功能重叠，通常用 <cstdint>
// #include <sys/types.h>     // size_t, off_t, pid_t 等 (在 POSIX 系统中)

// =============================================================================
// 本项目中的一些常量
// =============================================================================
#include <math/silly_math_const.h>
#include <geo/silly_geo_const.h>

// 数学常用常量 math/silly_math_const.h
#ifndef PI
#define PI silly::math::pi
#endif

#define SU_PI silly::math::pi

// 自然指数e
#ifndef BASE_E
#define BASE_E silly::math::e
#endif
#define SU_BASE_E silly::math::e

/// 2的平方根
#define SQRT_2 silly::math::sqrt_2

#define DEG2RAD(deg) ((deg) * silly::math::pi / 180.0)
#define RAD2DEG(rad) ((rad) * 180.0 / silly::math::pi)

#ifndef TINY
#define TINY (1e-18)
#endif
#define SU_TINY TINY

// 一个极小的浮点数, 用于浮点数比较		EPS 与opencv的冲突
#ifndef SU_EPSILON
#define SU_EPSILON (.1e-18)
#endif

#ifndef SU_TIME
#define SU_TIME
#define SEC_IN_MIN 60
#define MIN_IN_HOUR 60
#define HOUR_IN_DAY 24
#define SEC_IN_HOUR (SEC_IN_MIN * MIN_IN_HOUR)
#define MIN_IN_DAY (MIN_IN_HOUR * HOUR_IN_DAY)
#define SEC_IN_DAY (SEC_IN_MIN * MIN_IN_HOUR * HOUR_IN_DAY)
#endif

#ifndef SU_KB
#define SU_KB (1024)
#define SU_MB (1024 * 1024)
#define SU_GB (1024 * 1024 * 1024)
#endif

#define CN_RECT_LEFT 73.0
#define CN_RECT_TOP 54.0
#define CN_RECT_RIGHT 135.0
#define CN_RECT_BOTTOM 3.0

/// 地理常量 geo/silly_geo_const.h
#ifndef EARTH_RADIUS
#define EARTH_RADIUS silly::geo::earth_radius::equatorial
#endif
#define SU_EARTH_RADIUS silly::geo::earth_radius::equatorial

#define LN_2 (0.693147180559945f)

#define SU_RGB(r, g, b) ((unsigned int)(((unsigned char)(r) | ((unsigned int)((unsigned char)(g)) << 8)) | (((unsigned int)(unsigned char)(b)) << 16)))
#define SU_ARGB(a, r, g, b) (unsigned int)(((a) & 0xff) << 24 | ((r) & 0xff) << 16 | ((g) & 0xff) << 8 | (b & 0xff))
#define SU_RGBA(r, g, b, a) SU_ARGB(a, r, g, b)
#define SU_XRGB(r, g, b) SU_ARGB(0xff, r, g, b)

#define SU_VAR_NAME(var) #var

#define SU_CONSOLE_NONE "\033[m"
#define SU_CONSOLE_RED "\033[0;32;31m"
#define SU_CONSOLE_LIGHT_RED "\033[1;31m"
#define SU_CONSOLE_GREEN "\033[0;32;32m"
#define SU_CONSOLE_LIGHT_GREEN "\033[1;32m"
#define SU_CONSOLE_BLUE "\033[0;32;34m"
#define SU_CONSOLE_LIGHT_BLUE "\033[1;34m"
#define SU_CONSOLE_DARK_GRAY "\033[1;30m"
#define SU_CONSOLE_CYAN "\033[0;36m"
#define SU_CONSOLE_LIGHT_CYAN "\033[1;36m"
#define SU_CONSOLE_PURPLE "\033[0;35m"
#define SU_CONSOLE_LIGHT_PURPLE "\033[1;35m"
#define SU_CONSOLE_BROWN "\033[0;33m"
#define SU_CONSOLE_YELLOW "\033[1;33m"
#define SU_CONSOLE_LIGHT_GRAY "\033[0;37m"
#define SU_CONSOLE_WHITE "\033[1;37m"

#ifndef SU_FILE_NAME
/// 获取当前文件名称
#if IS_WIN32
#define SU_FILE_NAME (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : __FILE__)
#else
#define SU_FILE_NAME (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
#endif
#endif

/// 控制台信息print, 如果需要记录日志,使用 log/silly_log.h
#ifdef IS_WIN32
// windows 在控制台,以utf8编码输出
#define WINDOWS_UTF8_PAGE                                                      \
    {                                                                          \
        SetConsoleOutputCP(65001);                                             \
        CONSOLE_FONT_INFOEX info = {0};                                        \
        info.cbSize = sizeof(info);                                            \
        info.dwFontSize.Y = 18;                                                \
        info.FontWeight = FW_NORMAL;                                           \
        wcscpy_s(info.FaceName, L"Microsoft YaHei" /*需要是系统支持的字体*/);  \
        GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info); \
    }
#else
#define WINDOWS_UTF8_PAGE
#endif

#ifndef SU_DEBUG_PRINT
// 打印调试信息
#ifndef NDEBUG
#define SU_DEBUG_PRINT(s, ...) printf(SU_CONSOLE_BROWN "\n[DEBUG] %s:%I32d \n ... " s SU_CONSOLE_NONE " \n", SU_FILE_NAME, __LINE__, ##__VA_ARGS__);

#else
#define SU_DEBUG_PRINT(s, ...)
#endif
#endif

#ifndef SU_PRINTF
// 打印信息 未封装
#define SU_PRINTF(s, ...)
#endif

#ifndef SU_INFO_PRINT
// 打印提示信息
#define SU_INFO_PRINT(s, ...) printf(SU_CONSOLE_CYAN "\n[INFO] %s:%I32d \n " s SU_CONSOLE_NONE " \n", SU_FILE_NAME, __LINE__, ##__VA_ARGS__);
#define SU_INFO SU_INFO_PRINT
#endif

#ifndef SU_ERROR_PRINT
// 打印错误信息
#define SU_ERROR_PRINT(s, ...) printf(SU_CONSOLE_RED "\n[ERROR] %s:%I32d \n ... " s SU_CONSOLE_NONE, SU_FILE_NAME, __LINE__, ##__VA_ARGS__);
#endif

#ifndef SU_MARK_LINE
// 标记一行
#define SU_MARK_LINE printf(SU_CONSOLE_LIGHT_CYAN "\n[MARK] %s:%I32d \n" SU_CONSOLE_NONE, SU_FILE_NAME, __LINE__);
#endif

/// 常用函数
// std::max(a, b) 需要 a b同类型,
#ifndef SU_MAX
#define SU_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define SU_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define SU_CLAMP(v, min, max) (((v) < (min)) ? (min) : (((v) > (max)) ? (max) : (v)))
#define SU_MAX3(a, b, c) SU_MAX(a, SU_MAX(b, c))
#define SU_MIN3(a, b, c) SU_MIN(a, SU_MIN(b, c))
#endif

// 按照小端序转换
#define SU_LE_I64(p) ((p)[7] << 56 | ((p)[6] << 48) | ((p)[5] << 40) | ((p)[4] << 32) | ((p)[3] << 24) | ((p)[2] << 16) | ((p)[1] << 8) | (p)[0])
#define SU_LE_I32(p) ((p)[3] << 24 | ((p)[2] << 16) | (p)[1] << 8 | (p)[0])
#define SU_LE_I16(p) ((p)[1] << 8 | (p)[0])
// 按照大端序转换
#define SU_BE_I64(p) ((p)[0] << 56 | ((p)[1] << 48) | ((p)[2] << 40) | ((p)[3] << 32) | ((p)[4] << 24) | ((p)[5] << 16) | ((p)[6] << 8) | (p)[7])
#define SU_BE_I32(p) ((p)[0] << 24 | ((p)[1] << 16) | (p)[2] << 8 | (p)[3])
#define SU_BE_I16(p) ((p)[0] << 8 | (p)[1])

#ifndef SU_MEMCPY
#define SU_MEMCPY(p, off, v) memcpy((p) + off, &(v), sizeof(v));
#endif

#ifndef SU_MEMCPY_NEXT
#define SU_MEMCPY_NEXT(p, off, v)       \
    memcpy((p) + off, &(v), sizeof(v)); \
    off += sizeof(v);
#endif

#ifndef SU_MEM_FREE
// 内存释放 free
#define SU_MEM_FREE(p) \
    if ((p))           \
    {                  \
        free((p));     \
        (p) = nullptr; \
    }
#endif

#ifndef SU_MEM_DEL
// 内存释放 delete
#define SU_MEM_DEL(p)  \
    if ((p))           \
    {                  \
        delete (p);    \
        (p) = nullptr; \
    }
#endif

#ifndef SUM_MEM_DEL_ARR
// 内存释放 delete []
#define SUM_MEM_DEL_ARR(p) \
    if ((p))               \
    {                      \
        delete[] (p);      \
        (p) = nullptr;     \
    }
#endif

#ifdef IS_WIN32

static std::wstring MultiByteToWideCharSafe(const std::string& str, UINT codePage = CP_UTF8)
{
    if (str.empty())
        return L"";

    // 1. 计算所需缓冲区大小（不包括终止符）
    int wideCharLen = MultiByteToWideChar(codePage, 0, str.c_str(), (int)str.size(), nullptr, 0);
    if (wideCharLen <= 0)
    {
        throw std::runtime_error("MultiByteToWideChar size query failed");
    }

    // 2. 分配缓冲区（+1 用于终止符）
    std::wstring wideStr;
    wideStr.resize(wideCharLen+1);

    // 3. 执行转换
    int result = MultiByteToWideChar(codePage, 0, str.c_str(), (int)str.size(), &wideStr[0], wideCharLen);
    if (result <= 0)
    {
        throw std::runtime_error("MultiByteToWideChar conversion failed");
    }

    return wideStr;
}

static std::string WideCharToMultiByteSafe(const std::wstring& wstr, UINT codePage = CP_UTF8)
{
    if (wstr.empty())
        return "";

    // 计算所需多字节缓冲区大小（不包括终止符）
    const int multiByteLen = ::WideCharToMultiByte(codePage,                         // 代码页
                                                   0,                                // 转换标志
                                                   wstr.c_str(),                     // 源宽字符字符串
                                                   static_cast<int>(wstr.length()),  // 实际长度（不含终止符）
                                                   nullptr,                          // 不执行实际转换
                                                   0,                                // 仅获取缓冲区大小
                                                   nullptr,                          // 默认字符替换
                                                   nullptr                           // 是否使用默认字符
    );

    if (multiByteLen == 0)
    {
        const DWORD error = ::GetLastError();
        throw std::runtime_error("WideCharToMultiByte failed (size query): " + std::to_string(error));
    }

    // 使用vector自动管理内存（+1用于终止符）
    std::vector<char> multiByteBuf(multiByteLen + 1);

    // 执行实际转换
    const int result = ::WideCharToMultiByte(codePage, 0, wstr.c_str(), static_cast<int>(wstr.length()), multiByteBuf.data(), multiByteLen, nullptr, nullptr);

    if (result == 0)
    {
        const DWORD error = ::GetLastError();
        throw std::runtime_error("WideCharToMultiByte failed (conversion): " + std::to_string(error));
    }

    // 确保以null终止
    multiByteBuf[multiByteLen] = '\0';

    return std::string(multiByteBuf.data());
}

#endif