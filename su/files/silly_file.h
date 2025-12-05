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
#ifndef SILLY_FILE_H
#define SILLY_FILE_H
#include <system/silly_system.h>

#ifndef SILLY_FILE_MATCH_ALL_WILDCHAR
#define SILLY_FILE_MATCH_ALL_WILDCHAR "*"
#endif
using stdfp = std::filesystem::path;
class suFile
{
  public:
    static std::filesystem::path realpath(const std::filesystem::path &fp);
    static std::filesystem::path realpath(const std::string &ftpstr);

    /// <summary>
    /// 读取文件内容
    /// </summary>
    /// <param name="path">文件路径</param>
    /// <param name="content">内容接受对象</param>
    /// <param name="offset">偏移位置</param>
    /// <param name="len">预定读取大小,SIZE_MAX 为默认全读取</param>
    /// <returns>实际读取大小</returns>
    static size_t read(const std::filesystem::path &fp, std::string &content, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    static std::string read(const std::filesystem::path &fp, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    /// <summary>
    /// 按行读取
    /// </summary>
    /// <param name="fp"></param>
    /// <param name="lines"></param>
    /// <returns></returns>
    static bool readlines(const std::filesystem::path &fp, std::vector<std::string> &lines);

    static std::vector<std::string> readlines(const std::filesystem::path &fp);
    typedef std::function<bool(std::string&)> filter_func;
    static bool readlines(const std::filesystem::path &fp, std::vector<std::string> &lines, filter_func func);

    static std::vector<std::string> readlines(const std::filesystem::path &fp, filter_func func);

    /// <summary>
    /// 将内容写入文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="content"></param>
    /// <returns></returns>
    static size_t write(const std::filesystem::path &fp, const std::string &content);

    /// <summary>
    /// 将内容写入文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="lines">line中需要自行加入换行符,此功能不添加</param>
    /// <returns></returns>
    static size_t write(const std::filesystem::path &fp, const std::vector<std::string> &lines);

    /// <summary>
    /// 文件大小
    /// </summary>
    /// <param name="fp"></param>
    /// <returns></returns>
    static size_t size(const std::filesystem::path &fp);

    /// <summary>
    /// 最新更新时间
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static std::time_t last_modify_sec(const std::filesystem::path &fp);

    /// <summary>
    /// 最新更新时间
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static std::time_t last_modify_ms(const std::filesystem::path &fp);

    /// <summary>
    /// 路径是否存在
    /// </summary>
    /// <param name="fp"></param>
    /// <returns></returns>
    static bool exist(const std::filesystem::path &fp);

    /// <summary>
    /// 列出(仅)当前文件夹下所有包含filter的文件
    /// TODO: windows 下的中文路径匹配有点问题
    /// </summary>
    /// <param name="path"></param>
    /// <param name="filter"></param>
    /// <returns></returns>
    static std::vector<std::filesystem::path> list(const std::filesystem::path &fp, const std::string &u8filter = SILLY_FILE_MATCH_ALL_WILDCHAR);

    /// <summary>
    /// 递归列出当前文件夹下所有包含filter的文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="filter"></param>
    /// <returns></returns>
    static std::vector<std::filesystem::path> relist(const std::filesystem::path &fp, const std::string &u8filter = SILLY_FILE_MATCH_ALL_WILDCHAR);

    /// <summary>
    /// 创建目录,如果存在多个未存在的路径,均会创建
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static bool mkdir(const std::filesystem::path &fp);

    /// <summary>
    /// 删除单个文件
    /// </summary>
    /// <param name="path"></param>
    static void rmfile(const std::filesystem::path &fp);

    /// <summary>
    /// 删除目录
    /// </summary>
    /// <param name="path"></param>
    static void rmdir(const std::filesystem::path &fp);

    /// <summary>
    /// 拷贝文件
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    static void copyfile(const std::filesystem::path &src, const std::filesystem::path &dst);

    /// <summary>
    /// 拷贝目录
    /// </summary>
    /// <param name="src"></param>
    /// <param name="dst"></param>
    static void copydir(const std::filesystem::path &src, const std::filesystem::path &dst);

  private:
    static std::string file_filter_regex(const std::string &filter);
};
using sufile = suFile;
#endif  // SILLY_FILE_H
