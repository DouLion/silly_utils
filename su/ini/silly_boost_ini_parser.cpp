//
// Created by dell on 2023/12/18.
//

#include "silly_boost_ini_parser.h"
#include "boost/property_tree/ini_parser.hpp"
#include "su_marco.h"
#include <boost/algorithm/string.hpp>


bool silly_boost_ini_parser::load(const std::string& path)
{
	try 
	{
		file_path = path;
		boost::property_tree::ini_parser::read_ini(file_path, pt_tree);
		return true;
	}
	catch (const boost::property_tree::ini_parser_error& e) 
	{
		SU_ERROR_PRINT("Error:%s \n ", e.what());
		return false;
	}
}

bool silly_boost_ini_parser::save()
{
	try 
	{
		boost::property_tree::ini_parser::write_ini(file_path, pt_tree);
		return true;
	}
	catch (const boost::property_tree::ini_parser_error& e) 
	{
		SU_ERROR_PRINT("Error:%s \n ", e.what());
		return false;
	}
}

bool silly_boost_ini_parser::write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment)
{
	try
	{
		auto sectionIt = pt_tree.find(section);
		if (sectionIt != pt_tree.not_found())
		{
			auto& sectionNode = sectionIt->second;
			auto propertyIt = sectionNode.find(property);
			if (propertyIt != sectionNode.not_found())
			{
				// 修改属性
				propertyIt->second.data() = value;
			}
			else
			{
				// 添加属性
				sectionNode.put(property, value);
			}
		}
		else
		{
			// 添加节点
			// TODO:新建
		}

		//for (const auto& section : pt_tree)
		//{
		//	std::cout << "[" << section.first << "]\n";
		//	for (const auto& key : section.second)
		//	{
		//		std::cout << key.first << " = " << key.second.get_value<std::string>() << "\n";
		//	}
		//}
	}
	catch (const boost::property_tree::ini_parser_error& e)
	{
		SU_ERROR_PRINT("Error:%s \n ", e.what());
		return false;
	}
}

int silly_boost_ini_parser::read_int(const std::string& section, const std::string& property)
{
	try
	{
		boost::optional<int> value;
		auto sectionIt = pt_tree.find(section);
		if (sectionIt != pt_tree.not_found())
		{
			auto& sectionNode = sectionIt->second;
			auto propertyIt = sectionNode.find(property);
			if (propertyIt != sectionNode.not_found())
			{
				value = propertyIt->second.get_value_optional<int>();
			}
		}

		if (value)
		{
			return value.value();
		}
		else
		{
			return 0; // 返回默认值
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		SU_ERROR_PRINT("Error: %s\n", e.what());
		return 0; // 返回默认值
	}
}

bool silly_boost_ini_parser::read_bool(const std::string& section, const std::string& property)
{
	try
	{
		boost::optional<bool> value;
		auto sectionIt = pt_tree.find(section);
		if (sectionIt != pt_tree.not_found())
		{
			auto& sectionNode = sectionIt->second;
			auto propertyIt = sectionNode.find(property);
			if (propertyIt != sectionNode.not_found())
			{
				value = propertyIt->second.get_value_optional<bool>();
			}
		}

		if (value)
		{
			return value.value();
		}
		else
		{
			return false; // 返回默认值
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		SU_ERROR_PRINT("Error: %s\n", e.what());
		return false; // 返回默认值
	}
}

float silly_boost_ini_parser::read_float(const std::string& section, const std::string& property)
{
	try
	{
		boost::optional<float> value;
		auto sectionIt = pt_tree.find(section);
		if (sectionIt != pt_tree.not_found())
		{
			auto& sectionNode = sectionIt->second;
			auto propertyIt = sectionNode.find(property);
			if (propertyIt != sectionNode.not_found())
			{
				value = propertyIt->second.get_value_optional<float>();
			}
		}

		if (value)
		{
			return value.value();
		}
		else
		{
			return 0.0f; // 返回默认值
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		SU_ERROR_PRINT("Error: %s\n", e.what());
		return 0.0f; // 返回默认值
	}
}

double silly_boost_ini_parser::read_double(const std::string& section, const std::string& property)
{
	try
	{
		boost::optional<double> value;
		auto sectionIt = pt_tree.find(section);
		if (sectionIt != pt_tree.not_found())
		{
			auto& sectionNode = sectionIt->second;
			auto propertyIt = sectionNode.find(property);
			if (propertyIt != sectionNode.not_found())
			{
				value = propertyIt->second.get_value_optional<double>();
			}
		}

		if (value)
		{
			return value.value();
		}
		else
		{
			return 0.0; // 返回默认值
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		SU_ERROR_PRINT("Error: %s\n", e.what());
		return 0.0; // 返回默认值
	}
}

long silly_boost_ini_parser::read_long(const std::string& section, const std::string& property)
{
	try
	{
		boost::optional<long> value;
		auto sectionIt = pt_tree.find(section);
		if (sectionIt != pt_tree.not_found())
		{
			auto& sectionNode = sectionIt->second;
			auto propertyIt = sectionNode.find(property);
			if (propertyIt != sectionNode.not_found())
			{
				value = propertyIt->second.get_value_optional<long>();
			}
		}

		if (value)
		{
			return value.value();
		}
		else
		{
			return 0L; // 返回默认值
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		SU_ERROR_PRINT("Error: %s\n", e.what());
		return 0L; // 返回默认值
	}
}

std::string silly_boost_ini_parser::read(const std::string& section, const std::string& property)
{
	try
	{
		boost::optional<std::string> value;
		auto sectionIt = pt_tree.find(section);
		if (sectionIt != pt_tree.not_found())
		{
			auto& sectionNode = sectionIt->second;
			auto propertyIt = sectionNode.find(property);
			if (propertyIt != sectionNode.not_found())
			{
				value = propertyIt->second.get_value_optional<std::string>();
			}
		}

		if (value)
		{
			return value.value();
		}
		else
		{
			return "";
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		SU_ERROR_PRINT("Error: %s\n", e.what());
		return "";
	}

}
