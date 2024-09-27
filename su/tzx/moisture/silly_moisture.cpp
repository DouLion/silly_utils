/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_moisture.cpp
 * @description: silly_moisture实现
 * @version: v1.0.1 2024-09-11 dou li yang
 */
#include "silly_moisture.h"
#include <files/silly_file.h>
std::string soil_moisture_record::serialize() const
{
    std::string result;

    result.resize(serialized_size);
    result[0] = 'S';
    int index = 1;
    memcpy(&result[index], &pid, sizeof(pid));
    index += sizeof(pid);
    memcpy(&result[index], &moisture, sizeof(moisture));
    index += sizeof(moisture);
    memcpy(&result[index], &moisture_percent, sizeof(moisture_percent));
    index += sizeof(moisture_percent);
    memcpy(&result[index], &precipitation, sizeof(precipitation));

    return result;
}
bool soil_moisture_record::deserialize(const std::string& data)
{
    if (data.size() != serialized_size || data[0] != 'S')
    {
        return false;
    }

    int index = 1;
    memcpy(&pid, &data[index], sizeof(pid));
    index += sizeof(pid);
    memcpy(&moisture, &data[index], sizeof(moisture));
    index += sizeof(moisture);
    memcpy(&moisture_percent, &data[index], sizeof(moisture_percent));
    index += sizeof(moisture_percent);
    memcpy(&precipitation, &data[index], sizeof(precipitation));
    return true;
}
void silly_moisture::serialize(const std::string& file, const std::vector<soil_moisture_record>& records)
{
    std::string result;
    result.resize(4);
    int len = 0;
    for (auto& record : records)
    {
        std::string tmp = record.serialize();
        if (tmp.size() == soil_moisture_record::serialized_size)
        {
            assert(!tmp.empty());
            result.append(tmp);
            len++;
        }
    }

    if (len * soil_moisture_record::serialized_size + 4 == result.size())
    {
        memcpy(&result[0], &len, sizeof(len));
        silly_file::write(file, result);
    }
    else
    {
        throw std::runtime_error("serialize_by_time error");
    }
}
void silly_moisture::deserialize(const std::string& file, std::vector<soil_moisture_record>& records)
{
    std::string data;
    if (0 == silly_file::read(file, data))
    {
        return;
    }

    int len = 0;
    memcpy(&len, &data[0], sizeof(len));
    if (len * soil_moisture_record::serialized_size + 4 != data.size())
    {
        return;
    }
    for (int i = 0; i < len; i++)
    {
        soil_moisture_record record;
        record.deserialize(data.substr(4 + i * soil_moisture_record::serialized_size, soil_moisture_record::serialized_size));
        records.push_back(record);
    }
}
bool silly_moisture::deserialize(const std::string& file, const moisture_index_cache& cache, const int& pid, soil_moisture_record& record)
{
    auto iter = cache.find(pid);
    if (iter != cache.end())
    {
        size_t offset = iter->second.index;
        std::string content;
        silly_file::read(file, content, offset, soil_moisture_record::serialized_size);
        record.deserialize(content);
        if (record.pid == pid)
        {
            return true;
        }
    }
    // 缓存中没有
    std::cerr << "pid:" << pid << " not found" << std::endl;
    std::string data;
    if (0 == silly_file::read(file, data))
    {
        return false;
    }

    int len = 0;
    memcpy(&len, &data[0], sizeof(len));
    if (len * soil_moisture_record::serialized_size + 4 != data.size())
    {
        return false;
    }
    for (int i = 0; i < len; i++)
    {
        std::string content = data.substr(4 + i * soil_moisture_record::serialized_size, soil_moisture_record::serialized_size);
        // soil_moisture_record record;
        // record.deserialize();
        if (content.size() == soil_moisture_record::serialized_size && ((int*)(content.data() + 1))[0] == pid)
        {
            record.deserialize(content);
            return true;
        }
    }

    return false;
}

bool silly_moisture_index::read(const std::string& file)
{
    std::string content;
    silly_file::read(file, content);
    // assert();
    if (0 != content.size() % sizeof(moisture_index_info))
    {
        return false;
    }
    int num = content.size() / sizeof(moisture_index_info);
    for (int i = 0; i < num; i++)
    {
        moisture_index_info mi;
        memcpy(&mi, content.data() + i * sizeof(moisture_index_info), sizeof(moisture_index_info));
        cache[mi.pid] = mi;
    }

    return !cache.empty();
}

bool silly_moisture_index::write(const std::string& file, const moisture_index_cache& cache)
{
    std::string out;
    for (auto [_, mi] : cache)
    {
        std::string tmp;
        tmp.resize(sizeof(moisture_index_info));
        memcpy(tmp.data(), &mi, sizeof(moisture_index_info));
        out.append(tmp);
    }

    if (out.size() == cache.size() * sizeof(moisture_index_info))
    {
        silly_file::write(file, out);
        return std::filesystem::exists(file);
    }

    return false;
}
