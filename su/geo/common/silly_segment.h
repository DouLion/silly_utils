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

    bool intersect(const suSegment& rh) const;

    std::optional<suPoint> intersection(const suSegment& rh) const;

};

#endif  // SILLY_SEGMENT_H
