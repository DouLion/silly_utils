/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/10 18:06
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 内存文件映射
 */
#ifndef SILLY_MEM_MAP_H
#define SILLY_MEM_MAP_H
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

enum eMMFMode : int
{
    Read = 1,
    Write = 2
};
/*
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5*/

enum eMMFDisposition : int
{
    // 与 windows中 fileapi.h文件定义保持一致
    CreateNew =1, // #define CREATE_NEW          1
    CreateAlways = 2,
    OpenExisting = 3,
    OpenAlways = 4,
    TruncateExisting = 5
};

class suMemMapFile
{
public:
    using Ptr = unsigned char*;
    struct Param
    {
        std::filesystem::path path;
        // 映射大小, 0表示映射从offset之后的文件
        std::uintmax_t map_size = 0;
        // 写模式时, 指定分配文件大小
        // 如果 file_size 比现有文件大, 则会扩张文件
        // 如果 file_size 比现有文件小, 则不处理
        std::size_t file_size = 0;
        int64_t offset = 0; // 映射偏移开始位置
        eMMFMode mode = eMMFMode::Read; // 打开模式
        unsigned long disposition = eMMFDisposition::OpenExisting;
    };
  public:
    suMemMapFile() = default;
    ~suMemMapFile() = default;
    // 禁用拷贝构造函数和赋值运算符
    suMemMapFile(const suMemMapFile& rh) = delete;
    suMemMapFile& operator=(const suMemMapFile& rh) = delete;

    bool open(const Param& p);
    /// <summary>
    /// 打开内存文件映射, windows 写文件有问题,需要处理
    /// </summary>
    /// <param name="file"></param>
    /// <param name="mode"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool open(const std::filesystem::path& file, const eMMFMode& mode = eMMFMode::Read, const int64_t& offset = 0);

    /// <summary>
    /// 根据偏移量索引到内存位置
    /// </summary>
    /// <param name="offset"></param>
    /// <returns></returns>
    Ptr ptr(const size_t& offset = 0) const;

    /// <summary>
    /// 读取内容
    /// </summary>
    /// <param name="dst">数据存位置</param>
    /// <param name="size">读取大小</param>
    /// <param name="offset">偏移位置</param>
    /// <returns></returns>
    bool read(Ptr dst, const size_t& size, const size_t& offset = 0) const;

    /// <summary>
    /// TODO: 写入数据到内存文件映射,并且持久化到本地文件,这个还没有完全实现
    /// </summary>
    /// <param name="src"></param>
    /// <param name="size"></param>
    /// <param name="offset"></param>
    /// <returns></returns>
    bool write(Ptr src, const size_t& size, const size_t& offset = 0);

    /// <summary>
    /// 同步内存文件映射到本地文件
    /// </summary>
    /// <returns></returns>
    // bool sync();

    bool unmap();

    /// <summary>
    /// 关闭,析构函数已经调用此函数,要注意
    /// </summary>
    void close(bool del = false);

    /* size_t size()
     {
         return m_len;
     }*/

    bool resize(size_t size);

  protected:
    bool is_open()  const;
    bool is_mapped()  const;
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
    Ptr m_mmap{nullptr};     // 映射头位置
    std::mutex m_w_mutex;     // 写互斥
    std::mutex m_oc;           // 开关互斥
    // suMemMapFile::Param m_param;
    std::filesystem::path m_file;

    eMMFMode m_mode = eMMFMode::Read; // 打开模式
    unsigned long m_disposition = eMMFDisposition::OpenExisting;

    SU_HANDLE m_file_hdl = INVALID_HANDLE_VALUE;
#ifdef IS_WIN32
    SU_HANDLE m_map_hdl = INVALID_HANDLE_VALUE;
#endif
    // bool m_is_hdl_internal = false;
};

#endif  // SILLY_MEM_MAP_H
