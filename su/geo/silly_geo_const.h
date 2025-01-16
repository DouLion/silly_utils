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
namespace EarthRadius
{
constexpr double MEAN = 6371000.0;        // 平均半径 米
constexpr double EQUATORIAL = 6378137.0;  // 赤道半径 米
constexpr double POLAR = 6356752.3142;    // 极地半径 米
}  // namespace earth_radius
namespace WGS84
{
constexpr double A = 6378137.0;        // 长半轴 米
constexpr double INVF = 298.257223563;  // 扁率的倒数
constexpr double F = (1.0 / INVF);      // 扁率
constexpr double B = (A * (1.0 - F));  // 短半轴
constexpr double E2 = (2 * F - F * F); // 第一偏心扁率的平方
constexpr double GM = 3.986004418e14;  // 地心引力常数 立方米/秒的平方
constexpr double W = 7.2921151467e-5; // 自转角速度 弧度每秒
constexpr double J2 = 1.08262983226e-3;
}  // namespace WGS84
namespace CGCS2000
{
// CGCS2000 大多数参数跟WGS84 一致
constexpr double A = 6378137.0;
constexpr double INVF = 298.257222101;
constexpr double F = (1.0 / INVF);
constexpr double B = (A * (1.0 - F));
constexpr double E2 = (2 * F - F * F);
constexpr double GM = 3.9860044181e14;  // 地心引力常数 立方米/秒的平方
constexpr double W = 7.2921151467e-5;      // 自转角速度 弧度每秒
constexpr double J2 = 1.08262983226e-3;     // 重力场谐系数
}  // namespace CGCS2000
namespace XIAN80
{

constexpr double A = 6378140.0;    // 长半轴
constexpr double INVF = 298.257;    // 扁率的倒数
constexpr double F = (1.0 / INVF);  // 扁率
constexpr double B = (A * (1.0 - F));
constexpr double E2 = (2 * F - F * F);
}  // namespace XIAN80
namespace BEIJING54
{

constexpr double A = 6378245.0;    // 长半轴
constexpr double INVF = 298.3;      // 扁率的倒数
constexpr double F = (1.0 / INVF);  // 扁率
constexpr double B = (A * (1.0 - F));
constexpr double E2 = (2 * F - F * F);
}  // namespace BEIJING54

}  // namespace geo
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_GEO_CONST_H
