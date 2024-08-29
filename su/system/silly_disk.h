/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/3/11 11:25
 * @version: 1.0.1
 * @description: 单位换算基于1024进制
 */
#ifndef SILLY_UTILS_SILLY_DISK_H
#define SILLY_UTILS_SILLY_DISK_H
#include <su_marco.h>

enum class storage_unit
{
    esuByte = 0,
    esuKiloByte = 1,
    esuMegabits = 2,
    esuGigabits = 3,
    esuTerabyte = 4,
    esuPebibyte = 5,
    esuExbibyte = 6,
    esuZebibyte = 7,
    esuYobibyte = 8
};

class silly_disk
{
  public:
    /// <summary>
    /// 磁盘总空间
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static double space_capacity(const std::string& path, const storage_unit& unit = storage_unit::esuByte);

    /// <summary>
    /// 磁盘可用空间
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static double space_available(const std::string& path, const storage_unit& unit = storage_unit::esuByte);

    /// <summary>
    /// 磁盘已用空间
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    static double space_used(const std::string& path, const storage_unit& unit = storage_unit::esuByte);

    /// <summary>
    /// 磁盘可用空间百分比
    /// </summary>
    /// <param name="path"></param>
    /// <param name="unit"></param>
    /// <returns></returns>
    static double space_available_percent(const std::string& path);

    /// <summary>
    /// 磁盘已用空间百分比
    /// </summary>
    /// <param name="path"></param>
    /// <param name="unit"></param>
    /// <returns></returns>
    static double space_used_percent(const std::string& path);
};

#endif  // SILLY_UTILS_SILLY_DISK_H
