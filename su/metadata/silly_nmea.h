/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-13
 * @file: silly_nmea.h
 * @description:  GPGSA GPS数据传输标准
 * @version: v1.0.1 2024-12-13 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_NMEA_H
#define SILLY_UTILS_SILLY_NMEA_H
#include <su_marco.h>
#define GNGGA "GNGGA"
#define GNGSA "GNGSA"
#define GNGLL "GNGLL"
#define GNRMC "GNRMC"
#define GNVTG "GNVTG"
#define GPGSV "GPGSV"
#define GLGSV "GLGSV"
#define GBGSV "GBGSV"
#define GAGSV "GAGSV"
#define GQGSV "GQGSV"
namespace silly
{
namespace meta
{
class nmea
{
  public:
    nmea();
    nmea(const std::string& file);
    ~nmea();

    /// <summary>
    /// NMEA时间转秒
    /// </summary>
    /// <param name="time"></param>
    /// <returns></returns>
    static int64_t seconds(const std::string& time);

    /// <summary>
    /// NMEA经纬度转百分制经纬度
    /// </summary>
    /// <param name="degree"></param>
    /// <returns></returns>
    static double degree(const std::string& degree);


    /// <summary>
    ///
    /// </summary>
    /// <param name="file"></param>
    /// <returns></returns>
    bool open(const std::string& file);

    /// <summary>
    /// 读取指定类型的NMEA数据
    /// </summary>
    /// <param name="type">参照 GNGGA GNGSA 等, 使用宏定义避免拼写错误 </param>
    /// <param name="offset">跳过n条数据</param>
    /// <param name="size">最多读取n条数据</param>
    /// <returns></returns>
    std::vector<std::string> read(const std::string& type, const size_t& offset = 0, const size_t& size = SIZE_MAX);

    bool close();

  private:
    bool match(const std::string& str, const std::string& base);

  private:
    std::string m_path;
};
}  // namespace meta
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_NMEA_H
