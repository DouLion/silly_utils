/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-30
 * @file: silly_bilinear.h
 * @description: 双线性插值 类声明
 * @version: v1.0.1 2024-12-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_BILINEAR_H
#define SILLY_UTILS_SILLY_BILINEAR_H
namespace silly
{
namespace interpolation
{
class bilinear
{
  public:
    /// <summary>
    /// 
    /// q10  q11
    /// q00  q01 上下可以逆序,但是dx 和 dy要与q0 - q3的值要一致
    /// </summary>
    /// <param name="q00">左下角的值 (0, 0)</param>
    /// <param name="q01">右下角的值 (dx, 0)</param>
    /// <param name="q10">左上角的值 (0, dy)</param>
    /// <param name="q11">右上角的值 (dx, dy)</param>
    /// <param name="dx">距离 q00q10 的距离权重</param>
    /// <param name="dy">距离 q00q01 的距离权重</param>
    /// <returns></returns>
    static double calc(double q00, double q01, double q10, double q11, double dx, double dy);

  private:
};
}  // namespace interpolation
}  // namespace silly

#endif  //SILLY_UTILS_SILLY_BILINEAR_H
