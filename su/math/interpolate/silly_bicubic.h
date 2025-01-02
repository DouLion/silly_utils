/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-30
 * @file: silly_cubic.h
 * @description: silly_cubic 类声明
 * @version: v1.0.1 2024-12-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_BICUBIC_H
#define SILLY_UTILS_SILLY_BICUBIC_H
namespace silly
{
namespace interpolation
{
class bicubic
{
  public:
    /// <summary>
    /// 0 1 2 3
    /// 4 5 6 7
    /// 8 9 10 11
    /// </summary>
    /// <param name="grids">顺序如上</param>
    /// <param name="dx">距离权重</param>
    /// <param name="dy"></param>
    /// <returns></returns>
    static double calc(const std::vector<double>& grids, const double& dx, const double& dy);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="t"></param>
    /// <returns></returns>
    static double catmull_rom(const double& t);

  private:
};
}  // namespace interpolation
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_BICUBIC_H
