/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/11 18:14
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SCALER_HPP
#define SILLY_UTILS_SCALER_HPP

#include <cmath>
#include "su_marco.h"
namespace silly_utils
{
class Scaler
{
  public:
    static bool equal(const float& f1, const float& f2)
    {
        return std::abs(f1 - f2) < SU_TINY;
    }

    static bool equal(const double& d1, const double& d2)
    {
        return std::abs(d1 - d2) < SU_TINY;
    }

    static float deg2rad(const float d)
    {
        return (d * SU_PI) / 180.0f;
    }

    static double deg2rad(const double d)
    {
        return (d * SU_PI) / 180.0;
    }
};
}  // namespace silly_utils
#endif  // SILLY_UTILS_SCALER_HPP
