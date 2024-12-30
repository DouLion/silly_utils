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
// $GNGGA,030634.00,3415.26032290,N,11125.48176926,E,4,41,0.5,496.7253,M,-24.9553,M,1.0,3482*74
#define GNGSA "GNGSA"
// $GNGSA,M,3,10,25,26,28,31,32,,,,,,,0.9,0.5,0.8,1*32
#define GNGLL "GNGLL"
// $GNGLL,3415.26032290,N,11125.48176926,E,030634.00,A,D*7A
#define GNRMC "GNRMC"
// $GNRMC,030634.00,D,3415.26032290,N,11125.48176926,E,0.013,287.5,071224,4.0,W,R,S*54
#define GNVTG "GNVTG"
// $GNVTG,287.520,T,291.536,M,0.01325,N,0.02454,K,D*3A
#define GPGSV "GPGSV"
// $GPGSV,2,1,07,10,39,177,41,25,45,057,36,26,31,200,30,28,66,319,35,1*6E
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
