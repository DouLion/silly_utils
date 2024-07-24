/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/10 15:53
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
#pragma once

#ifndef SILLY_UTILS_FTP_UTILS_H
#define SILLY_UTILS_FTP_UTILS_H

#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <fstream>

struct ftp_info
{
    size_t size{0};
    unsigned char* cache{nullptr};
};

struct ftp_header_info
{
    std::vector<std::string> messages;
    ;
};

struct ftp_file_info
{
    std::string create_time;
    size_t file_size{0};
    std::string file_name;
    bool is_dir{false};
};

typedef std::vector<ftp_file_info> ftp_file_info_list;

// 分析ftp文件信息的回调函数, get_ftp_info需要使用
typedef void (*ftp_file_info_callback)(const std::string& line, ftp_file_info& info);

class ftp_utils
{
  public:
    /// <summary>
    /// 获取ftp路径下的文件(夹)信息
    /// </summary>
    /// <param name="url"></param>
    /// <param name="user"></param>
    /// <param name="password"></param>
    /// <returns></returns>
    static ftp_file_info_list get_ftp_info(const std::string& url, const std::string& user, const std::string& password, ftp_file_info_callback cbfunc = nullptr);

    /// <summary>
    /// ftp下载文件
    /// </summary>
    /// <param name="remote_file_path"></param>
    /// <param name="local_file_path"></param>
    /// <param name="user"></param>
    /// <param name="passwd"></param>
    /// <param name="timeout"></param>
    /// <returns></returns>
    static bool ftp_download(const std::string& remote_file_path, const std::string& local_file_path, const std::string& user, const std::string& passwd, int timeout);

    /// <summary>
    /// ftp上传文件
    /// </summary>
    /// <param name="localFileFath"></param>
    /// <param name="remote_file_path"></param>
    /// <param name="user"></param>
    /// <param name="passwd"></param>
    /// <param name="timeout"></param>
    /// <param name="IsDir"></param>
    /// <returns></returns>
    static bool ftp_upload(const std::string& localFileFath, const std::string& remote_file_path, const std::string& user, const std::string& passwd, int timeout, bool IsDir);
};

#endif  // SILLY_UTILS_FTP_UTILS_H
