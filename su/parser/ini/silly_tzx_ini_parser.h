/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/18 12:03
 * @version: 1.0.1
 * @description:
 */
#ifndef SILLY_TZX_INI_PARSER_H
#define SILLY_TZX_INI_PARSER_H

#include <files/silly_file.h>

class TzxIni
{
  public:
    TzxIni() = default;
    ~TzxIni() = default;
    bool read();
    bool write(const bool& gbk=false) const;
    bool set(const std::string& section, const std::string& property, const std::string& value, const std::string& comment = "");
    bool remove(const std::string& section, const std::string& property);
    double numeric(const std::string& section, const std::string& property) const;
    std::string string(const std::string& section, const std::string& property) const;

  private:
    supath m_file;
    // 写入锁
    mutable std::mutex m_write_mtx;
    std::map<int, std::string> m_ln2section;
    std::map<int, std::string> m_ln2property;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_sect2prop2value;
    std::map<size_t, std::string> m_ln2comment; // 行数对应的注释
};

#endif  // SILLY_TZX_INI_PARSER_H
