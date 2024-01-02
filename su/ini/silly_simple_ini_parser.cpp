//
// Created by dell on 2023/12/18.
//

#include "silly_simple_ini_parser.h"

bool silly_simple_ini_parser::load(const std::string& path)
{
	bool status = false;
	std::unique_lock<std::mutex> lock(m_write_mtx);
	if (m_simple_ini.LoadFile(path.c_str()) == SI_OK)
	{
		m_path = path;
		status = true;
	}
	return status;
}

bool silly_simple_ini_parser::save()
{
	std::unique_lock<std::mutex> lock(m_write_mtx);
	bool status = false;
	if (m_simple_ini.SaveFile(m_path.c_str(), false) == SI_OK)
	{
		status = true;
	}
	return status; 
}

bool silly_simple_ini_parser::write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment)
{
	std::unique_lock<std::mutex> lock(m_write_mtx);
	bool status = false;
	if (!comment.empty())
	{
		// 添加注释尽在从新创建是可添加,如果有注释就先删除然后再新建
		const char* value = m_simple_ini.GetValue(section.c_str(), property.c_str());
		int is_del = m_simple_ini.DeleteValue(section.c_str(), property.c_str(), value);
	}
	int is_set;
	if (comment.empty())
	{
		is_set = m_simple_ini.SetValue(section.c_str(), property.c_str(), value.c_str());
	}
	else
	{
		is_set = m_simple_ini.SetValue(section.c_str(), property.c_str(), value.c_str(), comment.c_str());
	}

	if (is_set == SI_UPDATED || is_set == SI_INSERTED) // 更新成或插入成功
	{
		status = true;
	}
	return status;
}

int silly_simple_ini_parser::read_int(const std::string& section, const std::string& property)
{
	std::unique_lock<std::mutex> lock(m_write_mtx);
	int value = static_cast<int>(m_simple_ini.GetLongValue(section.c_str(), property.c_str()));
	return value;
}

bool silly_simple_ini_parser::read_bool(const std::string& section, const std::string& property)
{
	std::unique_lock<std::mutex> lock(m_write_mtx);
	bool value = m_simple_ini.GetBoolValue(section.c_str(), property.c_str());
	return value;
}

float silly_simple_ini_parser::read_float(const std::string& section, const std::string& property)
{
	std::unique_lock<std::mutex> lock(m_write_mtx);
	float value = static_cast<float>(m_simple_ini.GetDoubleValue(section.c_str(), property.c_str()));
	return value;
}

double silly_simple_ini_parser::read_double(const std::string& section, const std::string& property)
{
	std::unique_lock<std::mutex> lock(m_write_mtx);
	double value = m_simple_ini.GetDoubleValue(section.c_str(), property.c_str());
	return value;
}

long silly_simple_ini_parser::read_long(const std::string& section, const std::string& property)
{
	std::unique_lock<std::mutex> lock(m_write_mtx);
	long value = m_simple_ini.GetLongValue(section.c_str(), property.c_str());
	return value;
}

std::string silly_simple_ini_parser::read(const std::string& section, const std::string& property)
{
	std::unique_lock<std::mutex> lock(m_write_mtx);
	std::string res;
	const char* value = m_simple_ini.GetValue(section.c_str(), property.c_str());
	if (value)
	{
		res = value;
	}
	return res;
}
