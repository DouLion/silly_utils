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
#ifndef SILLY_SQLITE3_H
#define SILLY_SQLITE3_H
#include <sqlite3.h>
#include <files/silly_file.h>

// WAL 模式, 使用视图隐式删除列
// 纯内存模式
/**
 * 多次更新后
 VACUUM;
重建整个数据库：按主键顺序重写所有表，消除碎片
压缩文件到最小
重建所有索引（包括空间索引！）
 */
enum class eSQLite3Type : uint8_t
{
    INVALID = 0,
    AUTOINC,  // 自增列
    INTEGER,  // 亲和类型 "INT", "INTEGER", "TINYINT"
    REAL,     // 亲和类型 "REAL", "FLOAT", "DOUBLE"
    TEXT,     // 亲和类型 "TEXT", "CHAR", "CLOB"
    BLOB,

};

class suSQLite3
{
  public:
    struct Table
    {
        std::string name;
        // 列名, 列类型, 是否主键, 是否自增
        std::vector<std::tuple<std::string, eSQLite3Type, bool>> cols;
        std::map<std::string, bool> visables;  // 列是否可见,用于伪删除
        /**
         * 添加列
         * @param colnm 列名
         * @param type 列类型
         * @param primary 是否主键
         */
        Table& AddCol(const std::string& colnm, const eSQLite3Type& type, const bool& primary = false);
        Table& AddCol(const std::string& colnm, const std::string& type, const bool& primary = false);
        std::string ViewName() const;
        /**
         * 生成建表语句
         */
        std::string GenDLLSQL() const;

        /**
         * 生成视图语句,默认会用 V12W_+table_name
         */
        std::string GenViewSQL() const;

        std::string GenDropSQL() const;
    };

    /**
     * 纯内存模式
     * @return
     */
    bool Memory();

    /**
     * 打开数据库文件
     * @param file 数据库文件
     * @return
     */
    bool Open(const suPath& file);

    /**
     * 创建数据库表
     * @param tbnm 表名
     * @param tbl 表结构
     * @param cover 是否覆盖
     * @return
     */
    bool CreateTable(const std::string& tbnm, const Table& tbl, const bool& cover = false);

    /**
     * 获取数据库表信息
     * @param tbnm 表名
     * @return
     */
    Table GetTable(const std::string& tbnm) const;

    /**
     * vacuum
     * 重建整个数据库：按主键顺序重写所有表，消除碎片
     * 压缩文件到最小
     * 重建所有索引（包括空间索引！）
     * @return
     */
    bool VACUUM() const;

    sqlite3* DB() const;
    std::string Err() const;

    /**
     * 数据库备份
     * @param file 备份文件
     * @return
     */
    bool Backup(const suPath& file) const;

    void Close();

  private:
  protected:
    bool m_mem = false;
    sqlite3* m_db = nullptr;
    std::string m_err;
    suPath m_file;
    std::map<std::string, Table> m_table2dll;
};

#endif  // SILLY_SQLITE3_H
