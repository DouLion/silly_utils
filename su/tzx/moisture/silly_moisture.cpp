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
void silly_moisture::serialize_by_time(const std::string& file, std::vector<soil_moisture_record>& records)
{
    std::string result ="\0\0\0\0";
    int len = 0;
    for (auto& record : records)
    {
        std::string tmp = record.serialize();
        if(tmp.size() == soil_moisture_record::serialized_size)
        {
            result +=   tmp;
            len++;
        }
    }
    if(len * soil_moisture_record::serialized_size + 4 == result.size())
    {
        memcpy(&result[0], &len, sizeof(len));
        silly_file::write(file, result);
    }
    throw std::runtime_error("serialize_by_time error");
}
void silly_moisture::deserialize_by_time(const std::string& file, std::vector<soil_moisture_record>& records)
{
    std::string data;
    if(0 == silly_file::read(file, data))
    {
        return ;
    }

    int len = 0;
    memcpy(&len, &data[0], sizeof(len));
    if(len * soil_moisture_record::serialized_size + 4 != data.size())
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
