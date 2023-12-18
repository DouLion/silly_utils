/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/18 12:00
 * @version: 1.0.1
 * @description: ini�ļ����� ����
 */
#ifndef SILLY_UTILS_SILLY_INI_PARSER_BASE_H
#define SILLY_UTILS_SILLY_INI_PARSER_BASE_H

#include <iostream>

class silly_ini_parser
{
public:
	/// <summary>
	/// ���ļ����ؽ�������,�������ݶ����ִ�Сд
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	virtual bool load(const std::string& path) = 0;
	
	/// <summary>
	/// �޸ĺ��ֶ����б���
	/// </summary>
	/// <returns></returns>
	virtual bool save() = 0;

	/// <summary>
	/// д��һ����������,����������,Ҳ���Ը���д
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <param name="value"></param>
	/// <param name="comment">ע������</param>
	/// <returns></returns>
	virtual bool write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment = "") = 0;

	/// <summary>
	/// ��ȡint
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual int read_int(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// ��ȡbool ֵ����, �κ�true , 1 ��  �����ִ�Сд ����Ϊ��true
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual bool read_bool(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// ��ȡfloat����
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual float read_float(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// ��ȡdouble����
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual double read_double(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// ��ȡһ��������
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual long read_long(const std::string& section, const std::string& property) = 0;

	/// <summary>
	/// Ĭ�϶�ȡһ���ַ���
	/// </summary>
	/// <param name="section"></param>
	/// <param name="property"></param>
	/// <returns></returns>
	virtual std::string read(const std::string& section, const std::string& property) = 0;
};

#endif //SILLY_UTILS_SILLY_INI_PARSER_BASE_H
