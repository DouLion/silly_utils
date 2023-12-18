//
// Created by dell on 2023/12/18.
//

#include "silly_boost_ini_parser.h"

bool silly_boost_ini_parser::load(const std::string& path)
{
	return false;
}

bool silly_boost_ini_parser::save()
{
	return false;
}

bool silly_boost_ini_parser::write(const std::string& section, const std::string& property, const std::string& value, const std::string& comment)
{
	return false;
}

int silly_boost_ini_parser::read_int(const std::string& section, const std::string& property)
{
	return 0;
}

bool silly_boost_ini_parser::read_bool(const std::string& section, const std::string& property)
{
	return false;
}

float silly_boost_ini_parser::read_float(const std::string& section, const std::string& property)
{
	return 0.0f;
}

double silly_boost_ini_parser::read_double(const std::string& section, const std::string& property)
{
	return 0.0;
}

long silly_boost_ini_parser::read_long(const std::string& section, const std::string& property)
{
	return 0;
}

std::string silly_boost_ini_parser::read(const std::string& section, const std::string& property)
{
	return std::string();
}
