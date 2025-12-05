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
#ifndef SILLY_PYRAMID_DATA_H
#define SILLY_PYRAMID_DATA_H
#include <tzx/pyramid/silly_pyramid_index.h>
class TzxPyramidData : public TzxPyramidBase
{
  public:
    TzxPyramidData();
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <param name="mode"></param>
    /// <returns></returns>
    bool open(const suPath& file, const eMMFMode& mode, const bool& usemmap) override;

    /// <summary>
    ///
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <returns></returns>
    std::string read(const TzxPyramidBlock& blk);

    /// <summary>
    ///
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool read(TzxPyramidBlock& blk);

    /// <summary>
    /// 写入,需要将偏移位置等信息返回
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool write(TzxPyramidBlock& blk);

    void set(TzxPyramidIndex* idx);

    void close();

  private:
    TzxPyramidIndex* m_index;
};

#endif  // SILLY_PYRAMID_DATA_H
