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

#ifndef SILLY_UTILS_SILLY_PYRAMID_INDEX_H
#define SILLY_UTILS_SILLY_PYRAMID_INDEX_H

#define TZX_IMAGE_DATA_INDEX_NAME "TzxImage.index"

#include <tzx/pyramid/silly_pyramid_base.h>
#include <tzx/pyramid/silly_pyramid_block.h>

#define TZX_IMAGE_DATA_POS_SIZE 8   // 记录data数据位置的数据大小
#define TZX_IMAGE_DATA_SIZE_SIZE 4  // 记录data数据大小的数据的大小
#define TZX_IMAGE_INDEX_INFO_SIZE 8
#define TZX_IMAGE_LAYER_SIZE 1
#define TZX_IMAGE_COLROW_SIZE 4

#define TZX_IMAGE_MAX_LEVEL 24
#define TZX_IMAGE_INDEX_DATA_BEGIN_POS 1024

namespace silly
{
namespace pyramid
{

struct layer_info
{
    uint64_t rbeg{0};
    uint64_t cbeg{0};

    uint64_t rend{0};
    uint64_t cend{0};

    uint64_t rows{0};
    uint64_t cols{0};

    layer_info& operator=(const layer_info& rh)
    {
        this->rbeg = rh.rbeg;
        this->cbeg = rh.cbeg;
        this->rows = rh.rows;
        this->cols = rh.cols;
        this->rend = rh.rend;
        this->cend = rh.cend;
        return *this;
    }

    bool out(block blk) const
    {
        return blk.row > rend || blk.row < rbeg || blk.col > cend|| blk.col < cbeg;
    }

    bool in(block blk) const
    {
        return blk.row >= rbeg && blk.row <= rend && blk.col >= cbeg && blk.col <= cend;
    }

    size_t index(block blk) const
    {
        return cols * (blk.row - rbeg) + blk.col - cbeg;
    }

    void fill() 
    {
        rows = rend - rbeg + 1;
        cols = cend - cbeg + 1;
    }
};

class index : public silly::pyramid::base
{
  public:
    index();

    /// <summary>
    /// 初始化各层的关键信息, 加快后面的计算
    /// </summary>
    /// <returns></returns>
    bool init_layer_info();

    bool open(const char* file, const silly_mmap::enum_mmap_open_mode& mode, const bool& usemmap);

    error read_block(block& blk);

    /// <summary>
    /// 获取索引文件,对应层数的起始位置
    /// </summary>
    /// <param name="layer"></param>
    /// <returns></returns>
    uint64_t get_layer_start_pos(const uint32_t& layer);

    /// <summary>
    /// 设置每层的起始信息
    /// </summary>
    /// <param name="linfo"></param>
    void set_layer_info(const uint32_t& level, const layer_info& linfo);

    /// <summary>
    /// 写入数据块中的指定块的索引数据 索引偏移量和数据块大小
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <param name="idata"></param>
    /// <returns></returns>
    bool write_block(const block& blk);

    void write_layer_info();

    bool close();

  public:
    std::map<uint32_t, layer_info> m_layer_infos;
    // 索引文件中,每层存储data信息的起始位置
    std::map<uint32_t, uint64_t> m_layer_bpos;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PYRAMID_INDEX_H
