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
#include <files/silly_path.h>

class suFile
{
  public:
    /// <summary>
    /// 读取文件内容
    /// </summary>
    /// <param name="fp">文件路径</param>
    /// <param name="content">内容接受对象</param>
    /// <param name="offset">偏移位置</param>
    /// <param name="len">预定读取大小,SIZE_MAX 为默认全读取</param>
    /// <returns>实际读取大小</returns>
    static size_t read(const suPath &fp, std::string &content, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    static std::string read(const suPath &fp, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    /// <summary>
    /// 按行读取
    /// </summary>
    /// <param name="fp"></param>
    /// <param name="lines"></param>
    /// <returns></returns>
    static bool readlines(const suPath &fp, std::vector<std::string> &lines);

    static std::vector<std::string> readlines(const suPath &fp);
    typedef std::function<bool(std::string&)> filter_func;
    static bool readlines(const suPath &fp, std::vector<std::string> &lines, filter_func func);

    static std::vector<std::string> readlines(const suPath &fp, filter_func func);

    /// <summary>
    /// 将内容写入文件
    /// </summary>
    /// <param name="fp"></param>
    /// <param name="content"></param>
    /// <returns></returns>
    static size_t write(const suPath &fp, const std::string &content);

    /// <summary>
    /// 将内容写入文件
    /// </summary>
    /// <param name="fp"></param>
    /// <param name="lines">line中需要自行加入换行符,此功能不添加</param>
    /// <returns></returns>
    static size_t write(const suPath &fp, const std::vector<std::string> &lines);
};
using sufile = suFile;
#endif  // SILLY_FILE_H
