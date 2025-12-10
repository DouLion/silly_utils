/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/18 12:00
 * @version: 1.0.1
 * @description: ini文件解析 基类
 */
#ifndef SILLY_INI_PARSER_BASE_H
#define SILLY_INI_PARSER_BASE_H

#include <files/silly_file.h>

class suIniBase
{
  public:
    virtual ~suIniBase() = default;

    /**
     * 指定文件路径,可以完全创建一个新的
     * @param file
     */
    void open(const suPath& file)
    {
        m_file = file;
    };
    /**
     * 从文件加载解析对象,所有内容都区分大小写
     * @return
     */
    virtual bool read() = 0;

    /**
     * 修改后手动进行保存
     * @return
     */
    virtual bool write() const = 0;

    /**
     * 写入一个配置内容,可以新增加,也可以覆盖写
     * @param section
     * @param property
     * @param value
     * @param comment
     * @return
     */
    virtual bool set(const std::string& section, const std::string& property, const std::string& value, const std::string& comment = "") = 0;

    /**
     * 读取为int
     * @param section
     * @param property
     * @return
     */
    virtual int get_int(const std::string& section, const std::string& property) const = 0;

    /**
     * 读取bool 值类型, 0, 1, true false的大小写
     * @param section
     * @param property
     * @return
     */
    virtual bool get_bool(const std::string& section, const std::string& property) const = 0;

    /**
     * 读取为float类型
     * @param section
     * @param property
     * @return
     */
    virtual float get_float(const std::string& section, const std::string& property) const = 0;

    /**
     * 读取为double类型
     * @param section
     * @param property
     * @return
     */
    virtual double get_double(const std::string& section, const std::string& property) const = 0;

    /**
     * 读取一个长整型
     * @param section
     * @param property
     * @return
     */
    virtual long get_long(const std::string& section, const std::string& property) const = 0;

    /**
     * 默认读取一个字符串
     * @param section
     * @param property
     * @return
     */
    virtual std::string get(const std::string& section, const std::string& property) const = 0;

  protected:
    supath m_file;
    // 写入锁
    mutable std::mutex m_write_mtx;
};

#endif  // SILLY_INI_PARSER_BASE_H
