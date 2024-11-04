/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_sqlite3.h
 * @description: sqlite3 的一些常用用法
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SQLITE3_H
#define SILLY_UTILS_SILLY_SQLITE3_H
#include <sqlite3.h>
#include <su_marco.h>
class silly_sqlite3
{
  public:
    silly_sqlite3() = default;
    ~silly_sqlite3();
    /// 默认只在内存中打开
    bool open(const std::string& file = "：memory：");

    sqlite3* db();
    std::string err();
    /// 备份到另一个数据库文件
    bool backup(const std::string& file);

   void close();

  private:
    sqlite3* m_db;
    std::string m_err;
    std::string m_file;
};

#endif  // SILLY_UTILS_SILLY_SQLITE3_H
