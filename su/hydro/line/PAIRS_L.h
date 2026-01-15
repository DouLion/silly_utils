/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://yyy.tianxhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: PAIRS_L.h
 * @description: PAIRS_L 头文件
 *                 可以根据 x * 1000 转为 整数,
 *                 然后 用 std::map<long long, double> 存储数据
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef PAIRS_L_H
#define PAIRS_L_H
#include <hydro/line/ALGO_L.h>
class MultiPairsL;
class PaPR_L;
class RZ2F_L;
class PairsL
{
    friend class MultiPairsL;
    friend class PaPR_L;
    friend class RZ2F_L;

  public:
    PairsL() = default;
    explicit PairsL(const double& xScale);
    /**
     * @brief: 添加一组xy关系,
     *  这里应该根据x的值动态插入,而不是直接放到最后
     * @param x --
     * @param y ---
     */
    void AddData(const double& x, const double& y);

    /**
     * @brief: 设置数据
     * @param: XYs xy关系线 [x0,y0, x1, y1 .... xn, yn]
     */
    void SetData(const std::vector<double>& XYs);

    /**
     * @brief: 设置数据
     * @param: Xs --
     * @param: Ys ---  与--一一对应
     */
    void SetData(const std::vector<double>& Xs, const std::vector<double>& Ys);

    /**
     * @brief: 清空数据
     */
    void Clear();

    /**
     * @brief: 根据x的由小到大排序
     */
    void SortByX();

    /**
     * @brief: 根据y的由小到大排序
     */
    void SortByY();

  protected:

    double GetMaxX() const;
    double GetMaxY() const;
    /**
     * 根据x插值计算y
     * @param x --
     * @param type 插值方式
     * @return
     */
    double GetYFromX(const double& x, const int& type = 4) const;

    /**
     * 根据y, 插值计算x
     * @param y ---
     * @param type 插值方式
     * @return
     */
    double GetXFromY(const double& y, const int& type = 4) const;

  protected:
    std::vector<double> m_Xs;
    std::vector<double> m_Ys;
    double m_xScale = 1.0;
};

#endif  // PAIRS_L_H
