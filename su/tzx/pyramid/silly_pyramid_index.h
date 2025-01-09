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

#include <tzx/pyramid/silly_pyramid_base.h>
#include <tzx/pyramid/silly_pyramid_block.h>
#include <geo/silly_geo.h>
namespace silly
{
namespace pyramid
{
const static std::string INDEX_NAME = "TzxImage.index";
namespace len
{
constexpr size_t DATA_POS = 8;  // 记录data数据位置的数据大小
constexpr size_t DATA_LEN = 4;  // 记录data数据大小的数据的大小
constexpr size_t IDXFIXED = 1024;
}  // namespace len

#pragma pack(1)
struct position
{
    size_t offset = 0;
    uint32_t size = 0;
};
#pragma pack()

class index;
class idx_pack
{
    friend class index;

  public:
    class layer
    {
        friend class idx_pack;
        friend class index;

      public:
        /// 给定行列号是否在本层中
        bool in(size_t row, size_t col) const;
        /// 获取index中的数据
        position seek(size_t row, size_t col) const;
        /// 计算在文件中的偏移位置
        size_t offset(size_t row, size_t col) const;
        void fill();

      public:
        size_t brow = 0;
        size_t bcol = 0;
        size_t erow = 0;
        size_t ecol = 0;
        size_t pos0 = 0;

        //
        uint32_t rows = 0;
        uint32_t cols = 0;

      private:
        // 这部分数据可能会很大,暂时不考虑使用其实现
        std::vector<position> index = {};
    };

  public:
    bool init();
    /* bool write(const std::string& file) = 0;
    virtual bool read(const std::string& file) = 0;*/
    /*virtual position seek(size_t row, size_t col) const = 0;
    virtual size_t offset(size_t row, size_t col) const = 0;*/
  public:
    // version 1
    uint8_t blayer = 0;
    uint8_t elayer = len::MAX_ZOOM;
    size_t  indexpos = len::IDXFIXED;
    silly_rect bound;
    layer layers[len::MAX_ZOOM];
};

class index : public silly::pyramid::base
{
  public:
    index();

    /// <summary>
    /// 解析数据
    /// </summary>
    /// <returns></returns>
    bool parse();

    bool open(const char* file, const silly::file::memory_map::access_mode& mode, const bool& usemmap);

    bool init();

    bool seek(block& blk);

    /// <summary>
    /// 写入数据块中的指定块的索引数据 索引偏移量和数据块大小
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <param name="idata"></param>
    /// <returns></returns>
    bool write(const block& blk);

    /// <summary>
    /// 写入基本信息
    /// </summary>
    /// <returns></returns>
    void write();

    bool close();

  private:
  public:
    idx_pack m_pack;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PYRAMID_INDEX_H
