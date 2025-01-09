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

#include <tzx/pyramid/silly_pyramid_index.h>

namespace silly
{
namespace pyramid
{
const static std::string DATA_NAME ="TzxImage.data";
class data : public silly::pyramid::base
{
  public:
    data();
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <param name="mode"></param>
    /// <returns></returns>
    bool open(const char* file, const silly::file::memory_map::access_mode& mode, const bool& usemmap);

    /// <summary>
    ///
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <returns></returns>
    std::string read(const block& blk);

    /// <summary>
    ///
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool read(block& blk);

    /// <summary>
    /// 写入,需要将偏移位置等信息返回
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool write(block& blk);

  public:
    index m_index;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_IMAGE_DATA_H
