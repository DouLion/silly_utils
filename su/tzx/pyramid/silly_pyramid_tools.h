/*
 * @Author: douliyang 734546831@qq.com
 * @Date: 2024-01-09 10:14:17
 * @LastEditors: douliyang 734546831@qq.com
 * @LastEditTime: 2024-04-18 14:32:33
 * @FilePath: \pyramid\silly_pyramid_tools.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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

namespace silly
{
namespace pyramid
{
class tools
{
  public:
    tools(void) = default;

    bool open(const std::string& root, const silly::file::memory_map::access_mode& mode = silly::file::memory_map::access_mode::ReadOnly, bool usemmap = false);
    void close();

    /// <summary>
    /// 读取指定块的数据内容
    /// </summary>
    /// <param name="layer">块所在层号 0 - 24</param>
    /// <param name="row">块所在行号 0 - 2^layer-1</param>
    /// <param name="col">块所在列号 0 - 2^layer-1</param>
    /// <param name="size">块数据大小</param>
    /// <returns>数据内容</returns>
    std::string read(const block& blk);

    /// <summary>
    /// 向指定块写入数据
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <param name="size"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    bool write(const block& blk);

    /// <summary>
    /// 将版本1的金字塔转为版本2的金字塔
    /// </summary>
    /// <param name="target_root"></param>
    /// <returns></returns>
    bool rebuild_to_v2(const std::string& target_root);

    bool backup();

    std::string err();

  private:
    silly::file::memory_map::access_mode m_mode;
    std::string m_root;
    info m_info;
    data m_data;
    std::string m_err;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PYRAMID_TOOLS_H
