/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-01-15
 * @file: silly_math_const.h
 * @description: 数学计算中的常量 对于类型安全和更好的调试体验，建议使用const或constexpr
 * @version: v1.0.1 2025-01-15 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_MATH_CONST_H
#define SILLY_UTILS_SILLY_MATH_CONST_H

namespace silly
{
namespace math
{
// π (圆周率)
constexpr double pi = 3.1415926535897932384626433832795028841971693993751;

// e (自然对数的底)
constexpr double e = 2.71828182845904523536;

// γ (欧拉-马斯刻若尼常数)
constexpr double euler_mascheroni = 0.57721566490153286060651209;

// φ (黄金比例)
constexpr double golden_ratio = 1.61803398874989484820;

// √2 (根号二)
constexpr double sqrt_2 = 1.4142135623730950488016887242097;

// Apéry's constant (ζ(3)) 黎曼ζ函数在3处的值
constexpr double apery_constant = 1.20205690315959428539;

// Catalan's constant (G) 由快速收敛的级数定义
constexpr double catalan_constant = 0.91596559417721901505;

// Feigenbaum constants (δ 和 α) 混沌理论中的分叉点间隔比
constexpr double feigenbaum_delta = 4.66920160910299067185;
constexpr double feigenbaum_alpha = 2.50290787509589282228;

// Khinchin's constant (K) 几乎所有的实数连分数展开式中部分商的几何平均值
constexpr double khinchin_constant = 2.68545200106530644531;
}  // namespace math

}  // namespace silly

#endif  // SILLY_UTILS_SILLY_MATH_CONST_H
