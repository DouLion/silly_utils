/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024/3/11 11:25
 * @version: 1.0.1
 * @description: 单位换算基于1024进制
 */
#ifndef SILLY_DISK_H
#define SILLY_DISK_H
#include <files/silly_file.h>

enum class eStorageUint : uint8_t
{
    Byte = 0,       ///< 字节：2^0 = 1 字节 (基本存储单位)

    KiloByte = 10,  ///< 千字节：2^10 = 1024 字节 (二进制千字节，KiB)
    ///< 注意：不同于十进制 KB (1000字节)，这是计算机科学中的标准

    MegaByte = 20,  ///< 兆字节：2^20 = 1,048,576 字节 (二进制兆字节，MiB)
    ///< 常用于表示文件大小、内存容量等

    GigaByte = 30,  ///< 吉字节：2^30 = 1,073,741,824 字节 (二进制吉字节，GiB)
    ///< 常用于表示硬盘容量、RAM大小等

    TeraByte = 40,  ///< 太字节：2^40 = 1,099,511,627,776 字节 (二进制太字节，TiB)
    ///< 用于大型存储系统、数据中心容量计量

    PetaByte = 50,  ///< 拍字节：2^50 = 1,125,899,906,842,624 字节 (二进制拍字节，PiB)
    ///< 超大规模数据存储，如云存储服务

    ExaByte = 60,   ///< 艾字节：2^60 = 1,152,921,504,606,846,976 字节 (二进制艾字节，EiB)
    ///< 国家级或全球级数据规模计量

    ZettaByte = 70, ///< 泽字节：2^70 = 1,180,591,620,717,411,303,424 字节 (二进制泽字节，ZiB)
    ///< 理论上的极大存储规模，目前尚未实用化

    YottaByte = 80  ///< 尧字节：2^80 = 1,208,925,819,614,629,174,706,176 字节 (二进制尧字节，YiB)
    ///< 目前已知最大的十进制SI前缀，在二进制中更是巨大无比
};

class suDisk
{
  public:
    /// <summary>
    /// 磁盘总空间
    /// </summary>
    /// <param name="file"></param>
    /// <param name="unit"></param>
    /// <returns></returns>
    static double space_capacity(const suPath& file, const eStorageUint& unit = eStorageUint::Byte);

    /// <summary>
    /// 磁盘可用空间
    /// </summary>
    /// <param name="file"></param>
    /// <param name="unit"></param>
    /// <returns></returns>
    static double space_available(const suPath& file, const eStorageUint& unit = eStorageUint::Byte);

    /// <summary>
    /// 磁盘已用空间
    /// </summary>
    /// <param name="file"></param>
    /// <param name="unit"></param>
    /// <returns></returns>
    static double space_used(const suPath& file, const eStorageUint& unit = eStorageUint::Byte);

    /// <summary>
    /// 磁盘可用空间百分比
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static double space_available_percent(const suPath& file);

    /// <summary>
    /// 磁盘已用空间百分比
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    static double space_used_percent(const suPath& file);
};

#endif  // SILLY_DISK_H
