/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/11 16:03
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
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

#if WIN32
#include <Windows.h>
#include <winsock2.h>
#pragma comment(lib, "WSOCK32")
#pragma comment(lib, "ws2_32")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

// 一些自定义宏

#define SU_VAR_NAME(var) #var

#define SU_PRINTF_COLOR_NONE "\033[m"
#define SU_PRINTF_COLOR_RED "\033[0;32;31m"
#define SU_PRINTF_COLOR_LIGHT_RED "\033[1;31m"
#define SU_PRINTF_COLOR_GREEN "\033[0;32;32m"
#define SU_PRINTF_COLOR_LIGHT_GREEN "\033[1;32m"
#define SU_PRINTF_COLOR_BLUE "\033[0;32;34m"
#define SU_PRINTF_COLOR_LIGHT_BLUE "\033[1;34m"
#define SU_PRINTF_COLOR_DARK_GRAY "\033[1;30m"
#define SU_PRINTF_COLOR_CYAN "\033[0;36m"
#define SU_PRINTF_COLOR_LIGHT_CYAN "\033[1;36m"
#define SU_PRINTF_COLOR_PURPLE "\033[0;35m"
#define SU_PRINTF_COLOR_LIGHT_PURPLE "\033[1;35m"
#define SU_PRINTF_COLOR_BROWN "\033[0;33m"
#define SU_PRINTF_COLOR_YELLOW "\033[1;33m"
#define SU_PRINTF_COLOR_LIGHT_GRAY "\033[0;37m"
#define SU_PRINTF_COLOR_WHITE "\033[1;37m"

#define SU_STD_TIME_FORMAT1 "%04d-%02d-%02d %02d:%02d:%02d"

#ifndef PI
#define PI (3.1415926535897932384626433832795028841971693993751)
#endif

#define SU_PI PI
#define SU_2PI (SU_PI * 2.)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_2
#define M_2PI (M_PI * 2.)
#endif

// 自然指数e
#ifndef BASE_E
#define BASE_E 2.71828182845904523536
#endif
#define SU_BASE_E BASE_E

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

/** earth radius */
#ifndef EARTH_RADIUS
#define EARTH_RADIUS (6378.137)
#endif
#define SU_EARTH_RADIUS (6378.137f)

#define LN_2 (0.693147180559945f)

/// 2的平方根
#define SQRT_2 (1.4142135623730950488016887242097f)

#define SU_RGB(r, g, b) ((unsigned int)(((unsigned char)(r) | ((unsigned int)((unsigned char)(g)) << 8)) | (((unsigned int)(unsigned char)(b)) << 16)))
#define SU_ARGB(a, r, g, b) (unsigned int)(((a) & 0xff) << 24 | ((r) & 0xff) << 16 | ((g) & 0xff) << 8 | (b & 0xff))
#define SU_RGBA(r, g, b, a) SU_ARGB(a, r, g, b)
#define SU_XRGB(r, g, b) SU_ARGB(0xff, r, g, b)

#ifndef SU_CONSOLE_DEFAULT_ENCODE
#if IS_WIN32
#define SU_CONSOLE_DEFAULT_ENCODE "chcp 65001"
#else
#define SU_CONSOLE_DEFAULT_ENCODE "export LANG=zh_CN.UTF-8"
#endif
#endif

