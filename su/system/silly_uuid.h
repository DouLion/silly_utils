/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/9 11:39
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_UUID_H
#define SILLY_UTILS_SILLY_UUID_H

#include <stdint.h>
#include <array>
#include <string>

#include <sstream>
#include <iomanip>
#include <ios>
#ifdef IS_WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

class silly_uuid
{
  public:
    static std::string random_uuid();
};

#endif  // SILLY_UTILS_SILLY_UUID_H
