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
    /**
     * 读取文件内容
     * @param fp 文件路径
     * @param content 读取内容
     * @param offset 偏移offset的位置开始读
     * @param len 指定读取长度
     * @return 实际读取的长度
     */
    static size_t read(const suPath &fp, std::string &content, const size_t &offset = 0, const size_t &len = SIZE_MAX);
    static std::string read(const suPath &fp, const size_t &offset = 0, const size_t &len = SIZE_MAX);

    typedef std::function<bool(std::string&)> filter_func;
    /**
     * 根据过滤条件按行读取,
     * 只有当改行满足过滤条件时,才会添加到返回值中
     * @param fp
     * @param lines 返回值
     * @param func 过滤条件
     * @return
     */
    static bool readlines(const suPath &fp, std::vector<std::string> &lines, const filter_func& func = nullptr);
    static std::vector<std::string> readlines(const suPath &fp, const filter_func& func = nullptr);

    /**
     * 将内容写入文件
     * @param fp
     * @param content
     * @return
     */
    static size_t write(const suPath &fp, const std::string &content);

    /**
     * 逐行写入文件
     * 如果行末没有换行符,会自行添加
     * @param fp
     * @param lines
     * @return
     */
    static size_t write(const suPath &fp, const std::vector<std::string> &lines);

    /**
     * 保留原文件内容,追加写
     * @param fp
     * @param content
     * @return
     */
    static size_t append(const suPath &fp, const std::string &content);
    static size_t append(const suPath &fp, const std::vector<std::string> &lines);
};
using sufile = suFile;
#endif  // SILLY_FILE_H
