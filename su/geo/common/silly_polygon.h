/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: silly_polygon.cpp
 * @description: silly_polygon 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_POLYGON_H
#define SILLY_POLYGON_H
#include <geo/common/silly_point.h>
#include <geo/common/silly_rect.h>

class suRing
{
public:
    std::vector<suPoint> points;
    int is_outer{1};

    /**
     * 计算面积
     * @return
     */
    double area() const;

    /**
     * 获取最小包围框
     * @return
     */
    suRect bound() const;

    /**
     * 是否包含点
     * @param p
     * @return
     */
    bool intersect(const suPoint& p) const;
};

/****************************************/
/// 面
/****************************************/
class suPoly
{
public:
    suRing outer; // 外环
    std::vector<suRing> holes; // 内环, 孔, 洞

    /**
     * 计算面积
     * @return
     */
    double area() const;

    /**
     * 获取最小包围框
     * @return
     */
    suRect bound() const;

    /**
     * 是否包含点
     * @param p
     * @return
     */
    bool intersect(const suPoint& p) const;
};

/****************************************/
/// 多面
/****************************************/
class suMultiPoly : public suVecTemplate<suPoly>
{
public:
    /**
     * 计算面积
     * @return
     */
    double area() const;

    /**
     * 获取最小包围框
     * @return
     */
    suRect bound() const;

    /**
     * 是否包含点
     * @param p
     * @return
     */
    bool intersect(const suPoint& p) const;
};
#endif  // SILLY_POLYGON_H
