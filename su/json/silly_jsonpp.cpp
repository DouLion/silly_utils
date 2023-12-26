//
// Created by dell on 2023/12/26.
//

#include "silly_jsonpp.h"
#include <fstream>

Json::Value silly_jsonpp::loadf(const std::string& file)
{
	std::fstream input;
	input.open(file, std::ios::binary | std::ios::in);
	if (!input.is_open())
	{
		return Json::nullValue;
	}
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(input, root))
	{
		return root;
	}
	else
	{
		root = Json::nullValue;
	}
	input.close();
	return root;
}

Json::Value silly_jsonpp::loads(const std::string& content)
{
	Json::Reader reader;
	Json::Value root;

	if (reader.parse(content, root))
	{
		return root;
	}
	return Json::nullValue;
}
