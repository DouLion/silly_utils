/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:47
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_IMAGE_DATA_H
#define SILLY_UTILS_SILLY_IMAGE_DATA_H

#include "tzx/pyramid/silly_pyramid_index.h"

#define TZX_IMAGE_DATA_DATA_NAME "TzxImage.data"

struct block_data
{
    block_data()
    {
    }
    uint64_t offset{0};
    uint32_t size{0};
    char* data{nullptr};
};

class silly_pyramid_data : public silly_pyramid_base
{
  public:
    silly_pyramid_data();
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <param name="mode"></param>
    /// <returns></returns>
    bool open(const char* file, const silly_mmap::enum_mmap_open_mode& mode, const bool& usemmap);

    /// <summary>
    ///
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <returns></returns>
    std::string read_block(const uint32_t& layer, const uint64_t& row, const uint64_t& col);

    /// <summary>
    ///
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <param name="datasize"></param>
    /// <returns></returns>
    char* read_block(const uint32_t& layer, const uint64_t& row, const uint64_t& col, uint32_t& datasize);

    bool write_block(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const block_data& bdata);

  public:
    silly_pyramid_index m_index;
};

#endif  // SILLY_UTILS_SILLY_IMAGE_DATA_H
