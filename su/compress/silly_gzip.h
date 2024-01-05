/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 11:28
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_GZIP_H
#define SILLY_UTILS_SILLY_GZIP_H

#include <iostream>

class silly_gzip
{
    /// <summary>
    /// 压缩文件
    /// </summary>
    /// <param name="s_src"></param>
    /// <param name="s_dst"></param>
    /// <returns></returns>
    int compress(const std::string& s_src, std::string& s_dst);

    /// <summary>
    ///  解压文件
    /// </summary>
    /// <param name="s_src"></param>
    /// <param name="s_dst"></param>
    /// <returns></returns>
    int decompress(const std::string& s_src, std::string& s_dst);

    /// <summary>
    /// 压缩内存数据
    /// </summary>
    /// <param name="c_in_val"></param>
    /// <param name="i_in_len"></param>
    /// <param name="c_out_val"></param>
    /// <param name="i_out_len"></param>
    /// <returns></returns>
    int compress(const char* c_in_val, const unsigned int& i_in_len, char** c_out_val, unsigned int* i_out_len);

    /// <summary>
    /// 解压内存数据
    /// </summary>
    /// <param name="c_in_val"></param>
    /// <param name="i_in_len"></param>
    /// <param name="c_out_val"></param>
    /// <param name="i_out_len"></param>
    /// <returns></returns>
    int decompress(const char* c_in_val, const unsigned int& i_in_len, char** c_out_val, unsigned int* i_out_len);
};

#endif  // SILLY_UTILS_SILLY_GZIP_H
