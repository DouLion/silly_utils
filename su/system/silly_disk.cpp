//
// Created by dell on 2024/3/11.
//

#include "silly_disk.h"
#include <files/silly_file.h>

double suDisk::space_capacity(const suPath& file, const eStorageUint& unit)
{
    double ret = 0;
    auto path = suPath(file);
    try
    {
        // 使用filesystem::space函数获取磁盘空间信息
        std::filesystem::space_info si = std::filesystem::space(path.path());

        ret = si.capacity;

        ret = ret / std::pow(1024.0, static_cast<double>(unit));
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return ret;
}

double suDisk::space_available(const suPath& file, const eStorageUint& unit)
{
    double ret = 0;
    auto path = suPath(file);
    try
    {
        // 使用filesystem::space函数获取磁盘空间信息
        std::filesystem::space_info si = std::filesystem::space(path.path());

        ret = si.available;
        ret = ret / std::pow(1024.0, static_cast<double>(unit));
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}

double suDisk::space_used(const suPath& file, const eStorageUint& unit)
{
    auto path = suPath(file);
    return space_capacity(path, unit) - space_available(path, unit);
}

double suDisk::space_available_percent(const suPath& file)
{
    double ret = 0;
    auto path = suPath(file);
    try
    {
        // 使用filesystem::space函数获取磁盘空间信息
        std::filesystem::space_info si = std::filesystem::space(path.path());

        ret = static_cast<double>(si.available) / static_cast<double>(si.capacity);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}

double suDisk::space_used_percent(const suPath& file)
{
    double ret = 0;
    auto path = suPath(file);
    try
    {
        // 使用filesystem::space函数获取磁盘空间信息
        std::filesystem::space_info si = std::filesystem::space(path.path());

        ret = 1.0 - static_cast<double>(si.available) / static_cast<double>(si.capacity);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return ret;
}
