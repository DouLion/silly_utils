/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:48
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PYRAMID_INFO_H
#define SILLY_UTILS_SILLY_PYRAMID_INFO_H

#include "tzx/pyramid/silly_pyramid_base.h"

namespace silly
{
namespace pyramid
{
const static std::string INFO_NAME ="TzxImage.info";
namespace len
{
constexpr size_t INFO_SRC = 512;
constexpr size_t INFO_PROJ = 1024;
constexpr size_t INFO_BOUND = 128;
constexpr size_t INFO_BT = 8;
constexpr size_t INFO_ET = 8;
constexpr size_t INFO_INTV = 4;
constexpr size_t INFO_FMT = 12;

// 预留
constexpr size_t INFO_TOTAL = 5640;
}  // namespace len
class info : public silly::pyramid::base
{
  public:
    enum tile_format
    {
        jpeg = 1,
        png,
        terrain,
        pbf
    };
    info();


    bool read();

    void write();

    void source(const std::string& src);

    void project(const std::string& proj);

    void bound(const std::string& bound);

    void format(const tile_format& fmt);

    void format(const std::string fmt);

    std::string source();
    std::string project();
    std::string bound();
    std::string format();



  protected:
    char m_source[len::INFO_SRC] = {0};
    /// Projection info.
    char m_projection[len::INFO_PROJ] = {0};
    /// Bound info.
    char m_bound[len::INFO_BOUND] = {0};
    /// version 1
    int64_t m_bt = 0;
    int64_t m_et = 0;
    int32_t m_interval = 0;
    /// version 2
    /// Tiling format info.
    char m_format[len::INFO_FMT] = {0};
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PYRAMID_INFO_H
