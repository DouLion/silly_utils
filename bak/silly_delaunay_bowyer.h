/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-03
 * @file: silly_delaunay_bowyer.h
 * @description: silly_delaunay_bowyer  Bowyer-Watson 算法
 *  https://github.com/jbegaint/delaunay-cpp/blob/master/delaunay.hpp
 * @version: v1.0.1 2024-09-03 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_DELAUNAY_BOWYER_H
#define SILLY_UTILS_SILLY_DELAUNAY_BOWYER_H
#include <geo/triangular/silly_delaunay.h>

/// <summary>
/// Bowyer-Watson 算法实现的三角化,目前效率有点问题
/// 目前采用逐点插入方式生成的Delaunay三角网的算法主要基于Bowyer-Watson算法，Bowyer-Watson算法的主要步骤如下：
/// 1）建立初始三角网格：针对给定的点集V, 找到一个包含该点集的矩形R, 我们称R为辅助窗口。连接R的任意一条对角线，形成两个三角形，作为初始Delaunay三角网格。
/// 2）逐点插入：假设目前已经有一个Delaunay三角网格T, 现在在它里面再插入一个点P, 需要找到该点P所在的三角形。从P所在的三角形开始，
/// 搜索该三角形的邻近三角形，并进行空外接圆检测。找到外接圆包含点P的所有的三角形并删除这些三角形，形成一个包含P的多边形空腔，
/// 我们称之为Delaunay空腔。然后连接P与Delaunay腔的每一个顶点，形成新的Delaunay三角网格。
///	3）删除辅助窗口R:重复步骤2）, 当点集V中所有点都已经插入到三角形网格中后，将顶点包含辅助窗口R的三角形全部删除。
/// 在这些步骤中，快速定位点所在的三角形、确定点的影响并构建Delaunay腔的过程是每插入一个点都会进行的。随着点数的增加，三角形数目增加很快，因此缩短这两个过程的计算时间，是提高算法效率的关键。
/// </summary>

class silly_delaunay_bowyer : public silly_delaunay

{
  public:
    silly_delaunay_bowyer() = default;
    ~silly_delaunay_bowyer() = default;

    void points(std::vector<silly_dt_point>& pts) override;

    void triangulate() override;

  private:
    void init_super_triangle();

    bool contains_super_tri(const silly_dt_tri& tri) const;

  private:
    double ddx = 0;
    double ddy = 0;
};

#endif  // SILLY_UTILS_SILLY_DELAUNAY_BOWYER_H
