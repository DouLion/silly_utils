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

class idx_pack
{
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
        size_t rows = 0;
        size_t cols = 0;

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
    size_t indexpos = len::IDXFIXED;
    silly_rect bound;
    layer layers[len::MAX_ZOOM];
};

class index : public silly::pyramid::base
{
  public:
    index();

    /// <summary>
    /// 打开文件,读取信息
    /// </summary>
    /// <param name="file"></param>
    /// <param name="mode"></param>
    /// <param name="usemmap"></param>
    /// <returns></returns>
    bool open(const char* file, const silly::file::memory_map::access_mode& mode, const bool& usemmap) override;
    bool open(const std::string& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap) override;
    bool open(const std::filesystem::path& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap) override;

    /// <summary>
    /// 读取指定块在数据文件中的索引位置和数据大小
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool read(block& blk);

    /// <summary>
    /// 返回起始层
    /// </summary>
    /// <returns></returns>
    uint8_t beg_layer() const;

    /// <summary>
    /// 返回结束层
    /// </summary>
    /// <returns></returns>
    uint8_t end_layer() const;

    size_t brow(const uint8_t& layer) const;
    size_t bcol(const uint8_t& layer) const;
    size_t erow(const uint8_t& layer) const;
    size_t ecol(const uint8_t& layer) const;
    size_t rows(const uint8_t& layer) const;
    size_t cols(const uint8_t& layer) const;

    /// <summary>
    /// 范围经纬度范围
    /// </summary>
    /// <returns></returns>
    silly_rect bound() const;

    ////////////////////////////////////////////////////////////
    /// 写入功能
    ////////////////////////////////////////////////////////////

    /// <summary>
    /// 设置起始层
    /// </summary>
    /// <param name="beg"></param>
    void beg_layer(const uint8_t& beg) noexcept;

    /// <summary>
    /// 设置结束层
    /// </summary>
    /// <param name="end"></param>
    void end_layer(const uint8_t& end) noexcept;

    /// <summary>
    /// 设置版本号
    /// PYRAMID_VERSION_1 或者 PYRAMID_VERSION_2
    /// 不要使用 PYRAMID_VERSION_11
    /// </summary>
    /// <param name="ver"></param>
    void version(const char ver[4]) throw(std::invalid_argument);

    /// <summary>
    /// 设置经纬度范围
    /// </summary>
    /// <param name="rect"></param>
    void bound(const silly_rect& rect) throw(std::invalid_argument);

    /// <summary>
    /// 写模式时,构建索引
    /// </summary>
    /// <returns></returns>
    bool build();

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
    void write_info();

    void close();

  private:
    /// <summary>
    /// 解析数据
    /// </summary>
    /// <returns></returns>
    bool parse();

  private:
    idx_pack m_pack;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PYRAMID_INDEX_H
