/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 18:06
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_MMAP_H
#define SILLY_UTILS_SILLY_MMAP_H
#include <su_marco.h>
#include <files/silly_file.h>

/// <summary>
/// 内存文件映射功能.
/// 最大允许映射512G, 理论上在64位机器上可以映射2^64字节（约16 EB）
/// 同一个资源文件，不能同时读和写? 这一点存疑,不同系统上表现可能不同,需要想办法测试，否则指向文件的指针会乱掉，导致程序异常奔溃。
/// 参照: https://blog.csdn.net/baidu_38172402/article/details/106673606
/// https://zhuanlan.zhihu.com/p/477641987
/// https://learn.microsoft.com/zh-CN/windows/win32/api/winbase/nf-winbase-createfilemappinga
/// https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
/// boost/libs/iostreams/src/mapped_file.cpp
/// </summary>
namespace silly
{
namespace file
{
static inline size_t page_size()
{
    static const size_t page_size = [] {
#ifdef _WIN32
        SYSTEM_INFO SystemInfo;
        GetSystemInfo(&SystemInfo);
        return SystemInfo.dwAllocationGranularity;
#else
        return sysconf(_SC_PAGE_SIZE);
#endif
    }();
    return page_size;
}
size_t inline make_offset_page_aligned(size_t offset) noexcept
{
    const size_t page_size_ = page_size();
    // Use integer division to round down to the nearest page alignment.
    return offset / page_size_ * page_size_;
}

class memory_map
{
  public:
    enum access_mode
    {
        Read = 1,
        Write = 2
    };
  public:
    using cur = char;
    class param
    {
      public:
        std::filesystem::path path;
        std::uintmax_t new_file_size = 0;
        std::size_t length = std::numeric_limits<std::size_t>::max();
        int64_t offset = 0;
        access_mode flag;
    };

  public:
    memory_map(void);
    ~memory_map(void);
    // 禁用拷贝构造函数和赋值运算符
    memory_map(const memory_map& rh) = delete;
    memory_map& operator=(const memory_map& rh) = delete;

    bool open(const param& p);
    /// <summary>
    /// 打开内存文件映射, windows 写文件有问题,需要处理
    /// </summary>
    /// <param name="file"></param>
    /// <param name="mode"></param>
    /// <returns></returns>
    bool open(const std::string& file, const int& mode = access_mode::Read, const int64_t& off = 0);

    /// <summary>
    /// 根据偏移量索引到内存位置
    /// </summary>
    /// <param name="offset"></param>
    /// <returns></returns>
    cur* ptr(const size_t& offset = 0);

    /// <summary>
    /// 读取内容
    /// </summary>
    /// <param name="dst">数据存位置</param>
    /// <param name="size">读取大小</param>
    /// <param name="offset">偏移位置</param>
    /// <returns></returns>
    bool read(cur* dst, const size_t& size, const size_t& offset = 0);

    /// <summary>
    /// TODO: 写入数据到内存文件映射,并且持久化到本地文件,这个还没有完全实现
    /// </summary>
    /// <param name="src"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool write(cur* src, const size_t& size, const size_t& offset = 0);

    /// <summary>
    /// 同步内存文件映射到本地文件
    /// </summary>
    /// <returns></returns>
    bool sync();

    bool unmap();

    /// <summary>
    /// 关闭,析构函数已经调用此函数,要注意
    /// </summary>
    void close();

    /* size_t size()
     {
         return m_len;
     }*/

    bool resize(size_t size);

  private:
    bool is_open();
    bool is_mapped();
    bool open();
    bool open_file();
    bool map_file();
    void try_map_file();
    void cleanup_and_throw(const char* msg);
    void clear();
    std::uintmax_t filesize();

  private:
    size_t m_map_len = 0;     // 映射大小
    size_t m_file_len{0};     // 文件大小 只读时, 映射大小不超过文件大小
    size_t m_map_offset = 0;  // 映射偏移位置
    cur* m_mmap{nullptr};     // 映射头位置
    std::mutex m_w_mutex;     // 写互斥
    param m_param;

    silly_handle m_hdl_file = INVALID_HANDLE_VALUE;
#if WIN32
    silly_handle m_hdl_map = INVALID_HANDLE_VALUE;
#endif
    // bool m_is_hdl_internal = false;
};
}  // namespace file
}  // namespace silly
#endif  // SILLY_UTILS_SILLY_MMAP_H
