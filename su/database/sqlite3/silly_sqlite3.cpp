/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-31
 * @file: silly_sqlite3.cpp
 * @description: silly_sqlite3实现
 * @version: v1.0.1 2024-10-31 dou li yang
 */
#include "silly_sqlite3.h"
silly_sqlite3::~silly_sqlite3()
{
    close();
}
bool silly_sqlite3::open(const std::string& file)
{
    if(m_db)
    {
       m_err = "数据库已经打开";
       throw std::runtime_error("已经打开的数据库: [" + m_file + "]");
    }
    int ret = sqlite3_open(file.c_str(), &m_db);
    if(ret == SQLITE_OK)
    {
        return true;
    }
    return false;
}
sqlite3* silly_sqlite3::db()
{
    return m_db;
}
void silly_sqlite3::close()
{
    if(m_db)
    {
        sqlite3_close(m_db);
        m_db = nullptr;
    }

}
std::string silly_sqlite3::err()
{
    return m_err;
}
bool silly_sqlite3::backup(const std::string& file)
{
    if(m_file == file)
    {
        m_err = "同一个数据库文件无法备份";
        return false;
    }

    // 将内存数据库备份到文件
    sqlite3 *backup_db;
    int rc = sqlite3_open(file.c_str(), &backup_db);
    if (rc) {
        std::cerr << "Can't open disk database: " << sqlite3_errmsg(backup_db) << std::endl;
        return false;
    }

    sqlite3_backup *pBackup = sqlite3_backup_init(backup_db, "main", m_db, "main");
    if (pBackup == NULL) {
        std::cerr << "Backup initialization failed: " << sqlite3_errmsg(backup_db) << std::endl;
        sqlite3_close(backup_db);
        return false;
    }

    while ((rc = sqlite3_backup_step(pBackup, 100)) == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
        // Wait or retry as necessary
        sqlite3_close(backup_db);
        return false;
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Backup step failed: " << sqlite3_errmsg(backup_db) << std::endl;
        return false;
    }

    sqlite3_backup_finish(pBackup);
    sqlite3_close(backup_db);
    return true;
}
