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

/// 金字塔文件描述信息偏移量即信息长度
#define PYRAMID_DESC_OFFSET 0
#define PYRAMID_DESC_LENGTH 4
/// 金字塔文件主版本信息偏移量即信息长度
#define PYRAMID_MVER_OFFSET 4
#define PYRAMID_MVER_LENGTH 4
/// 金字塔文件次版本信息偏移量即信息长度
#define PYRAMID_PVER_OFFSET 8
#define PYRAMID_PVER_LENGTH 4

namespace silly
{
namespace pyramid
{
enum error
{
    OK = 0,
    UNKNOWN = 1,
    NOT_OPEN = 2
};

enum version
{
    Version_1 = 1,
    Version_2 = 2,
    Version_3 = 3,
    Version_4 = 4,
    Version_5 = 5,
    Version_6 = 6
};

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
    bool open(const char* file, const silly::file::memory_map::access_mode& mode, const bool& usemmap);

    /// <summary>
    /// 关闭文件
    /// </summary>
    /// <returns></returns>
    bool close();

    /// <summary>
    /// 读取指定大小的数据块
    /// </summary>
    /// <param name="seek_offset"></param>
    /// <param name="data"></param>
    /// <param name="read_size"></param>
    /// <param name="offset_in_data"></param>
    /// <returns></returns>
    bool read(size_t seek_offset, char* data, const size_t& read_size, const size_t& offset_in_data = 0);

    /// <summary>
    /// 写入指定大小的数据库
    /// </summary>
    /// <param name="seek_offset"></param>
    /// <param name="data"></param>
    /// <param name="write_size"></param>
    /// <param name="offset_in_data"></param>
    /// <returns></returns>
    bool write(size_t seek_offset, char* data, const size_t& write_size, const size_t& offset_in_data = 0);

    /// <summary>
    /// fseek,仅对stream有效
    /// </summary>
    /// <param name="pos"></param>
    /// <param name="flag"></param>
    void seek(const size_t& pos = 0, const int& flag = SEEK_SET);

  protected:
    /// <summary>
    /// 普通文件流打开
    /// </summary>
    /// <param name="file"></param>
    /// <param name="mode"></param>
    /// <returns></returns>
    bool stream_open(const char* file, const char* mode);

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
    bool stream_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset);

    /// <summary>
    /// 内存文件映射读取
    /// </summary>
    /// <param name="data"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool mmap_read(size_t seek_offset, char* data, const size_t& size, const size_t& offset);

    /// <summary>
    /// 文件流写入
    /// </summary>
    /// <param name="data"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool stream_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset);

    /// <summary>
    /// TODO: 内存文件映射写入, 这个目前有问题,暂不实现
    /// </summary>
    /// <param name="data"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool mmap_write(size_t seek_offset, char* data, const size_t& size, const size_t& offset);

    /// <summary>
    /// 关闭文件流
    /// </summary>
    void stream_close();

    /// <summary>
    /// 关闭内存文件映射
    /// </summary>
    void mmap_close();
    error read_info();

    void write_info();

  protected:
    // 文件名称
    std::string m_file;

    // 是否已经打开
    bool m_opened{false};
    // 是否为普通文件流
    bool m_normal;
    // mmap文件
    silly::file::memory_map m_mmap;
    // 文件流
    FILE* m_stream;
    // 主版本号	这两个再文件中总是为小端序 读写, 与文档描述中有出入
    unsigned int m_major_ver{2};
    // 次版本号
    unsigned int m_primary_ver{0};

    // 头信息
    char m_desc[4];
    // 多线程读写时用的锁
    std::mutex m_mutex;
    // 加载类型
    silly::file::memory_map::access_mode m_mode;
};
}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_PYRAMID_BASE_H
