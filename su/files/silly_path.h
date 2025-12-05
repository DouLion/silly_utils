/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-04
 * @file: silly_path.cpp
 * @description: 文件路径处理
 *                  主要是为了处理windows平台下的中文路径
 * @version: v1.0.1 2025-12-04 dou li yang
 */
#ifndef SILLY_PATH_H
#define SILLY_PATH_H
#include <system/silly_system.h>
#define SU_MATCH_ALL_WILD_CHAR "*"
class suPath
{
  public:
    suPath() = default;
    suPath(const std::string& path);
    suPath(const char* path);
    suPath(const suPath& path);
    suPath(const std::filesystem::path& path);
    ~suPath() = default;
    /**
     * 添加节点 目录
     * windows下会自动将含有中文的UTF8 转为GBK路径,避免出现问题
     * @param node
     * @return
     */
    suPath& append(const std::string& node);
#if _WIN32
    suPath(const std::wstring& path);
    suPath(const wchar_t* path);
    suPath& append(const std::wstring& node);
    std::wstring wstring() const;
    std::wstring name_ws() const;
    std::wstring stem_ws() const;
    std::wstring extension_ws() const;
#endif

////////////////////////////////////////////////////////////////
/// 基本属性
////////////////////////////////////////////////////////////////
    // 转换为 string
    operator std::string() const { return m_path.string(); }
    static suPath cwd();

    /**
     *
     * @return
     */
    bool is_absolute() const;
    suPath absolute() const;
    static suPath absolute(const suPath& path);


    suPath relative(const suPath& root) const;
    static suPath relative(const suPath& src, const suPath& root);

    /**
     * 腹肌目录
     * @return
     */
    suPath parent() const;

    suPath root() const;
    static suPath root(const suPath& path);

    static bool exists(const suPath& path);
    bool exists() const;

    /**
     *  是否问文件夹
     * @return
     */
    bool is_dir() const;
    static bool is_dir(const suPath& path);

    /**
     * 是否为普通文件(存储数据的文件),使用is_regular_file实现
     * is_block_file: 检查是否为块设备文件（如硬盘等存储设备）
     * is_character_file: 检查是否为字符设备文件（如键盘、串口等设备）
     * 在Windows系统上，块设备和字符设备的概念不适用，这些函数通常会返回false。
     * 这些函数主要用于Unix/Linux系统中区分不同类型的特殊文件。
     * @return
     */
    bool is_file() const;
    static bool is_file(const suPath& path);

    /**
     * 是否为链接文件
     * @return
     */
    bool is_link() const;
    static bool is_link(const suPath& path);

    std::string string() const;
    std::string u8string() const;

    /**
     *  跟随系统的文件名称,包含后缀
     * @return
     */
    std::string name() const;

    /**
     *  utf8的文件名称,包含后缀
     * @return
     */
    std::string name_utf8() const;

    /**
     *  跟随系统的文件名称,不包含后缀
     * @return
     */
    std::string stem() const;

    /**
     *  utf8的文件名称,不包含后缀
     * @return
     */
    std::string stem_utf8() const;

    /**
     *  跟随系统的文件后缀
     * @return
     */
    std::string extension() const;

    /**
     * utf8的文件后缀
     * @return
     */
    std::string extension_utf8() const;

    /**
     * 文件大小, 仅争对普通文件
     * @return
     */
    size_t size() const;
    static size_t size(const suPath &fp);

    /**
     * 文件(夹)修改时间戳
     * @return
     */
    std::time_t mstamp() const;
    static std::time_t mstamp(const suPath &fp);

    /**
    * 文件(夹)创建时间戳, linux下不一定有效
    * 在 Linux 中，标准的 ext4 文件系统并不直接记录文件的“创建时间”（birth time），
    * 但现代 Linux 内核（2.6.23+）和某些文件系统（如 ext4、Btrfs、XFS、ZFS 等）
    * 已经支持记录文件的创建时间（称为 crtime或 birth time）。
    * @return
    */
    std::time_t crstamp() const;
    static std::time_t crstamp(const suPath &fp);

    /**
     * 最近一次访问时间
     * @return
     */
    std::time_t astamp() const;
    static std::time_t astamp(const suPath &fp);


////////////////////////////////////////////////////////////////
/// 基本操作
////////////////////////////////////////////////////////////////
    static void chdir(const suPath& path);
    static bool mkdir(const suPath& path);

    /**
     * 创建软链接, 仅支持普通文件和目录
     * 设备,串口,链接 文件. 不支持创建链接
     * @param lnk 链接地址, 如果lnk目录不存在,会自动创建
     */
    void mklnk(const suPath& lnk) const;
    static void mklnk(const suPath& src, const suPath& lnk);

    /**
     * 删除普通文件
     * @param fp
     */
    static void rmfile(const suPath &fp);

    /**
     * 删除目录, 慎用递归
     * @param fp
     * @param r 是否递归删除
     */
    static void rmdir(const suPath &fp, const bool& r = false);

    /**
     * 删除软链接
     * @param fp
     */
    static void rmlnk(const suPath &fp);

    /**
     * 拷贝文件
     * @param src
     * @param dst
     * @param cover 如果原文件存在,是否覆盖
     */
    static void copyfile(const suPath &src, const suPath &dst, const bool& cover=false);

    /**
     * 递归拷贝目录
     * @param src
     * @param dst
     * @param cover 如果有文件存在,是否覆盖
     */
    static void copydir(const suPath &src, const suPath &dst, const bool& cover=false);

    /**
     * 根据通配符 列出(仅)文件夹下所有匹配的文件
     * @param fp
     * @param u8filter 如果有中文, 必须是utf8编码
     * @return
     */
    std::vector<suPath> list(const std::string &u8filter = SU_MATCH_ALL_WILD_CHAR) const;
    static std::vector<suPath> list(const suPath &fp, const std::string &u8filter = SU_MATCH_ALL_WILD_CHAR);

    /**
     * 根据通配符 递归列出文件夹下所有匹配的文件
     * @param fp
     * @param u8filter
     * @return
     */
    std::vector<suPath> relist(const std::string &u8filter = SU_MATCH_ALL_WILD_CHAR) const;
    static std::vector<suPath> relist(const suPath &fp, const std::string &u8filter = SU_MATCH_ALL_WILD_CHAR);

    /**
     * 预留此函数,以处理现在还未考虑到的情况
     * @return
     */
    std::filesystem::path path() const;

  protected:
    std::filesystem::path m_path = std::filesystem::current_path();
};

using supath = suPath;

#endif  // SILLY_PATH_H
