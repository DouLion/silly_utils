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

std::string MoistureFile::Record::serialize() const
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
bool MoistureFile::Record::deserialize(const std::string& data)
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
void MoistureFile::serialize(const std::filesystem::path& file, const std::vector<MoistureFile::Record>& records)
{
    std::string result;
    // 前四个字节为
    result.resize(4);
    for (auto& record : records)
    {
        std::string tmp = record.serialize();
        if (tmp.size() == MoistureFile::Record::serialized_size)
        {
            assert(!tmp.empty());
            result.append(tmp);
            m_num++;
        }
    }

    if (m_num * MoistureFile::Record::serialized_size + 4 == result.size())
    {
        memcpy(&result[0], &m_num, sizeof(m_num));
        sufile::write(file, result);
    }
    else
    {
        throw std::runtime_error("序列化错误");
    }
}
void MoistureFile::deserialize(const std::filesystem::path& file, std::vector<MoistureFile::Record>& records)
{
    std::string data;
    if (0 == sufile::read(file, data))
    {
        return;
    }

    memcpy(&m_num, &data[0], sizeof(m_num));
    if (m_num * MoistureFile::Record::serialized_size + 4 != data.size())
    {
        return;
    }
    for (int i = 0; i < m_num; i++)
    {
        MoistureFile::Record record;
        record.deserialize(data.substr(4 + i * MoistureFile::Record::serialized_size, MoistureFile::Record::serialized_size));
        records.push_back(record);
    }
}
bool MoistureFile::deserialize(const std::filesystem::path& file, const MoistureIndex::Cache& cache, const int& pid, MoistureFile::Record& record)
{
    auto iter = cache.find(pid);
    if (iter != cache.end())
    {
        size_t offset = iter->second.index;
        std::string content;
        sufile::read(file, content, offset, MoistureFile::Record::serialized_size);
        record.deserialize(content);
        if (record.pid == pid)
        {
            return true;
        }
    }
    // 缓存中没有
    std::cerr << "pid:" << pid << " not found" << std::endl;
    std::string data;
    if (0 == sufile::read(file, data))
    {
        return false;
    }

    memcpy(&m_num, &data[0], sizeof(m_num));
    if (m_num * MoistureFile::Record::serialized_size + 4 != data.size())
    {
        return false;
    }
    for (int i = 0; i < m_num; i++)
    {
        std::string content = data.substr(4 + i * MoistureFile::Record::serialized_size, MoistureFile::Record::serialized_size);
        // MoistureFile::Record record;
        // record.deserialize();
        if (content.size() == MoistureFile::Record::serialized_size && ((int*)(content.data() + 1))[0] == pid)
        {
            record.deserialize(content);
            return true;
        }
    }

    return false;
}

bool MoistureIndex::read(const std::filesystem::path& file)
{
    std::string content;
    sufile::read(file, content);
    // assert();
    if (0 != content.size() % sizeof(MoistureIndex::Info))
    {
        return false;
    }
    int num = content.size() / sizeof(MoistureIndex::Info);
    for (int i = 0; i < num; i++)
    {
        MoistureIndex::Info mi;
        memcpy(&mi, content.data() + i * sizeof(MoistureIndex::Info), sizeof(MoistureIndex::Info));
        m_cache[mi.pid] = mi;
    }

    return !m_cache.empty();
}

bool MoistureIndex::write(const std::filesystem::path& file, const MoistureIndex::Cache& cache)
{
    std::string out;
    for (auto [_, mi] : cache)
    {
        std::string tmp;
        tmp.resize(sizeof(MoistureIndex::Info));
        memcpy(tmp.data(), &mi, sizeof(MoistureIndex::Info));
        out.append(tmp);
    }

    if (out.size() == cache.size() * sizeof(MoistureIndex::Info))
    {
        sufile::write(file, out);
        return std::filesystem::exists(file);
    }

    return false;
}
