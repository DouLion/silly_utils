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
#include <iostream>
#include <mutex>
#if IS_WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif


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
namespace silly{

class mmap
{
  public:
    using cur = char;
    class param
    {
      public:
        std::filesystem::path path;
        std::uintmax_t new_file_size = 0;
        std::size_t length = std::numeric_limits<std::size_t>::max();
        int64_t offset = 0;
        char* hint = nullptr;
        enum flags { ReadOnly, ReadWrite, Private } flag;
    };
  public:
    mmap() = default;
    ~mmap();

    bool open(const param& p);
    /// <summary>
    /// 打开内存文件映射, windows 写文件有问题,需要处理
    /// </summary>
    /// <param name="file"></param>
    /// <param name="mode"></param>
    /// <returns></returns>
    bool open(const std::string& file, const int& mode = param::ReadOnly, const int64_t& off = 0);

    /// <summary>
    /// 根据偏移量索引到内存位置
    /// </summary>
    /// <param name="offset"></param>
    /// <returns></returns>
    cur* at(const size_t& offset = 0);

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
    /// 关闭,析构函数已经调用此函数,要注意
    /// </summary>
    void close ();

    size_t size()
    {
        return m_size;
    }

    bool resize(size_t size);

  private:
    bool open();
    bool open_file();
    bool map_file();
    void try_map_file();
    bool unmap_file();
    void cleanup_and_throw(const char* msg);
    void clear();
    std::uintmax_t filesize();
  private:
    size_t m_size{200 * SU_MB};           // 映射大小
    size_t m_filesize{0};       // 文件大小
    cur* m_mmap{nullptr};  // 映射头位置
    std::mutex m_w_mutex;       // 写互斥
    bool m_is_wide{false};
    param m_param;

    int m_fd;
#if IS_WIN32
    HANDLE m_h_file;
    HANDLE m_h_map_file;
#endif
};
}

#endif  // SILLY_UTILS_SILLY_MMAP_H
