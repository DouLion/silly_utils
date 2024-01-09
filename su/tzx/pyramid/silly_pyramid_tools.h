/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/14 9:49
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_PYRAMID_TOOLS_H
#define SILLY_UTILS_SILLY_PYRAMID_TOOLS_H

#include "tzx/pyramid/silly_pyramid_info.h"
#include "tzx/pyramid/silly_pyramid_index.h"
#include "tzx/pyramid/silly_pyramid_data.h"

#include <mutex>
#include <thread>

/// <summary>
/// TzxImage 格式的文件组
/// </summary>
class silly_pyramid
{
  public:
    silly_pyramid(void) = default;

    bool open(const std::string& root, const silly_mmap::open_mode& mode = silly_mmap::open_mode::READONLY, bool usemmap = false);
    void close();

    /// <summary>
    /// 读取指定块的数据内容
    /// </summary>
    /// <param name="layer">块所在层号 0 - 24</param>
    /// <param name="row">块所在行号 0 - 2^layer-1</param>
    /// <param name="col">块所在列号 0 - 2^layer-1</param>
    /// <param name="size">块数据大小</param>
    /// <returns>数据内容</returns>
    char* read_data(const uint32_t& layer, const uint64_t& row, const uint64_t& col, size_t& size);

    /// <summary>
    /// 向指定块写入数据
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <param name="size"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    bool write(const uint32_t& layer, const uint64_t& row, const uint64_t& col, const size_t& size, const char* data);

    /// <summary>
    /// 将版本1的金字塔转为版本2的金字塔
    /// </summary>
    /// <param name="target_root"></param>
    /// <returns></returns>
    bool rebuild_to_v2(const std::string& target_root);

  private:
    silly_mmap::open_mode m_mode;
    std::string m_root;
    silly_pyramid_info m_info;
    silly_pyramid_data m_data;
};

class silly_pyramid_tools
{
};

#endif  // SILLY_UTILS_SILLY_PYRAMID_TOOLS_H
