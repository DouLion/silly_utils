/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-10
 * @file: silly_pyramid_handle.h
 * @description: 处理金字塔数据的对象,本处理对象会自动覆盖源文件内容
 *               Handle of process pyramid data, this TzxPyramidHandle will cover the source file
 * @version: v1.0.1 2025-01-10 dou li yang
 */
#ifndef SILLY_PYRAMID_HANDLE_H
#define SILLY_PYRAMID_HANDLE_H
#include <tzx/pyramid/silly_pyramid_info.h>
#include <tzx/pyramid/silly_pyramid_index.h>
#include <tzx/pyramid/silly_pyramid_data.h>

class TzxPyramidHandle
{
  public:
    TzxPyramidHandle();
    ~TzxPyramidHandle();
    // 删除拷贝构造函数
    TzxPyramidHandle(const TzxPyramidHandle&) = delete;
    // 删除拷贝赋值操作符
    TzxPyramidHandle& operator=(const TzxPyramidHandle&) = delete;
    bool begin_read(const suPath& root);
    bool begin_write(const suPath& root);

    ////////////////////////////////////////////////////////////
    /// 读取功能
    ////////////////////////////////////////////////////////////

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
    suRect bound() const;

    /// <summary>
    /// 读取块信息,包含索引等
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool read(TzxPyramidBlock& blk);

    /// <summary>
    /// 读取块信息,仅包含数据
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    std::string read(const TzxPyramidBlock& blk);

    std::string format() const;
    std::string project() const;

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
    /// SU_PYRAMID_VERSION_1 或者 SU_PYRAMID_VERSION_2
    /// 不要使用 SU_PYRAMID_VERSION_11
    /// </summary>
    /// <param name="ver"></param>
    void version(const char ver[4]);

    /// <summary>
    /// 设置经纬度范围
    /// </summary>
    /// <param name="rect"></param>
    void bound(const suRect& rect);

    void format(const std::string& fmt);
    void project(const std::string& proj);

    /// <summary>
    /// 初始化索引层级
    /// </summary>
    /// <returns></returns>
    bool index_layer();

    /// <summary>
    /// 写入块信息
    /// </summary>
    /// <param name="blk">写入完成后会包含索引位置等信息</param>
    /// <returns></returns>
    bool write(TzxPyramidBlock& blk);

    ////////////////////////////////////////////////////////////
    /// 其他
    ////////////////////////////////////////////////////////////
    void close();

  private:
    bool open();  // 打开所有文件

  private:
    bool m_opened = false;
    std::string m_root;
    TzxPyramidIndex* m_index = nullptr;
    TzxPyramidInfo* m_info = nullptr;
    TzxPyramidData* m_data = nullptr;
    eMMFMode m_mode = eMMFMode::Read;
    bool m_use_mmap = false;

    std::mutex m_open_mutex;
};

#endif  // SILLY_PYRAMID_HANDLE_H
