/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-23
 * @file: silly_grid_algo.cpp
 * @description: 整理网格点处理的相关算法
 * @version: v1.0.1 2025-12-23 dou li yang
 */
#ifndef SILLY_GRID_ALGO_H
#define SILLY_GRID_ALGO_H
#include <metadata/silly_tzx_grid.h>
#include <geo/silly_geo_utils.h>
#include <geo/silly_vector_to_raster.h>
class suGridAlgo
{
  public:
    typedef std::function<float(const std::vector<float>&)> summary_func;

    /**
     * @brief: 获取点p的kn个邻域内, 满足summary_func的点的统计值
     * @param tgrd 网格信息
     * @param p 点坐标, tgrd 保持同坐标系
     * @param kn kernal 数量. 不超过10
     * @param func 统计函数
     * @return
     */
    static float SummaryPoint(const silly_tzx_grid& tgrd, const suPoint& p, const uint32_t& kn, const summary_func& func = suGridAlgo::average);

    /**
     * @brief: 获取mtx矩阵中第r行第c列的kn个邻域内, 满足summary_func的点的统计值
     * @param mtx 矩阵信息
     * @param r 行号
     * @param c 列号
     * @param kn kernal 数量. 不超过10
     * @param func 统计函数
     * @return
     */
    static float SummaryPoint(const suFMatrix& mtx, const int& r, const int& c, const uint32_t& kn, const summary_func& func = suGridAlgo::average);

    /**
     * @brief: 获取mp的多面内, 满足summary_func的点的统计值
     * @param tgrd
     * @param mp
     * @param func 统计函数
     * @return
     */
    static float SummaryMultiPoly(const silly_tzx_grid& tgrd, const suMultiPoly& mp, const summary_func& func = suGridAlgo::average);

    /**
     *  @brief: 获取mtx矩阵中, 多面mp内, 满足summary_func的点的统计值
     * @param mtx
     * @param sps
     * @param func 统计函数
     * @return
     */
    static float SummaryMultiPoly(const suFMatrix& mtx, const XScanLine::ScanPairs& sps, const summary_func& func = suGridAlgo::average);

    // TODO: 刘情  补充 mask 掩膜统计的方式

    /**
     * @brief: summary_func 的常用实现
     * @param vec
     * @return
     */
    static float average(const std::vector<float>& vec)
    {
        if (vec.empty())
        {
            return 0;
        }
        float ret = 0;
        for (auto v : vec)
        {
            ret += v;
        }
        return ret / static_cast<float>(vec.size());
    }
    static float max(const std::vector<float>& vec)
    {
        if (vec.empty())
        {
            return 0;
        }
        return *std::max_element(vec.begin(), vec.end());
    }
    static float min(const std::vector<float>& vec)
    {
        if (vec.empty())
        {
            return 0;
        }
        return *std::min_element(vec.begin(), vec.end());
    }
};
#endif  // SILLY_GRID_ALGO_H
