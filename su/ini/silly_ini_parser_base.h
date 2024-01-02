/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/18 12:00
 * @version: 1.0.1
 * @description: ini文件解析 基类
 */
#ifndef SILLY_UTILS_SILLY_INI_PARSER_BASE_H
#define SILLY_UTILS_SILLY_INI_PARSER_BASE_H

#include <iostream>

class silly_ini_parser
{
public:
	/// <summary>
	/// 从文件加载解析对象,所有内容都区分大小写
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	virtual bool load(const std::string& path) = 0;
	
	/// <summary>
	/// 修改后手动进行保存
	/// </summary>
	/// <returns></returns>
	virtual bool save() = 0;

	/// <summary>
	/// 写入一个配置内容,可以新增加,也可以覆盖写
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <param name="value"></param>
	/// <param name="comment">注释内容</param>
	/// <returns></returns>
	virtual bool write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment = "") = 0;

	/// <summary>
	/// 读取int
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual int read_int(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// 读取bool 值类型, 任何true , 1 等  不区分大小写 都认为是true
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual bool read_bool(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// 读取float类型
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual float read_float(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// 读取double类型
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual double read_double(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// 读取一个长整型
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual long read_long(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// 默认读取一个字符串
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual std::string read(const std::string& section, const std::string& property) = 0;
protected:
	std::string m_path;
};

#endif //SILLY_UTILS_SILLY_INI_PARSER_BASE_H
