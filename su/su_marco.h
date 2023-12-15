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
// 获取变量名称的宏
#include <stdio.h>
#include <iostream>
#include <cstring>
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


#define SU_STD_TIME_FORMAT1     "%04d-%02d-%02d %02d:%02d:%02d"

#ifndef PI
#define PI (3.1415926535897932384626433832795028841971693993751f)
#endif

#define SU_PI PI
#define SU_2PI (SU_PI * 2.)

// 自然指数e
#ifndef BASE_E
#define BASE_E 2.71828182845904523536
#endif
#define SU_BASE_E BASE_E

#ifndef TINY
#define TINY (0.0000001f)
#endif
#define SU_TINY TINY

// #define		EPS 与opencv的冲突
#ifndef SU_EPS
#define SU_EPS (.1e-12f)
#endif

/** earth radius */
#ifndef EARTH_RADIUS
#define EARTH_RADIUS (6378.137f)
#endif
#define SU_EARTH_RADIUS EARTH_RADIUS

#define LN_2 (0.693147180559945f)
#define SQRT_2 (1.4142135623730950488016887242097f)

#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_2PI (M_PI * 2.)

#define SU_RGB(r, g, b) ((unsigned int)(((unsigned char)(r) | ((unsigned int)((unsigned char)(g)) << 8)) | (((unsigned int)(unsigned char)(b)) << 16)))
#define SU_ARGB(a, r, g, b) (unsigned int)(((a) & 0xff) << 24 | ((r) & 0xff) << 16 | ((g) & 0xff) << 8 | (b & 0xff))
#define SU_RGBA(r, g, b, a) SU_ARGB(a, r, g, b)
#define SU_XRGB(r, g, b) SU_ARGB(0xff, r, g, b)

// #ifndef __FILENAME__
// #define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)
// #endif

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
#define SU_FILE_NAME (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define SU_FILE_NAME (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif
#endif

#ifndef SU_DEBUG_PRINT
// 打印调试信息
#ifndef NDEBUG
#define SU_DEBUG_PRINT(s, ...)                                                                                                                                                                  \
    printf(SU_PRINTF_COLOR_BROWN "\n[DEBUG] FILE:" SU_PRINTF_COLOR_RED " %s " SU_PRINTF_COLOR_BROWN "LINE: " SU_PRINTF_COLOR_RED "%I32d \n " SU_PRINTF_COLOR_BROWN "... ", SU_FILE_NAME, __LINE__); \
    printf(SU_PRINTF_COLOR_LIGHT_GREEN s SU_PRINTF_COLOR_NONE "\n", ##__VA_ARGS__);

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
#define SU_INFO_PRINT(s, ...)                                                                                                                                                                           \
    printf(SU_PRINTF_COLOR_CYAN "\n[INFO] FILE:" SU_PRINTF_COLOR_DARK_GRAY " %s " SU_PRINTF_COLOR_CYAN "LINE: " SU_PRINTF_COLOR_DARK_GRAY "%I32d \n " SU_PRINTF_COLOR_CYAN "... ", SU_FILE_NAME, __LINE__); \
    printf(SU_PRINTF_COLOR_DARK_GRAY s SU_PRINTF_COLOR_NONE "\n", ##__VA_ARGS__);
#endif

#ifndef SU_ERROR_PRINT
// 打印错误信息
#define SU_ERROR_PRINT(s, ...)                                                                                                                                                                            \
    printf(SU_PRINTF_COLOR_RED "\n[ERROR] FILE:" SU_PRINTF_COLOR_LIGHT_GREEN " %s " SU_PRINTF_COLOR_RED "LINE: " SU_PRINTF_COLOR_LIGHT_GREEN "%I32d \n " SU_PRINTF_COLOR_RED "... ", SU_FILE_NAME, __LINE__); \
    printf(SU_PRINTF_COLOR_LIGHT_GREEN s SU_PRINTF_COLOR_NONE "\n", ##__VA_ARGS__);
#endif



#ifndef SU_MARK_LINE
// 标记一行
#define SU_MARK_LINE printf(SU_PRINTF_COLOR_LIGHT_CYAN "\n[MARK] FILE:" SU_PRINTF_COLOR_LIGHT_RED " %s " SU_PRINTF_COLOR_LIGHT_CYAN "LINE: " SU_PRINTF_COLOR_LIGHT_RED "%I32d \n" SU_PRINTF_COLOR_NONE, SU_FILE_NAME, __LINE__);

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
