//
// Created by dell on 2025/9/23.
//

#include "silly_spatial_vector.h"

silly_point silly_spatial_vector::left_distance(const silly_point& p1, const silly_point& p2, const silly_point& p3, const double& distance)
{
    // 方向向量(dx, dy)
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    // 单位化法向量(-dy, dx)
    double length = std::sqrt(dx * dx + dy * dy);
    double nx = -dy / length;
    double ny = dx / length;

    // 新点P4的坐标 = 点P3的坐标 + 单位法向量 * 距离
    silly_point p4;
    p4.x = p3.x + nx * distance;
    p4.y = p3.y + ny * distance;

    return p4;
}

silly_point silly_spatial_vector::right_distance(const silly_point& p1, const silly_point& p2, const silly_point& p3, const double& distance)
{
    // 方向向量(dx, dy)
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    // 单位化法向量(-dy, dx)
    double length = std::sqrt(dx * dx + dy * dy);
    double nx = dy / length;
    double ny = -dx / length;

    // 新点P4的坐标 = 点P3的坐标 + 单位法向量 * 距离
    silly_point p4;
    p4.x = p3.x + nx * distance;
    p4.y = p3.y + ny * distance;

    return p4;
}
eSpatialSide silly_spatial_vector::side_ot_vec(const silly_point& from, const silly_point& to, const silly_point& p)
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
silly_point silly_spatial_vector::vectorSub(const silly_point& a, const silly_point& b)
{
    return {a.x - b.x, a.y - b.y};
}

// 向量加法
silly_point silly_spatial_vector::vectorAdd(const silly_point& a, const silly_point& b)
{
    return {a.x + b.x, a.y + b.y};
}

// 向量数乘
silly_point silly_spatial_vector::vectorScale(const silly_point& v, const double& s)
{
    return {v.x * s, v.y * s};
}

// 向量单位化
silly_point silly_spatial_vector::normalize(const silly_point& v)
{
    double len = std::sqrt(v.x * v.x + v.y * v.y);
    return {v.x / len, v.y / len};
}

silly_point silly_spatial_vector::rotateRIGHT90(const silly_point& v)
{
    return {v.y, -v.x};  // 顺时针旋转90度
}

silly_point silly_spatial_vector::rotateLEFT90(const silly_point& v)
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
silly_point silly_spatial_vector::angle_bisector_left(const silly_point& p1, const silly_point& p2, const silly_point& p3, const double& distance)
{
    // 计算方向向量
    silly_point v1 = vectorSub(p2, p1);  // P1->P2
    silly_point v2 = vectorSub(p3, p2);  // P2->P3

    // 单位化
    v1 = normalize(v1);
    v2 = normalize(v2);

    // 角平分线方向向量
    silly_point bisectorDir = normalize(vectorAdd(v1, v2));

    // 获取左侧法向量
    silly_point leftNormal = rotateLEFT90(bisectorDir);

    // 根据距离计算P4位置
    silly_point p4 = vectorAdd(p2, vectorScale(leftNormal, distance));

    return p4;
}

silly_point silly_spatial_vector::angle_bisector_right(const silly_point& p1, const silly_point& p2, const silly_point& p3, const double& distance)

{
    // 计算方向向量
    silly_point v1 = vectorSub(p2, p1);  // P1->P2
    silly_point v2 = vectorSub(p3, p2);  // P2->P3

    // 单位化
    v1 = normalize(v1);
    v2 = normalize(v2);

    // 角平分线方向向量
    silly_point bisectorDir = normalize(vectorAdd(v1, v2));

    // 获取右侧法向量
    silly_point rightNormal = rotateRIGHT90(bisectorDir);

    // 根据距离计算P4位置
    silly_point p4 = vectorAdd(p2, vectorScale(rightNormal, distance));

    return p4;
}