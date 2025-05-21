/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-10-11
 * @file: silly_grid_index.h
 * @description: 网格点索引统一
 * @version: v1.0.1 2024-10-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_GRID_INDEX_H
#define SILLY_UTILS_SILLY_GRID_INDEX_H
#include <su_marco.h>
struct llpoint
{
    double lon = 0;
    double lat = 0;
};
class silly_cn_grid
{
  public:
    /// <summary>
    ///  计算给定经纬度对应点的序号,从1开始 0或者小数表示无效
    /// </summary>
    /// <param name="lgtd"></param>
    /// <param name="lttd"></param>
    /// <param name="step">最小0.01</param>
    /// <returns></returns>
    static int64_t index(const llpoint& point, const double& step);

    /// <summary>
    ///  计算给定序号对应点的经纬度
    /// </summary>
    /// <param name="index"></param>
    /// <param name="step"></param>
    /// <returns></returns>
    static llpoint point(const int64_t& index, const double& step);

    /// <summary>
    ///  计算给定序号和经纬度, 推算步长
    /// </summary>
    /// <param name="index"></param>
    /// <param name="step"></param>
    static double step(const llpoint& point, const int64_t& index);
};
class silly_grid_index
{
    /*
     * 以 北纬3度到北纬54度 东经74度到东经135度 , 先从西到东 然后从南向北构建全国的网格点索引
     * 第一个点序号为1  在此范围之外的点序号为0
     * */

  public:
    /// <summary>
    ///  计算给定经纬度对应点的序号
    /// </summary>
    /// <param name="lgtd"></param>
    /// <param name="lttd"></param>
    /// <param name="step"></param>
    /// <returns></returns>
    static int64_t index(const double& lgtd, const double& lttd, const double& step);

    /// <summary>
    ///  计算给定序号对应点的经纬度
    /// </summary>
    /// <param name="index"></param>
    /// <param name="step"></param>
    /// <returns></returns>
    static std::pair<double, double> point(const int64_t& index, const double& step);

    /// <summary>
    /// 检查给定经纬度对应点的序号
    /// </summary>
    /// <param name="lgtd"></param>
    /// <param name="lttd"></param>
    /// <param name="step"></param>
    /// <returns></returns>
    static int64_t checked_index(const double& lgtd, const double& lttd, const double& step);

    /// <summary>
    /// 检查给定序号对应点的经纬度
    /// </summary>
    /// <param name="index"></param>
    /// <param name="step"></param>
    /// <returns></returns>
    static std::pair<double, double> checked_point(const int64_t& index, const double& step);

    /// <summary>
    /// 根据自定义上下左右和起始序号 计算给定经纬度对应点的序号
    /// </summary>
    /// <param name="lgtd"></param>
    /// <param name="lttd"></param>
    /// <param name="step"></param>
    /// <param name="minp"></param>
    /// <param name="maxp"></param>
    /// <param name="bidx"></param>
    /// <returns></returns>
    static int64_t customize_index(const double& lgtd, const double& lttd, const double& step, std::pair<double, double> minp, std::pair<double, double> maxp, const int64_t& bidx);

    /// <summary>
    /// 根据自定义上下左右和起始序号 计算给定序号对应点的经纬度
    /// </summary>
    /// <param name="index"></param>
    /// <param name="step"></param>
    /// <param name="minp">left, bottom </param>
    /// <param name="maxp">right, top</param>
    /// <param name="bidx">起始索引,从 0 开始 或者从 任意值开始</param>
    /// <returns></returns>
    static std::pair<double, double> customize_point(const int64_t& index, const double& step, std::pair<double, double> minp, std::pair<double, double> maxp, const int64_t& bidx);
};

#endif  // SILLY_UTILS_SILLY_GRID_INDEX_H
