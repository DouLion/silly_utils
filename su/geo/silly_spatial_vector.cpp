//
// Created by dell on 2025/9/23.
//

#include "silly_spatial_vector.h"

suPoint suSpatialVec::LeftDistTo(const suPoint& p1, const suPoint& p2, const suPoint& p3, const double& distance)
{
    // 方向向量(dx, dy)
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    // 单位化法向量(-dy, dx)
    double length = std::sqrt(dx * dx + dy * dy);
    double nx = -dy / length;
    double ny = dx / length;

    // 新点P4的坐标 = 点P3的坐标 + 单位法向量 * 距离
    suPoint p4;
    p4.x = p3.x + nx * distance;
    p4.y = p3.y + ny * distance;

    return p4;
}

suPoint suSpatialVec::RightDistTo(const suPoint& p1, const suPoint& p2, const suPoint& p3, const double& distance)
{
    // 方向向量(dx, dy)
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    // 单位化法向量(-dy, dx)
    double length = std::sqrt(dx * dx + dy * dy);
    double nx = dy / length;
    double ny = -dx / length;

    // 新点P4的坐标 = 点P3的坐标 + 单位法向量 * 距离
    suPoint p4;
    p4.x = p3.x + nx * distance;
    p4.y = p3.y + ny * distance;

    return p4;
}
eSpatialSide suSpatialVec::OnSide(const suPoint& from, const suPoint& to, const suPoint& p)
{
    double z = (to.y - from.y) * (p.x - from.x) - (to.x - from.x) * (p.y - from.y);

    if (z < 0)
    {
        // std::cout << "left" << std::endl;
        return eSpatialSide::LEFT;  // 点P在P1->P2的左侧
    }
    if (z > 0)
    {
        // std::cout << "right" << std::endl;
        return eSpatialSide::RIGHT;  // 点P在P1->P2的右侧
    }
    {
        // std::cout << "on" << std::endl;
        return eSpatialSide::ON;  // 点P在线段P1P2上或其延长线上
    }
}

// 计算两点之间的向量
suPoint suSpatialVec::SubV(const suPoint& a, const suPoint& b)
{
    return {a.x - b.x, a.y - b.y};
}

// 向量加法
suPoint suSpatialVec::AddV(const suPoint& a, const suPoint& b)
{
    return {a.x + b.x, a.y + b.y};
}

// 向量数乘
suPoint suSpatialVec::ScaleV(const suPoint& v, const double& s)
{
    return {v.x * s, v.y * s};
}

// 向量单位化
suPoint suSpatialVec::NormalizeV(const suPoint& v)
{
    double len = std::sqrt(v.x * v.x + v.y * v.y);
    return {v.x / len, v.y / len};
}

suPoint suSpatialVec::RotateRight90(const suPoint& v)
{
    return {v.y, -v.x};  // 顺时针旋转90度
}

suPoint suSpatialVec::RotateLeft90(const suPoint& v)
{
    return {-v.y, v.x};  // 逆时针旋转90度
}

/// <summary>
/// 角平分线左侧点
/// </summary>
/// <param name="p1"></param>
/// <param name="p2"></param>
/// <param name="p3"></param>
/// <param name="distance"></param>
/// <returns></returns>
suPoint suSpatialVec::LeftAngleBisector(const suPoint& p1, const suPoint& p2, const suPoint& p3, const double& distance)
{
    // 计算方向向量
    suPoint v1 = SubV(p2, p1);  // P1->P2
    suPoint v2 = SubV(p3, p2);  // P2->P3

    // 单位化
    v1 = NormalizeV(v1);
    v2 = NormalizeV(v2);

    // 角平分线方向向量
    suPoint bisectorDir = NormalizeV(AddV(v1, v2));

    // 获取左侧法向量
    suPoint leftNormal = RotateLeft90(bisectorDir);

    // 根据距离计算P4位置
    suPoint p4 = AddV(p2, ScaleV(leftNormal, distance));

    return p4;
}

suPoint suSpatialVec::RightAngleBisector(const suPoint& p1, const suPoint& p2, const suPoint& p3, const double& distance)

{
    // 计算方向向量
    suPoint v1 = SubV(p2, p1);  // P1->P2
    suPoint v2 = SubV(p3, p2);  // P2->P3

    // 单位化
    v1 = NormalizeV(v1);
    v2 = NormalizeV(v2);

    // 角平分线方向向量
    suPoint bisectorDir = NormalizeV(AddV(v1, v2));

    // 获取右侧法向量
    suPoint rightNormal = RotateRight90(bisectorDir);

    // 根据距离计算P4位置
    suPoint p4 = AddV(p2, ScaleV(rightNormal, distance));

    return p4;
}