/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/12/18 12:02
 * @version: 1.0.1
 * @description: SimpleIni(https://github.com/brofield/simpleini) 模块实现的INI文件读写工具
 */
#ifndef SILLY_UTILS_SILLY_SIMPLE_INI_PARSER_H
#define SILLY_UTILS_SILLY_SIMPLE_INI_PARSER_H

#include <ini/silly_ini_parser_base.h>

class silly_simple_ini_parser : public silly_ini_parser
{
public:
	bool load(const std::string& path) override;

	bool save() override;

	bool write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment = "") override;

	int read_int(const std::string& section, const std::string& property) override;

	bool read_bool(const std::string& section, const std::string& property) override;

	float read_float(const std::string& section, const std::string& property) override;

	double read_double(const std::string& section, const std::string& property) override;

	long read_long(const std::string& section, const std::string& property) override;

	std::string read(const std::string& section, const std::string& property) override;

};

#endif //SILLY_UTILS_SILLY_SIMPLE_INI_PARSER_H