#ifndef SU_PRINT_COLORS
#define SU_PRINT_COLORS                                                    \
    printf(SU_PRINTF_COLOR_NONE "SU_PRINTF_COLOR_NONE\n");                 \
    printf(SU_PRINTF_COLOR_RED "SU_PRINTF_COLOR_RED\n");                   \
    printf(SU_PRINTF_COLOR_LIGHT_RED "SU_PRINTF_COLOR_LIGHT_RED\n");       \
    printf(SU_PRINTF_COLOR_GREEN "SU_PRINTF_COLOR_GREEN\n");               \
    printf(SU_PRINTF_COLOR_LIGHT_GREEN "SU_PRINTF_COLOR_LIGHT_GREEN\n");   \
    printf(SU_PRINTF_COLOR_BLUE "SU_PRINTF_COLOR_BLUE\n");                 \
    printf(SU_PRINTF_COLOR_LIGHT_BLUE "SU_PRINTF_COLOR_LIGHT_BLUE\n");     \
    printf(SU_PRINTF_COLOR_DARK_GRAY "SU_PRINTF_COLOR_DARK_GRAY\n");       \
    printf(SU_PRINTF_COLOR_CYAN "SU_PRINTF_COLOR_CYAN\n");                 \
    printf(SU_PRINTF_COLOR_LIGHT_CYAN "SU_PRINTF_COLOR_LIGHT_CYAN\n");     \
    printf(SU_PRINTF_COLOR_PURPLE "SU_PRINTF_COLOR_PURPLE\n");             \
    printf(SU_PRINTF_COLOR_LIGHT_PURPLE "SU_PRINTF_COLOR_LIGHT_PURPLE\n"); \
    printf(SU_PRINTF_COLOR_BROWN "SU_PRINTF_COLOR_BROWN\n");               \
    printf(SU_PRINTF_COLOR_YELLOW "SU_PRINTF_COLOR_YELLOW\n");             \
    printf(SU_PRINTF_COLOR_LIGHT_GRAY "SU_PRINTF_COLOR_LIGHT_GRAY\n");     \
    printf(SU_PRINTF_COLOR_WHITE "SU_PRINTF_COLOR_WHITE\n");

#endif

#ifndef SU_FILE_NAME
// 获取当前文件名称
#if IS_WIN32
#define SU_FILE_NAME (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : __FILE__)
#else
#define SU_FILE_NAME (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
#endif
#endif

#ifndef SU_DEBUG_PRINT
// 打印调试信息
#ifndef NDEBUG
#define SU_DEBUG_PRINT(s, ...) printf(SU_PRINTF_COLOR_BROWN "\n[DEBUG] %s:%I32d \n ... " s SU_PRINTF_COLOR_NONE " \n", SU_FILE_NAME, __LINE__, ##__VA_ARGS__);

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
#define SU_INFO_PRINT(s, ...) printf(SU_PRINTF_COLOR_CYAN "\n[INFO] %s:%I32d \n " s SU_PRINTF_COLOR_NONE " \n", SU_FILE_NAME, __LINE__, ##__VA_ARGS__);
#endif

#ifndef SU_ERROR_PRINT
// 打印错误信息
#define SU_ERROR_PRINT(s, ...) printf(SU_PRINTF_COLOR_RED "\n[ERROR] %s:%I32d \n ... " s SU_PRINTF_COLOR_NONE, SU_FILE_NAME, __LINE__, ##__VA_ARGS__);
#endif

#ifndef SU_MARK_LINE
// 标记一行
#define SU_MARK_LINE printf(SU_PRINTF_COLOR_LIGHT_CYAN "\n[MARK] %s:%I32d \n" SU_PRINTF_COLOR_NONE, SU_FILE_NAME, __LINE__);

#endif

#define SU_RETURN_OK \
    printf("OK.\n"); \
    return 0;
#define SU_RETURN_ERR \
    printf("ERR.\n"); \
    return 1;

#ifndef SU_MAX
#define SU_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef SU_MIN
#define SU_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

// 转化小端序的int值
#define SU_CONVERT_LITTLE_ENDIAN_LONG(src_ptr) (src_ptr[3] << 24 | (src_ptr[2] << 16) | src_ptr[1] << 8 | src_ptr[0])
// 转化小端序的int值
#define SU_CONVERT_LITTLE_ENDIAN_INT(src_ptr) (src_ptr[3] << 24 | (src_ptr[2] << 16) | src_ptr[1] << 8 | src_ptr[0])
// 转化小端序的short值
#define SU_CONVERT_LITTLE_ENDIAN_SHORT(src_ptr) (src_ptr[1] << 8) + src_ptr[0]

#ifndef SU_MEMCPY
#define SU_MEMCPY(p, off, v) memcpy(p + off, &v, sizeof(v));
#endif

#ifndef SU_MEMCPY_AUTO_INC
#define SU_MEMCPY_AUTO_INC(p, off, v) \
    memcpy(p + off, &v, sizeof(v));   \
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
