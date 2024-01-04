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
#include <files/TFF_FileUtils.h>
#include <su_marco.h>

#ifndef SILLY_FILE_MEM_FREE
#define SILLY_FILE_MEM_FREE SU_MEM_FREE
#endif

#ifndef SILLY_FILE_MATCH_ALL_WILDCHAR
#define SILLY_FILE_MATCH_ALL_WILDCHAR "*"
#endif

class silly_file
{
  public:
    /// <summary>
    /// 读取文件内容
    /// </summary>
    /// <param name="path">文件路径</param>
    /// <param name="content">内容接受对象</param>
    /// <param name="offset">偏移位置</param>
    /// <param name="len">预定读取大小,SIZE_MAX 为默认全读取</param>
    /// <returns>实际读取大小</returns>
    static size_t read(const std::string &path, std::string &content, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    static size_t read(const std::string &path, unsigned char **content, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    /// <summary>
    /// 将内容写入文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="content"></param>
    /// <returns></returns>
    static size_t write(const std::string &path, const std::string &content);

    /// <summary>
    /// 列出(仅)当前文件夹下所有包含filter的文件
    /// TODO: windwos 下的中文路径匹配有点问题
    /// </summary>
    /// <param name="path"></param>
    /// <param name="filter"></param>
    /// <returns></returns>
    static std::vector<std::string> list_all(const std::string &path, const std::string &filter = SILLY_FILE_MATCH_ALL_WILDCHAR);

    /// <summary>
    /// 递归列出当前文件夹下所有包含filter的文件
    /// </summary>
    /// <param name="path"></param>
    /// <param name="filter"></param>
    /// <returns></returns>
    static std::vector<std::string> list_all_recurse(const std::string &path, const std::string &filter = SILLY_FILE_MATCH_ALL_WILDCHAR);

  private:
    static std::string file_filter_regex(const std::string &filter);
};

#endif  // SILLY_UTILS_SILLY_FILE_H
