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

#ifndef SILLY_UTILS_PYRAMID_BASE_H
#define SILLY_UTILS_PYRAMID_BASE_H

#include <files/silly_memory_map.h>
#define PYRAMID_MATCH_VERSION(a, b) ((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2] && (a)[3] == (b)[3])

namespace silly
{
namespace pyramid
{

namespace len
{
constexpr size_t HEAD = 4;       // 头长度
constexpr size_t VER = 4;        // 版本信息长度
constexpr size_t MAX_ZOOM = 25;  // 最大层级数
}  // namespace len
constexpr char PYRAMID_VERSION_1[len::VER] = {0x00, 0x01, 0x00, 0x00};   // 0x00010000;
constexpr char PYRAMID_VERSION_2[len::VER] = {0x02, 0x00, 0x00, 0x00};   // 0x00020000;
constexpr char PYRAMID_VERSION_11[len::VER] = {0x01, 0x00, 0x00, 0x00};  // 0x01000000;

class base
{
    /// <summary>
    /// 读取时使用mmap, 创建和修改时使用普通文件流
    /// </summary>
  public:
    base() = default;
    /// <summary>
    ///
    /// </summary>
    /// <param name="filePath"></param>
    /// <param name="mode">1 读 2 写</param>
    /// <param name="usemmap">读取时默认 使用mmap, 写总是使用文件流</param>
    /// <returns></returns>
    // TODO: 目前 读使用mmap, 写使用文件流, 第三个参数暂时没用,后续测试完成后需要支持
    virtual bool open(const char* file, const silly::file::memory_map::access_mode& mode, const bool& usemmap);
    virtual bool open(const std::string& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap);
    virtual bool open(const std::filesystem::path& file, const silly::file::memory_map::access_mode& mode, const bool& usemmap);

    /// <summary>
    /// 关闭文件
    /// </summary>
    /// <returns></returns>
    void close();

    /// <summary>
    /// 读取指定大小的数据
    /// </summary>
    /// <param name="seek_offset"></param>
    /// <param name="data"></param>
    /// <param name="read_size"></param>
    /// <returns></returns>
    bool read(const size_t& seek_offset, char* data, const size_t& read_size);

    /// <summary>
    /// 写入指定大小的数据
    /// </summary>
    /// <param name="seek_offset"></param>
    /// <param name="data"></param>
    /// <param name="write_size"></param>
    /// <returns></returns>
    bool write(const size_t& seek_offset, const char* data, const size_t& write_size);

    /// <summary>
    /// 文件末尾写入
    /// </summary>
    /// <param name="data"></param>
    /// <param name="write_size"></param>
    /// <returns></returns>
    size_t append(const char* data, const size_t& write_size);

    /// <summary>
    /// 设置版本信息
    /// </summary>
    /// <param name="ver"></param>
    void version(const char ver[4]);

    /// <summary>
    /// 获取文件末尾的位置
    /// </summary>
    /// <returns></returns>
    size_t end();


  protected:
    /// <summary>
    /// 普通文件流打开
    /// </summary>
    /// <param name="file"></param>
    /// <param name="mode"></param>
    /// <returns></returns>
    bool stream_open(const char* file, const std::ios_base::openmode& mode);

    /// <summary>
    /// 内存文件映射打开
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    bool mmap_open(const char* file);

    /// <summary>
    /// 文件流读取
    /// </summary>
    /// <param name="data"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool stream_read(const size_t& seek_offset, char* data, const size_t& size);

    /// <summary>
    /// 内存文件映射读取
    /// </summary>
    /// <param name="data"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool mmap_read(const size_t& seek_offset, char* data, const size_t& size);

    /// <summary>
    /// 文件流写入
    /// </summary>
    /// <param name="data"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    void stream_write(const size_t& seek_offset, const char* data, const size_t& size);

    /// <summary>
    /// TODO: 内存文件映射写入, 这个目前有问题,暂不实现
    /// </summary>
    /// <param name="data"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool mmap_write(const size_t& seek_offset, const char* data, const size_t& size);

    /// <summary>
    /// fseek,仅对stream有效
    /// </summary>
    /// <param name="pos"></param>
    void seek(const size_t& pos = 0);

    /// <summary>
    /// 关闭文件流
    /// </summary>
    void stream_close();

    /// <summary>
    /// 关闭内存文件映射
    /// </summary>
    void mmap_close();
    bool read_info();

    void write_info();

  protected:
    char m_head[len::HEAD] = {0};
    char m_version[len::VER] = {0x00, 0x02, 0x00, 0x00};
    // 读写类型
    silly::file::memory_map::access_mode m_mode;
    // private:
    // 文件名称
    std::string m_file;

    // 是否已经打开
    bool m_opened{false};
    // 是否为普通文件流
    bool m_normal;
    // mmap文件
    silly::file::memory_map m_mmap;
    // 文件流
    std::fstream m_stream;
    // 多线程写时用的锁
    std::mutex m_mutex;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_PYRAMID_BASE_H
