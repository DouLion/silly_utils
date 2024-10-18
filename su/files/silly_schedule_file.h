
/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-18
 * @file: silly_schedule_file.h
 * @description: 定时生成的文件
 * @version: v1.0.1 2024-10-18 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SCHEDULE_FILE_H
#define SILLY_UTILS_SILLY_SCHEDULE_FILE_H
#include <files/silly_file.h>
#include <datetime/silly_posix_time.h>

class silly_schedule_file
{
  public:
    silly_schedule_file() = default;
    ~silly_schedule_file() = default;
    silly_schedule_file(const std::string& root, const std::string& ext);

    // 复制自己,仅包含root format extension
    silly_schedule_file copy() const;

    /// setter
    void root(const std::string& r);
    void extension(const std::string& e);
    void format(const std::string& r);

    /// 文件名
    std::string filename(const std::string& tm);
    std::string filename(const silly_posix_time& tm);
    std::string filename(const std::string& tm,const std::string& ptm);
    std::string filename(const silly_posix_time& tm,const silly_posix_time& ptm);


    // 创建自己写入需要的目录
    bool create_dir(const std::string& tm = "");
    bool create_dir(const silly_posix_time& tm);

    bool exist(const std::string& tm);
    bool exist(const silly_posix_time& tm);

    std::string path();



  private:
    std::string m_path; // 路径
    std::string m_filename;
    std::string m_root;
    std::string m_extension; // 文件后缀
    std::string m_format = DATE_FORMAT_2; // 日期格式
};

#endif  // SILLY_UTILS_SILLY_SCHEDULE_FILE_H
