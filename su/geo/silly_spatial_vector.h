//
// Created by dell on 2025/9/23.
//

#ifndef SILLY_SPATIAL_VECTOR_H
#define SILLY_SPATIAL_VECTOR_H

// 空间向量
#include <geo/silly_geo.h>
enum eSpatialSide : int
{
    /* 模拟数字键盘
       7 8 9
       4 5 6
       1 2 3
    */
    LEFT_BOTTOM = 1,
    DOWN_TO = 2,
    RIGHT_BOTTOM = 3,
    LEFT = 4,
    ON = 5,
    RIGHT = 6,
    LEFT_TOP = 7,
    UP_TO = 8,
    RIGHT_TOP = 9

};

class suSpatialVec
{
  public:
    /// <summary>
    /// 线段 p1->p2, p3 为线段上一点, p3->p4 垂直于 p1->p2向左, 并且p3->p4 距离为distance
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="p3"></param>
    /// <param name="distance"></param>
    /// <returns></returns>
    static suPoint LeftDistTo(const suPoint& p1, const suPoint& p2, const suPoint& p3, const double& distance);

    static suPoint RightDistTo(const suPoint& p1, const suPoint& p2, const suPoint& p3, const double& distance);

    /// <summary>
    /// 点p在向量from1->to的 左侧或者右侧, 或者其所在延长线上
    /// </summary>
    /// <param name="from"></param>
    /// <param name="to"></param>
    /// <param name="p"></param>
    /// <returns></returns>
    static eSpatialSide OnSide(const suPoint& from, const suPoint& to, const suPoint& p);

    // 向量减法
    static suPoint SubV(const suPoint& a, const suPoint& b);

    // 向量加法
    static suPoint AddV(const suPoint& a, const suPoint& b);

    // 向量数乘
    static suPoint ScaleV(const suPoint& v, const double& s);

    // 向量单位化
    static suPoint NormalizeV(const suPoint& v);

    // 顺时针旋转90度(用于获取右侧法向量)
    static suPoint RotateRight90(const suPoint& v);

    // 逆时针旋转90度(用于获取左侧法向量)
    static suPoint RotateLeft90(const suPoint& v);

    /// <summary>
    /// 角平分线左侧点
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="p3"></param>
    /// <param name="distance"></param>
    /// <returns></returns>
    static suPoint LeftAngleBisector(const suPoint& p1, const suPoint& p2, const suPoint& p3, const double& distance);

    /// <summary>
    /// 角平分线右侧点
    /// </summary>
    /// <param name="p1"></param>
    /// <param name="p2"></param>
    /// <param name="p3"></param>
    /// <param name="distance"></param>
    /// <returns></returns>
    static suPoint RightAngleBisector(const suPoint& p1, const suPoint& p2, const suPoint& p3, const double& distance);
};

#endif  // SILLY_SPATIAL_VECTOR_H
