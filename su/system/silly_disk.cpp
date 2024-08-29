//
// Created by dell on 2024/3/11.
//

#include "silly_disk.h"
#include <filesystem>
#include <cmath>

double silly_disk::space_capacity(const std::string& path, const storage_unit& unit)
{
    double ret = 0;
    try
    {
        // 使用filesystem::space函数获取磁盘空间信息
        std::filesystem::space_info si = std::filesystem::space(path);

        ret = si.capacity;

        ret = ret / std::pow(1024.0, static_cast<double>(unit));
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return ret;
}

double silly_disk::space_available(const std::string& path, const storage_unit& unit)
{
    double ret = 0;
    try
    {
        // 使用filesystem::space函数获取磁盘空间信息
        std::filesystem::space_info si = std::filesystem::space(path);

        ret = si.available;
        ret = ret / std::pow(1024.0, static_cast<double>(unit));
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}

double silly_disk::space_used(const std::string& path, const storage_unit& unit)
{
    return space_capacity(path, unit) - space_available(path, unit);
}

double silly_disk::space_available_percent(const std::string& path)
{
    double ret = 0;
    try
    {
        // 使用filesystem::space函数获取磁盘空间信息
        std::filesystem::space_info si = std::filesystem::space(path);

        ret = static_cast<double>(si.available) / static_cast<double>(si.capacity);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}

double silly_disk::space_used_percent(const std::string& path)
{
    double ret = 0;
    try
    {
        // 使用filesystem::space函数获取磁盘空间信息
        std::filesystem::space_info si = std::filesystem::space(path);

        ret = 1.0 - static_cast<double>(si.available) / static_cast<double>(si.capacity);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}
