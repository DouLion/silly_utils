/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/18 12:00
 * @version: 1.0.1
 * @description: boost 模块实现的INI文件读写工具
 */
#ifndef SILLY_UTILS_SILLY_BOOST_INI_PARSER_H
#define SILLY_UTILS_SILLY_BOOST_INI_PARSER_H

#ifdef SILLY_UTILS_BOOST_ENABLE
#include <ini/silly_ini_parser_base.h>
#include "boost/property_tree/ptree.hpp"

#include <iostream>
#include <map>
#include <string>

/// <summary>
/// boost库读取ini文件文件中不能存在重复的section(节)名称,如果有重复节名称会报错,无法处理
/// 暂不支持添加注释,且保存时不会保存源文件中的注释
/// </summary>
class silly_boost_ini_parser : public silly_ini_parser
{
  public:
    /// <summary>
    /// 从文件加载解析对象,所有内容都区分大小写
    /// </summary>
    /// <param name="path">文件路径</param>
    /// <returns></returns>
    bool load(const std::string& path) override;

    /// <summary>
    /// 修改后手动进行保存
    /// </summary>
    /// <returns></returns>
    bool save() override;

    /// <summary>
    /// 写入一个配置内容,可以新增加,也可以覆盖写
    /// </summary>
    /// <param name="section"></param>
    /// <param name="property"></param>
    /// <param name="value"></param>
    /// <param name="comment">注释内容</param>
    /// <returns></returns>
    bool write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment = "") override;

    /// <summary>
    /// 读取int
    /// </summary>
    /// <param name="section"></param>
    /// <param name="property"></param>
    /// <returns></returns>
    int read_int(const std::string& section, const std::string& property) override;

    /// <summary>
    /// 读取bool 值类型, 任何true , 1 等  不区分大小写 都认为是true
    /// </summary>
    /// <param name="section"></param>
    /// <param name="property"></param>
    /// <returns></returns>
    bool read_bool(const std::string& section, const std::string& property) override;

    /// <summary>
    /// 读取float类型
    /// </summary>
    /// <param name="section"></param>
    /// <param name="property"></param>
    /// <returns></returns>
    float read_float(const std::string& section, const std::string& property) override;

    /// <summary>
    /// 读取double类型
    /// </summary>
    /// <param name="section"></param>
    /// <param name="property"></param>
    /// <returns></returns>
    double read_double(const std::string& section, const std::string& property) override;

    /// <summary>
    /// 读取一个long整型
    /// </summary>
    /// <param name="section"></param>
    /// <param name="property"></param>
    /// <returns></returns>
    long read_long(const std::string& section, const std::string& property) override;

    /// <summary>
    /// 默认读取一个字符串
    /// </summary>
    /// <param name="section"></param>
    /// <param name="property"></param>
    /// <returns></returns>
    std::string read(const std::string& section, const std::string& property) override;

  private:
    boost::property_tree::ptree m_tree;
};
#endif
#endif  // SILLY_UTILS_SILLY_BOOST_INI_PARSER_H
