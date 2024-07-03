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
    stream_builder.settings_ = jv_def;  // Config emitUTF8
    std::unique_ptr<Json::StreamWriter> writer(stream_builder.newStreamWriter());
    writer->write(root, &stream);
    return stream.str();
}
void find_object(const Json::Value jv_obj, const std::string& key, const std::string& filter, std::vector<std::string>& arr);
void find_array(const Json::Value jv_arr, const std::string& key, const std::string& filter, std::vector<std::string>& arr);

void find_array(const Json::Value jv_arr, const std::string& key, const std::string& filter, std::vector<std::string>& arr)
{
    for (auto jv_tmp : jv_arr)
    {
        if (jv_tmp.isArray())
        {
            find_array(jv_tmp, key, filter, arr);
        }
        else if (jv_tmp.isObject())
        {
            find_object(jv_tmp, key, filter, arr);
        }
    }
}

void find_object(const Json::Value jv_obj, const std::string& key, const std::string& filter, std::vector<std::string>& arr)
{
    for (auto name : jv_obj.getMemberNames())
    {
        if (name == key)
        {
            if (jv_obj[name].isString())
            {
                std::string content = jv_obj[name].asString();
                if (filter.empty())
                {
                    arr.push_back(content);
                }
                else
                {
                    if (content.find(filter) < content.size())
                    {
                        arr.push_back(content);
                    }
                }
            }
        }
        if (jv_obj[name].isArray())
        {
            find_array(jv_obj[name], key, filter, arr);
        }
        else if (jv_obj[name].isObject())
        {
            find_object(jv_obj[name], key, filter, arr);
        }
    }
}

void silly_jsonpp::find_by_key(const std::string& json, const std::string& key, const std::string& filter, std::vector<std::string>& arr)
{
    Json::Value root = loads(json);
    find_by_key(root, key, filter, arr);
}

void silly_jsonpp::find_by_key(const Json::Value& root, const std::string& key, const std::string& filter, std::vector<std::string>& arr)
{
    if (root.isNull())
    {
        return;
    }
    if (root.isArray())
    {
        find_array(root, key, filter, arr);
    }
    else if (root.isObject())
    {
        find_object(root, key, filter, arr);
    }
}
bool silly_jsonpp::check_member_string(const Json::Value& root, const std::string& key, std::string& val)
{
    if(root.isMember(key))
    {
        if(root[key].isString())
        {
            val = root[key].asString();
            return true;
        }
    }
    return false;
}
bool silly_jsonpp::check_member_int(const Json::Value& root, const std::string& key, int& val)
{
    if (root.isMember(key))
    {
        if (root[key].isInt())
        {
            val = root[key].asInt();
            return true;
        }
        else
        {
            SFP_ERROR("字段 {} 不是int类型", key)
        }
    }
    else
    {
        SFP_ERROR("不存在字段 {}", key)
    }
    return false;
}
bool silly_jsonpp::check_member_double(const Json::Value& root, const std::string& key, double& val)
{
    if (root.isMember(key))
    {
        if (root[key].isDouble())
        {
            val = root[key].asDouble();
            return true;
        }
        else
        {
            SFP_ERROR("字段 {} 不是double类型", key)
        }
    }
    else
    {
        SFP_ERROR("不存在字段 {}", key)
    }
    return false;
}
bool silly_jsonpp::check_member_bool(const Json::Value& root, const std::string& key, bool& val)
{
    if (root.isMember(key))
    {
        if (root[key].isBool())
        {
            val = root[key].asBool();
            return true;
        }
        {
            SFP_ERROR("字段 {} 不是bool类型", key)
        }
    }
    else
    {
        SFP_ERROR("不存在字段 {}", key)
    }
    return false;
}
bool silly_jsonpp::check_member_array(const Json::Value& root, const std::string& key)
{
    if (root.isMember(key))
    {
        if (root[key].isArray())
        {
            return true;
        }
        else
        {
            SFP_ERROR("字段 {} 不是数组类型", key)
        }
    }
    else
    {
        SFP_ERROR("不存在字段 {}", key)
    }
    return false;
}
