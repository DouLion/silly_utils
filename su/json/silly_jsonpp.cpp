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

std::string silly_jsonpp::to_string(const Json::Value root)
{
	static Json::Value jv_def = []() {
		Json::Value jv_def;
		Json::StreamWriterBuilder::setDefaults(&jv_def);
		jv_def["emitUTF8"] = true;
		return jv_def;
	}();

	std::ostringstream stream;
	Json::StreamWriterBuilder stream_builder;
	stream_builder.settings_ = jv_def;//Config emitUTF8
	std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
	writer->write(root, &stream);
	return stream.str();
}
