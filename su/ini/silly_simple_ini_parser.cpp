//
// Created by dell on 2023/12/18.
//

#include "silly_simple_ini_parser.h"

bool silly_simple_ini_parser::load(const std::string& path)
{
	if (ini.LoadFile(path.c_str()) == SI_OK)
	{
		faile_path = path;
		return true;
	}
	return false;
}

bool silly_simple_ini_parser::save()
{
	bool status = false;
	if (ini.SaveFile(faile_path.c_str(), false) == SI_OK)
	{
		status = true;
	}
	return status; 
}

bool silly_simple_ini_parser::write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment)
{
	bool status = false;
	if (!comment.empty())
	{
		// 添加注释尽在从新创建是可添加,如果有注释就先删除然后再新建
		const char* value = ini.GetValue(section.c_str(), property.c_str());
		int is_del = ini.DeleteValue(section.c_str(), property.c_str(), value);
		int a = 0;
	}
	int is_set;
	if (comment.empty())
	{
		is_set = ini.SetValue(section.c_str(), property.c_str(), value.c_str());
	}
	else
	{
		is_set = ini.SetValue(section.c_str(), property.c_str(), value.c_str(), comment.c_str());
	}

	if (is_set == SI_UPDATED || is_set == SI_INSERTED) // 更新成或插入成功
	{
		status = true;
	}
	return status;
}

int silly_simple_ini_parser::read_int(const std::string& section, const std::string& property)
{
	int value = static_cast<int>(ini.GetLongValue(section.c_str(), property.c_str()));
	return value;
}

bool silly_simple_ini_parser::read_bool(const std::string& section, const std::string& property)
{
	bool value = ini.GetBoolValue(section.c_str(), property.c_str());
	return value;
}

float silly_simple_ini_parser::read_float(const std::string& section, const std::string& property)
{
	float value = static_cast<float>(ini.GetDoubleValue(section.c_str(), property.c_str()));
	return value;
}

double silly_simple_ini_parser::read_double(const std::string& section, const std::string& property)
{
	double value = ini.GetDoubleValue(section.c_str(), property.c_str());
	return value;
}

long silly_simple_ini_parser::read_long(const std::string& section, const std::string& property)
{
	long value = ini.GetLongValue(section.c_str(), property.c_str());
	return value;
}

std::string silly_simple_ini_parser::read(const std::string& section, const std::string& property)
{
	const char* value = ini.GetValue(section.c_str(), property.c_str());
	if (value)
	{
		return value;
	}
	return "";
}
