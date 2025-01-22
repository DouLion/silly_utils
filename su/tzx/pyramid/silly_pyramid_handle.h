/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-10
 * @file: silly_pyramid_handle.h
 * @description: 处理金字塔数据的对象,本处理对象会自动覆盖源文件内容
 *               Handle of process pyramid data, this handle will cover the source file
 * @version: v1.0.1 2025-01-10 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_PYRAMID_HANDLE_H
#define SILLY_UTILS_SILLY_PYRAMID_HANDLE_H
#include <tzx/pyramid/silly_pyramid_info.h>
#include <tzx/pyramid/silly_pyramid_index.h>
#include <tzx/pyramid/silly_pyramid_data.h>
namespace silly
{
namespace pyramid
{
class handle
{
  public:
    handle();
    ~handle();
    // 删除拷贝构造函数
    handle(const handle&) = delete;
    // 删除拷贝赋值操作符
    handle& operator=(const handle&) = delete;
    bool begin_read(const char* root);
    bool begin_read(const std::string& root);
    bool begin_read(const std::filesystem::path& root);

    bool begin_write(const char* root);
    bool begin_write(const std::string& root);
    bool begin_write(const std::filesystem::path& root);

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
    silly_rect bound() const;

    /// <summary>
    /// 读取块信息,包含索引等
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    bool read(block& blk);

    /// <summary>
    /// 读取块信息,仅包含数据
    /// </summary>
    /// <param name="blk"></param>
    /// <returns></returns>
    std::string read(const block& blk);

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
    /// PYRAMID_VERSION_1 或者 PYRAMID_VERSION_2
    /// 不要使用 PYRAMID_VERSION_11
    /// </summary>
    /// <param name="ver"></param>
    void version(const char ver[4]);

    /// <summary>
    /// 设置经纬度范围
    /// </summary>
    /// <param name="rect"></param>
    void bound(const silly_rect& rect);

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
    bool write(block& blk);

    ////////////////////////////////////////////////////////////
    /// 其他
    ////////////////////////////////////////////////////////////
    void close();

  private:
    bool open();  // 打开所有文件

  private:
    bool m_opened = false;
    std::string m_root;
    silly::pyramid::index* m_index;
    silly::pyramid::info* m_info;
    silly::pyramid::data* m_data;
    silly::file::memory_map::access_mode m_mode;
    bool m_use_mmap = false;

    std::mutex m_open_mutex;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PYRAMID_HANDLE_H
