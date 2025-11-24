/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-30
 * @file: silly_akima.h
 * @description: akima插值
 * @version: v1.0.1 2024-12-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_AKIMA_H
#define SILLY_UTILS_SILLY_AKIMA_H
#include <su_marco.h>
namespace silly
{
namespace interpolation
{
class akima
{
  public:
    /// <summary>
    /// 计算akima导数
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    std::vector<double> derivatives(const std::vector<double>& x, const std::vector<double>& y);
    std::vector<double> derivatives(const std::vector<std::pair<double, double>>& points);

    /// <summary>
    /// 计算插值结果
    /// </summary>
    /// <param name="x"></param>
    /// <returns></returns>
    double calc(const double& x) const;

  private:
    /// <summary>
    /// 计算斜率
    /// </summary>
    void slopes();

  private:
    std::vector<double> m_derivatives;
    std::vector<double> m_slopes;
    std::vector<double> m_x;
    std::vector<double> m_y;
};

}  // namespace interpolation
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_AKIMA_H
