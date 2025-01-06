/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/3 11:32
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_FILE_H
#define SILLY_UTILS_SILLY_FILE_H
#include <filesystem>
#include <su_marco.h>

#ifndef SILLY_FILE_MEM_FREE
#define SILLY_FILE_MEM_FREE SU_MEM_FREE
#endif

#ifndef SILLY_FILE_MATCH_ALL_WILDCHAR
#define SILLY_FILE_MATCH_ALL_WILDCHAR "*"
#endif
using stdfp = std::filesystem::path;

/// 统一 如果是使用filesytem 那么跟随系统, 使用 宽字符
/// 如果 是使用string ,输出统一使用utf8

namespace silly
{
namespace file
{

class node
{
  public:
    node() = default;
    ~node() = default;
    node(std::string path);
    node(std::filesystem::path path);

    node(const node &other);

    node &operator=(const node &other);

    std::string name() const;

    std::string stem() const;

    std::string ext() const;

    void trace();

  public:
    std::string path;
    bool is_dir = false;
    std::vector<std::unique_ptr<node>> children;
};
class tools
{
  public:
    /// <summary>
    /// 读取文件内容
    /// </summary>
    /// <param name="u8path">文件路径</param>
    /// <param name="content">内容接受对象</param>
    /// <param name="offset">偏移位置</param>
    /// <param name="len">预定读取大小,SIZE_MAX 为默认全读取</param>
    /// <returns>实际读取大小</returns>
    static size_t read(const std::string &u8path, std::string &content, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    static size_t read(const std::string &u8path, unsigned char **content, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    static bool read(const std::string &u8path, std::vector<std::string> &lines);

    static size_t size(const std::string &u8path);

    /// <summary>
    /// 将内容写入文件
    /// </summary>
    /// <param name="u8path"></param>
    /// <param name="content"></param>
    /// <returns></returns>
    static size_t write(const std::string &u8path, const std::string &content);

    static size_t write(const std::string &u8path, const std::vector<std::string> &lines);

    /// <summary>
    /// 列出(仅)当前文件夹下所有包含filter的文件
    /// TODO: windows 下的中文路径匹配有点问题
    /// </summary>
    /// <param name="u8path"></param>
    /// <param name="filter"></param>
    /// <returns></returns>
    static std::vector<std::string> list(const std::string &u8path, const std::string &filter = SILLY_FILE_MATCH_ALL_WILDCHAR);

    /// <summary>
    /// 递归列出当前文件夹下所有包含filter的文件
    /// </summary>
    /// <param name="u8path"></param>
    /// <param name="filter"></param>
    /// <returns></returns>
    static std::vector<std::string> relist(const std::string &u8path, const std::string &filter = SILLY_FILE_MATCH_ALL_WILDCHAR);

    /// <summary>
    /// 最新更新时间
    /// </summary>
    /// <param name="u8path"></param>
    /// <returns></returns>
    static size_t last_modify_sec(const std::string &u8path);

    /// <summary>
    /// 最新更新时间
    /// </summary>
    /// <param name="u8path"></param>
    /// <returns></returns>
    static size_t last_modify_ms(const std::string &u8path);

    static bool exist(const std::string &u8path);
    static bool exist(const std::filesystem::path &u8path);

    /// <summary>
    /// 创建目录,如果存在多个未存在的路径,均会创建
    /// </summary>
    /// <param name="u8path"></param>
    /// <returns></returns>
    static bool mkdir(const std::string &u8path);
    static bool mkdir(const std::filesystem::path &u8path);

    /// <summary>
    /// 删除单个文件
    /// </summary>
    /// <param name="u8path"></param>
    static void rm(const std::string &u8path);
    static void rm(const std::filesystem::path &u8path);

    /// <summary>
    /// 删除目录
    /// </summary>
    /// <param name="u8path"></param>
    static void rrm(const std::string &u8path);
    static void rrm(const std::filesystem::path &u8path);

    /// <summary>
    /// 拷贝文件
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    static void copy(const std::string &src, const std::string &dst);
    static void copy(const std::filesystem::path &src, const std::filesystem::path &dst);

    /// <summary>
    /// 拷贝目录
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    static void rcopy(const std::string &src, const std::string &dst);
    static void rcopy(const std::filesystem::path &src, const std::filesystem::path &dst);

  private:
    static std::string file_filter_regex(const std::string &filter);
};
}  // namespace file
}  // namespace silly

using silly_file = silly::file::tools;
#endif  // SILLY_UTILS_SILLY_FILE_H
