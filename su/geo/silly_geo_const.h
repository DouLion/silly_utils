/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-15
 * @file: silly_geo_const.h
 * @description: silly_geo_const 类声明
 * @version: v1.0.1 2025-01-15 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_GEO_CONST_H
#define SILLY_UTILS_SILLY_GEO_CONST_H
namespace silly
{
namespace geo
{
namespace earth_radius
{
constexpr double mean = 6371000.0;        // 平均半径 米
constexpr double equatorial = 6378137.0;  // 赤道半径 米
constexpr double polar = 6356752.3142;    // 极地半径 米
}  // namespace earth_radius
namespace wgs84
{
constexpr double semi_major_axis = 6378137.0;              // 长半轴 米
constexpr double inverse_flattening = 298.257223563;       // 扁率的倒数
constexpr double flattening = (1.0 / inverse_flattening);  // 扁率
constexpr double e2 = (2 * flattening - flattening * flattening);
}  // namespace wgs84
namespace cgcs2000
{

constexpr double semi_major_axis = 6378137.0;
constexpr double inverse_flattening = 298.257222101;
constexpr double flattening = (1.0 / inverse_flattening);
constexpr double e2 = (2 * flattening - flattening * flattening);
constexpr double gm = 3.986004418 * 1014;  // 地心引力常数 立方米/秒的平方
constexpr double w = 7.2921151467e-5;      // 自转角速度 弧度每秒
constexpr double j = 1.08262983226e-3;     // 重力场谐系数
}  // namespace cgcs2000
namespace xian80
{

constexpr double semi_major_axis = 6378140.0;              // 长半轴
constexpr double inverse_flattening = 298.257;             // 扁率的倒数
constexpr double flattening = (1.0 / inverse_flattening);  // 扁率
constexpr double e2 = (2 * flattening - flattening * flattening);
}  // namespace xian80
namespace beijing54
{

constexpr double semi_major_axis = 6378245.0;              // 长半轴
constexpr double inverse_flattening = 298.3;               // 扁率的倒数
constexpr double flattening = (1.0 / inverse_flattening);  // 扁率
constexpr double e2 = (2 * flattening - flattening * flattening);
}  // namespace beijing54

}  // namespace geo
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_GEO_CONST_H
