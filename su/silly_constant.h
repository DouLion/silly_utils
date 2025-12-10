/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-10
 * @file: silly_constant.h
 * @description: 常量文件 所有常量都放在次位置
 * @version: v1.0.1 2025-12-10 dou li yang
 */
#ifndef SILLY_CONSTANT_H
#define SILLY_CONSTANT_H

namespace MATH
{
// π (圆周率)
constexpr double PI_ = 3.1415926535897932384626433832795028841971693993751;

// e (自然对数的底)
constexpr double E_ = 2.71828182845904523536;

// γ (欧拉-马斯刻若尼常数)
constexpr double EULER_MASCHERONI_ = 0.57721566490153286060651209;

// φ (黄金比例)
constexpr double GOLDEN_RATIO_ = 1.61803398874989484820;

// √2 (根号二)
constexpr double SQRT_2_ = 1.4142135623730950488016887242097;

// Apéry's constant (ζ(3)) 黎曼ζ函数在3处的值
constexpr double APERY_CONSTANT_ = 1.20205690315959428539;

// Catalan's constant (G) 由快速收敛的级数定义
constexpr double CATALAN_CONSTANT_ = 0.91596559417721901505;

// Feigenbaum constants (δ 和 α) 混沌理论中的分叉点间隔比
constexpr double FEIGENBAUM_DELTA_ = 4.66920160910299067185;
constexpr double FEIGENBAUM_ALPHA_ = 2.50290787509589282228;

// Khinchin's constant (K) 几乎所有的实数连分数展开式中部分商的几何平均值
constexpr double KHINCHIN_CONSTANT_ = 2.68545200106530644531;
}  // namespace math

namespace EARTH
{
static constexpr double MEAN = 6371000.0;        // 平均半径 米
static constexpr double EQUATORIAL = 6378137.0;  // 赤道半径 米
static constexpr double POLAR = 6356752.3142;    // 极地半径 米
}  // namespace suEarthRadius

namespace WGS84
{
static constexpr double A = 6378137.0;         // 长半轴 米
static constexpr double INVF = 298.257223563;  // 扁率的倒数
static constexpr double F = (1.0 / INVF);      // 扁率
static constexpr double B = (A * (1.0 - F));   // 短半轴
static constexpr double E2 = (2 * F - F * F);  // 第一偏心扁率的平方
static constexpr double GM = 3.986004418e14;   // 地心引力常数 立方米/秒的平方
static constexpr double W = 7.2921151467e-5;   // 自转角速度 弧度每秒
static constexpr double J2 = 1.08262983226e-3;
}  // namespace suWGS84

namespace CGCS2000
{
// CGCS2000 大多数参数跟WGS84 一致
static constexpr double A = 6378137.0;
static constexpr double INVF = 298.257222101;
static constexpr double F = (1.0 / INVF);
static constexpr double B = (A * (1.0 - F));
static constexpr double E2 = (2 * F - F * F);
static constexpr double GM = 3.9860044181e14;   // 地心引力常数 立方米/秒的平方
static constexpr double W = 7.2921151467e-5;    // 自转角速度 弧度每秒
static constexpr double J2 = 1.08262983226e-3;  // 重力场谐系数
}  // namespace suCGCS2000

namespace XIAN80
{

static constexpr double A = 6378140.0;     // 长半轴
static constexpr double INVF = 298.257;    // 扁率的倒数
static constexpr double F = (1.0 / INVF);  // 扁率
static constexpr double B = (A * (1.0 - F));
static constexpr double E2 = (2 * F - F * F);
}  // namespace suXIAN80

namespace BEIJING54
{

static constexpr double A = 6378245.0;     // 长半轴
static constexpr double INVF = 298.3;      // 扁率的倒数
static constexpr double F = (1.0 / INVF);  // 扁率
static constexpr double B = (A * (1.0 - F));
static constexpr double E2 = (2 * F - F * F);
}  // namespace BEIJING54

#endif