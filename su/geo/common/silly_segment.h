/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-08-08
 * @file: suSegment.cpp
 * @description: suSegment 头文件
 * @version: v1.0.1 2025-08-08 dou li yang
 */
#ifndef SILLY_SEGMENT_H
#define SILLY_SEGMENT_H
#include <geo/common/silly_point.h>

class suSegment
{
public:
    suPoint p0;
    suPoint p1;
  public:
    suSegment() = default;
    ~suSegment() = default;
    suSegment(const suPoint& _p0, const suPoint& _p1)
    {
        p0 = _p0;
        p1 = _p1;
    }

    /**
     * 两个线段是否相交
     * @param rh
     * @return
     */
    bool intersect(const suSegment& rh) const;

    /**
     * 两个线段的交点
     * @param rh
     * @return
     */
    std::optional<suPoint> intersection(const suSegment& rh) const;

    /**
     * 获取点到线段的最短距离
     * @param p
     * @param in_seg true, 限制在线段范围内, false 考虑延迟线
     * @return
     */
    double distance(const suPoint& p, const bool& in_seg = false) const;

};

#endif  // SILLY_SEGMENT_H
