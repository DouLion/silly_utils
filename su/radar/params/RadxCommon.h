/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-02-24
 * @file: RadxCommon.h
 * @description: RadxCommon 类声明
 * @version: v1.0.1 2025-02-24 dou li yang
 */
#ifndef SILLY_UTILS_RADXCOMMON_H
#define SILLY_UTILS_RADXCOMMON_H
#include <su_marco.h>
namespace radar
{
typedef enum
{
    DEBUG_OFF = 0,
    DEBUG_NORM = 1,
    DEBUG_VERBOSE = 2,
    DEBUG_EXTRA = 3
} debug_t;


typedef enum {
    FILELIST = 0,
    ARCHIVE = 1,
    REALTIME = 2
} mode_t;

typedef enum {
    pFALSE = 0, pTRUE = 1
} tdrp_bool_t;


struct OutputFiled
{
    std::string id;
    std::string name;
    std::string long_name;
    std::string standard_name;
    std::string units;
    bool do_write;
};

struct CopyField
{
    std::string input_name;
    std::string output_name;
    std::string censor_non_weather;
};

}  // namespace radar

#endif  // SILLY_UTILS_RADXCOMMON_H
