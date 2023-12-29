//
// Created by dell on 2023/12/18.
//

#include "silly_boost_ini_parser.h"
#include "boost/property_tree/ini_parser.hpp"
#include "su_marco.h"



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

		SU_ERROR_PRINT("Error:%s \n ", e.message().c_str());
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
		SU_ERROR_PRINT("Error:%s \n ", e.message().c_str());
		SU_ERROR_PRINT("Error:%s \n ", e.what());
		return false;
	}
}

bool silly_boost_ini_parser::write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment)
{
	try
	{
		auto pu = pt_tree.put(section + "." + property, value);

		return true;
	}
	catch (const boost::property_tree::ini_parser_error& e)
	{
		SU_ERROR_PRINT("Error:%s \n ", e.message().c_str());
		SU_ERROR_PRINT("Error:%s \n ", e.what());
		return false;
	}
}

int silly_boost_ini_parser::read_int(const std::string& section, const std::string& property)
{
	int value = pt_tree.get<int>(section + "." + property, 0);
	return value;
}

bool silly_boost_ini_parser::read_bool(const std::string& section, const std::string& property)
{
	bool value = pt_tree.get<bool>(section + "." + property, false);
	return value;
}

float silly_boost_ini_parser::read_float(const std::string& section, const std::string& property)
{
	float value = pt_tree.get<float>(section + "." + property, 0.0f);
	return value;
}

double silly_boost_ini_parser::read_double(const std::string& section, const std::string& property)
{
	double value = pt_tree.get<double>(section + "." + property, 0.0);
	return value;
}

long silly_boost_ini_parser::read_long(const std::string& section, const std::string& property)
{
	long value = pt_tree.get<long>(section + "." + property, 0);
	return value;
}

std::string silly_boost_ini_parser::read(const std::string& section, const std::string& property)
{

	//boost::optional<boost::property_tree::ptree&> node = pt_tree.get_child_optional(section);
	//if (node) 
	//{
	//	boost::optional<std::string> value = node->get_optional<std::string>(property);
	//	if (value) 
	//	{
	//		std::cout << "Value: " << *value << std::endl;
	//	}
	//	else {
	//		std::cout << "Key not found: " << property << std::endl;
	//	}

	//}
	//else 
	//{
	//	std::cout << "Section not found: " << section << std::endl;
	//}

	std::string seletc = section + "." + property ;
	std::string value = pt_tree.get<std::string>(seletc, "");
	return value;
}
