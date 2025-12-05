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
#ifndef SILLY_PYRAMID_BASE_H
#define SILLY_PYRAMID_BASE_H
#include <tzx/pyramid/silly_pyramid_marco.h>
#include <files/silly_memory_map.h>
#include <log/silly_log.h>

class TzxPyramidBase
{
    /// <summary>
    /// 读取时使用mmap, 创建和修改时使用普通文件流
    /// </summary>
  public:
    TzxPyramidBase() = default;
    /// <summary>
    ///
    /// </summary>
    /// <param name="filePath"></param>
    /// <param name="mode">1 读 2 写</param>
    /// <param name="usemmap">读取时默认 使用mmap, 写总是使用文件流</param>
    /// <returns></returns>
    // TODO: 目前 读使用mmap, 写使用文件流, 第三个参数暂时没用,后续测试完成后需要支持
    virtual bool open(const suPath& file, const eMMFMode& mode, const bool& usemmap);

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
    bool stream_open(const suPath& file, const std::ios_base::openmode& mode);

    /// <summary>
    /// 内存文件映射打开
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    bool mmap_open(const suPath& file);

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

  public:


  protected:
    char m_head[SU_PYRAMID_HEAD_LEN] = {0};
    char m_version[SU_PYRAMID_VER_LEN] = {0x00, 0x02, 0x00, 0x00};
    // 读写类型
    eMMFMode m_mode;
    // private:
    // 文件名称
    std::string m_file;

    // 是否已经打开
    bool m_opened{false};
    // 是否为普通文件流
    bool m_normal;
    // mmap文件
    suMemMapFile m_mmap;
    // 文件流
    std::fstream m_stream;
    // 多线程写时用的锁
    std::mutex m_mutex;
};

#endif  // SILLY_PYRAMID_BASE_H
