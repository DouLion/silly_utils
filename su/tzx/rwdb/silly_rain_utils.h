/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-06-05
 * @file: silly_rain_utils.h
 * @description: silly_rain_utils 类声明
 * @version: v1.0.1 2025-06-05 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_RAIN_UTILS_H
#define SILLY_UTILS_SILLY_RAIN_UTILS_H
#include <su_macro.h>
namespace su
{
class RainUtils
{
  public:
    /// @brief 根据降雨值动态调整权重
    /// 如果附近几个站都有大雨,数值小的雨量站应该剔除
    /// 如果附近几个站也是小雨, 那么计算结果始终不会产生雨量告警
    /// @param[in] stcd2weight 雨量站对应的权重
    /// @param[in] stcd2rain 雨量站对应的降雨量
    /// @param[in] ignore 忽略的雨量值
    /// @return 调整后的权重
    static std::map<std::string, double> AdjustWeight(const std::map<std::string, double>& stcd2weight, const std::map<std::string, double>& stcd2rain, const double& ignore = 0.1);

    /// 计算加权雨量
    /// @brief
    /// @param[in] stcd2weight
    /// @param[in] stcd2rain
    /// @return 加权雨量
    static double WeightedRain(const std::map<std::string, double>& stcd2weight, const std::map<std::string, double>& stcd2rain);
};
}  // namespace su

#endif  // SILLY_UTILS_SILLY_RAIN_UTILS_H
