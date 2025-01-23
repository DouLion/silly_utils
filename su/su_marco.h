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
#include <iostream>
#include <sstream>
// 字符串库
#include <string>
// 容器
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <queue>
#include <stack>
// 算法库
#include <algorithm>
// 数值库
#include <cmath>
#include <numeric>
#include <random>
// 时间库
#include <chrono>
// 文件和流库
#include <fstream>
#include <filesystem>
#include <dirent.h>
// 异常处理库
#include <stdexcept>
// 内存管理库
#include <memory>
// 多线程库
#include <thread>
#include <mutex>
#include <future>
#include <atomic>
#include <functional>
// c++ 17 新的头文件
#include <any>          // 提供了一个类型安全的工具，可以存储单个值的任意类型。
#include <optional>     // 提供了一个模板类，用于表示可选的（有或没有值的）对象。
#include <string_view>  // 提供了对字符序列的非拥有、不可修改的引用。
#include <variant>      // 类似于联合（union），但是更安全和更易用，可以保存其中列出的任何数据类型。

// 标准c头文件
#include <stdio.h>   // 标准输入输出库
#include <stdlib.h>  // 标准库头文件，包括内存分配、程序控制、数学运算等
#include <string.h>  // 字符串处理库
#include <math.h>    // 数学函数库
#include <ctype.h>   // 字符类型库
#include <limits.h>  // 定义各种变量类型的属性
#include <assert.h>  // 提供断言测试支持
#include <errno.h>   // 错误号定义库
#include <time.h>    // 时间日期库
#include <locale.h>  // 本地化支持库
#include <stddef.h>  // 定义许多常用的类型和宏
#include <signal.h>  // 信号处理库
#include <regex>
//

#include <clocale>

#include <codecvt>
#include <cstring>
#include <locale>

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// 数据库
// #include <sql.h>
// #include <sqlext.h>
#if WIN32
// 注意这个顺序,一定是先Windows.h, 后winsock2.h
#include <Windows.h>
#include <winsock2.h>
#pragma comment(lib, "WSOCK32")
#pragma comment(lib, "ws2_32")

#include <psapi.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/mman.h>
#include <unistd.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif
#if WIN32
using silly_handle = HANDLE;
#else
using silly_handle = int;
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE -1
#endif
#endif
// 本项目中的常量
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
#if WIN32
// windows 在控制台,以utf8编码输出
#define WINDOWS_UTF8_PAGE                                                      \
    {                                                                          \
        SetConsoleOutputCP(65001);                                             \
        CONSOLE_FONT_INFOEX info = {0};                                        \
        info.cbSize = sizeof(info);                                            \
        info.dwFontSize.Y = 16;                                                \
        info.FontWeight = FW_NORMAL;                                           \
        wcscpy_s(info.FaceName, L"Consolas");                                  \
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info); \
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
